/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "solvercvode_p.h"
#include "utils.h"

#include <algorithm>

#include "sundialsbegin.h"
#include "cvodes/cvodes.h"
#include "cvodes/cvodes_bandpre.h"
#include "cvodes/cvodes_diag.h"
#include "nvector/nvector_serial.h"
#include "sunlinsol/sunlinsol_band.h"
#include "sunlinsol/sunlinsol_dense.h"
#include "sunlinsol/sunlinsol_spbcgs.h"
#include "sunlinsol/sunlinsol_spgmr.h"
#include "sunlinsol/sunlinsol_sptfqmr.h"
#include "sunnonlinsol/sunnonlinsol_fixedpoint.h"
#include "sundialsend.h"

namespace libOpenCOR {

// Integration methods.

const std::string SolverCvode::Impl::ADAMS_MOULTON_METHOD = "Adams-Moulton"; // NOLINT
const std::string SolverCvode::Impl::BDF_METHOD = "BDF"; // NOLINT

// Iteration types.

const std::string SolverCvode::Impl::FUNCTIONAL_ITERATION_TYPE = "Functional"; // NOLINT
const std::string SolverCvode::Impl::NEWTON_ITERATION_TYPE = "Newton"; // NOLINT

// Linear solvers.

const std::string SolverCvode::Impl::DENSE_LINEAR_SOLVER = "Dense"; // NOLINT
const std::string SolverCvode::Impl::BANDED_LINEAR_SOLVER = "Banded"; // NOLINT
const std::string SolverCvode::Impl::DIAGONAL_LINEAR_SOLVER = "Diagonal"; // NOLINT
const std::string SolverCvode::Impl::GMRES_LINEAR_SOLVER = "GMRES"; // NOLINT
const std::string SolverCvode::Impl::BICGSTAB_LINEAR_SOLVER = "BiCGStab"; // NOLINT
const std::string SolverCvode::Impl::TFQMR_LINEAR_SOLVER = "TFQMR"; // NOLINT

// Preconditioners.

const std::string SolverCvode::Impl::NO_PRECONDITIONER = "None"; // NOLINT
const std::string SolverCvode::Impl::BANDED_PRECONDITIONER = "Banded"; // NOLINT

// Properties information.

const std::string SolverCvode::Impl::ID = "KISAO:0000019"; // NOLINT
const std::string SolverCvode::Impl::NAME = "CVODE"; // NOLINT

const std::string SolverCvode::Impl::MAXIMUM_STEP_ID = "KISAO:0000467"; // NOLINT
const std::string SolverCvode::Impl::MAXIMUM_STEP_NAME = "Maximum step"; // NOLINT

const std::string SolverCvode::Impl::MAXIMUM_NUMBER_OF_STEPS_ID = "KISAO:0000415"; // NOLINT
const std::string SolverCvode::Impl::MAXIMUM_NUMBER_OF_STEPS_NAME = "Maximum number of steps"; // NOLINT

const std::string SolverCvode::Impl::INTEGRATION_METHOD_ID = "KISAO:0000475"; // NOLINT
const std::string SolverCvode::Impl::INTEGRATION_METHOD_NAME = "Integration method"; // NOLINT
const std::vector<std::string> SolverCvode::Impl::INTEGRATION_METHOD_LIST = {ADAMS_MOULTON_METHOD, BDF_METHOD}; // NOLINT
const std::string SolverCvode::Impl::INTEGRATION_METHOD_DEFAULT_VALUE = BDF_METHOD; // NOLINT

const std::string SolverCvode::Impl::ITERATION_TYPE_ID = "KISAO:0000476"; // NOLINT
const std::string SolverCvode::Impl::ITERATION_TYPE_NAME = "Iteration type"; // NOLINT
const std::vector<std::string> SolverCvode::Impl::ITERATION_TYPE_LIST = {FUNCTIONAL_ITERATION_TYPE, NEWTON_ITERATION_TYPE}; // NOLINT
const std::string SolverCvode::Impl::ITERATION_TYPE_DEFAULT_VALUE = NEWTON_ITERATION_TYPE; // NOLINT

const std::string SolverCvode::Impl::LINEAR_SOLVER_ID = "KISAO:0000477"; // NOLINT
const std::string SolverCvode::Impl::LINEAR_SOLVER_NAME = "Linear solver"; // NOLINT
const std::vector<std::string> SolverCvode::Impl::LINEAR_SOLVER_LIST = {DENSE_LINEAR_SOLVER, BANDED_LINEAR_SOLVER, DIAGONAL_LINEAR_SOLVER, GMRES_LINEAR_SOLVER, BICGSTAB_LINEAR_SOLVER, TFQMR_LINEAR_SOLVER}; // NOLINT
const std::string SolverCvode::Impl::LINEAR_SOLVER_DEFAULT_VALUE = DENSE_LINEAR_SOLVER; // NOLINT

const std::string SolverCvode::Impl::PRECONDITIONER_ID = "KISAO:0000478"; // NOLINT
const std::string SolverCvode::Impl::PRECONDITIONER_NAME = "Preconditioner"; // NOLINT
const std::vector<std::string> SolverCvode::Impl::PRECONDITIONER_LIST = {NO_PRECONDITIONER, BANDED_PRECONDITIONER}; // NOLINT
const std::string SolverCvode::Impl::PRECONDITIONER_DEFAULT_VALUE = BANDED_PRECONDITIONER; // NOLINT

const std::string SolverCvode::Impl::UPPER_HALF_BANDWIDTH_ID = "KISAO:0000479"; // NOLINT
const std::string SolverCvode::Impl::UPPER_HALF_BANDWIDTH_NAME = "Upper half-bandwidth"; // NOLINT

const std::string SolverCvode::Impl::LOWER_HALF_BANDWIDTH_ID = "KISAO:0000480"; // NOLINT
const std::string SolverCvode::Impl::LOWER_HALF_BANDWIDTH_NAME = "Lower half-bandwidth"; // NOLINT

const std::string SolverCvode::Impl::RELATIVE_TOLERANCE_ID = "KISAO:0000209"; // NOLINT
const std::string SolverCvode::Impl::RELATIVE_TOLERANCE_NAME = "Relative tolerance"; // NOLINT

const std::string SolverCvode::Impl::ABSOLUTE_TOLERANCE_ID = "KISAO:0000211"; // NOLINT
const std::string SolverCvode::Impl::ABSOLUTE_TOLERANCE_NAME = "Absolute tolerance"; // NOLINT

const std::string SolverCvode::Impl::INTERPOLATE_SOLUTION_ID = "KISAO:0000481"; // NOLINT
const std::string SolverCvode::Impl::INTERPOLATE_SOLUTION_NAME = "Interpolate solution"; // NOLINT

// Right-hand side function.

namespace {
int rhsFunction(double pVoi, N_Vector pStates, N_Vector pRates, void *pUserData)
{
    auto *userData = static_cast<SolverCvodeUserData *>(pUserData);

    userData->computeRates()(pVoi, N_VGetArrayPointer_Serial(pStates), N_VGetArrayPointer_Serial(pRates), userData->variables());

    return 0;
}
} // namespace

// Solver user data.

SolverCvodeUserData::SolverCvodeUserData(double *pVariables, SolverOde::ComputeRates pComputeRates)
    : mVariables(pVariables)
    , mComputeRates(pComputeRates)
{
}

double *SolverCvodeUserData::variables() const
{
    return mVariables;
}

SolverOde::ComputeRates SolverCvodeUserData::computeRates() const
{
    return mComputeRates;
}

// Solver.

SolverPtr SolverCvode::Impl::create()
{
    return std::shared_ptr<SolverCvode> {new SolverCvode {}};
}

std::vector<SolverPropertyPtr> SolverCvode::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGe0, MAXIMUM_STEP_ID, MAXIMUM_STEP_NAME,
                                     {},
                                     toString(MAXIMUM_STEP_DEFAULT_VALUE),
                                     true),
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGt0, MAXIMUM_NUMBER_OF_STEPS_ID, MAXIMUM_NUMBER_OF_STEPS_NAME,
                                     {},
                                     toString(MAXIMUM_NUMBER_OF_STEPS_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::List, INTEGRATION_METHOD_ID, INTEGRATION_METHOD_NAME,
                                     INTEGRATION_METHOD_LIST,
                                     INTEGRATION_METHOD_DEFAULT_VALUE,
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::List, ITERATION_TYPE_ID, ITERATION_TYPE_NAME,
                                     ITERATION_TYPE_LIST,
                                     ITERATION_TYPE_DEFAULT_VALUE,
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::List, LINEAR_SOLVER_ID, LINEAR_SOLVER_NAME,
                                     LINEAR_SOLVER_LIST,
                                     LINEAR_SOLVER_DEFAULT_VALUE,
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::List, PRECONDITIONER_ID, PRECONDITIONER_NAME,
                                     PRECONDITIONER_LIST,
                                     PRECONDITIONER_DEFAULT_VALUE,
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGe0, UPPER_HALF_BANDWIDTH_ID, UPPER_HALF_BANDWIDTH_NAME,
                                     {},
                                     toString(UPPER_HALF_BANDWIDTH_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGe0, LOWER_HALF_BANDWIDTH_ID, LOWER_HALF_BANDWIDTH_NAME,
                                     {},
                                     toString(LOWER_HALF_BANDWIDTH_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGe0, RELATIVE_TOLERANCE_ID, RELATIVE_TOLERANCE_NAME,
                                     {},
                                     toString(RELATIVE_TOLERANCE_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGe0, ABSOLUTE_TOLERANCE_ID, ABSOLUTE_TOLERANCE_NAME,
                                     {},
                                     toString(ABSOLUTE_TOLERANCE_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::Boolean, INTERPOLATE_SOLUTION_ID, INTERPOLATE_SOLUTION_NAME,
                                     {},
                                     toString(INTERPOLATE_SOLUTION_DEFAULT_VALUE),
                                     false),
    };
}

SolverCvode::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[MAXIMUM_STEP_ID] = toString(MAXIMUM_STEP_DEFAULT_VALUE);
    mProperties[MAXIMUM_NUMBER_OF_STEPS_ID] = toString(MAXIMUM_NUMBER_OF_STEPS_DEFAULT_VALUE);
    mProperties[INTEGRATION_METHOD_ID] = INTEGRATION_METHOD_DEFAULT_VALUE;
    mProperties[ITERATION_TYPE_ID] = ITERATION_TYPE_DEFAULT_VALUE;
    mProperties[LINEAR_SOLVER_ID] = LINEAR_SOLVER_DEFAULT_VALUE;
    mProperties[PRECONDITIONER_ID] = PRECONDITIONER_DEFAULT_VALUE;
    mProperties[UPPER_HALF_BANDWIDTH_ID] = toString(UPPER_HALF_BANDWIDTH_DEFAULT_VALUE);
    mProperties[LOWER_HALF_BANDWIDTH_ID] = toString(LOWER_HALF_BANDWIDTH_DEFAULT_VALUE);
    mProperties[RELATIVE_TOLERANCE_ID] = toString(RELATIVE_TOLERANCE_DEFAULT_VALUE);
    mProperties[ABSOLUTE_TOLERANCE_ID] = toString(ABSOLUTE_TOLERANCE_DEFAULT_VALUE);
    mProperties[INTERPOLATE_SOLUTION_ID] = toString(INTERPOLATE_SOLUTION_DEFAULT_VALUE);
}

SolverCvode::Impl::~Impl()
{
    if (mContext != nullptr) {
        N_VDestroy_Serial(mStatesVector);
        SUNLinSolFree(mLinearSolver);
        SUNNonlinSolFree(mNonLinearSolver);
        SUNMatDestroy(mMatrix);

        CVodeFree(&mSolver);

        SUNContext_Free(&mContext);

        delete mUserData;
    }
}

std::map<std::string, std::string> SolverCvode::Impl::propertiesId() const
{
    static const std::map<std::string, std::string> PROPERTIES_ID = {
        {MAXIMUM_STEP_NAME, MAXIMUM_STEP_ID},
        {MAXIMUM_NUMBER_OF_STEPS_NAME, MAXIMUM_NUMBER_OF_STEPS_ID},
        {INTEGRATION_METHOD_NAME, INTEGRATION_METHOD_ID},
        {ITERATION_TYPE_NAME, ITERATION_TYPE_ID},
        {LINEAR_SOLVER_NAME, LINEAR_SOLVER_ID},
        {PRECONDITIONER_NAME, PRECONDITIONER_ID},
        {UPPER_HALF_BANDWIDTH_NAME, UPPER_HALF_BANDWIDTH_ID},
        {LOWER_HALF_BANDWIDTH_NAME, LOWER_HALF_BANDWIDTH_ID},
        {RELATIVE_TOLERANCE_NAME, RELATIVE_TOLERANCE_ID},
        {ABSOLUTE_TOLERANCE_NAME, ABSOLUTE_TOLERANCE_ID},
        {INTERPOLATE_SOLUTION_NAME, INTERPOLATE_SOLUTION_ID},
    };

    return PROPERTIES_ID;
}

bool SolverCvode::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                                   ComputeRates pComputeRates)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;
    auto maximumStep = MAXIMUM_STEP_DEFAULT_VALUE;
    auto maximumNumberOfSteps = MAXIMUM_NUMBER_OF_STEPS_DEFAULT_VALUE;
    auto integrationMethod = INTEGRATION_METHOD_DEFAULT_VALUE;
    auto iterationType = ITERATION_TYPE_DEFAULT_VALUE;
    auto linearSolver = LINEAR_SOLVER_DEFAULT_VALUE;
    auto preconditioner = PRECONDITIONER_DEFAULT_VALUE;
    auto upperHalfBandwidth = UPPER_HALF_BANDWIDTH_DEFAULT_VALUE;
    auto lowerHalfBandwidth = LOWER_HALF_BANDWIDTH_DEFAULT_VALUE;
    auto relativeTolerance = RELATIVE_TOLERANCE_DEFAULT_VALUE;
    auto absoluteTolerance = ABSOLUTE_TOLERANCE_DEFAULT_VALUE;

    maximumStep = toDouble(mProperties[MAXIMUM_STEP_ID], ok);

    if (!ok || (maximumStep < 0.0)) {
        addError(R"(The ")" + MAXIMUM_STEP_NAME + R"(" property has an invalid value (")" + mProperties[MAXIMUM_STEP_ID] + R"("). It must be a floating point number greater or equal to zero.)");
    }

    maximumNumberOfSteps = toInt(mProperties[MAXIMUM_NUMBER_OF_STEPS_ID], ok);

    if (!ok || (maximumNumberOfSteps <= 0)) {
        addError(R"(The ")" + MAXIMUM_NUMBER_OF_STEPS_NAME + R"(" property has an invalid value (")" + mProperties[MAXIMUM_NUMBER_OF_STEPS_ID] + R"("). It must be a floating point number greater than zero.)");
    }

    integrationMethod = mProperties[INTEGRATION_METHOD_ID];

    if (std::find(INTEGRATION_METHOD_LIST.begin(), INTEGRATION_METHOD_LIST.end(), integrationMethod) == INTEGRATION_METHOD_LIST.end()) {
        addError(R"(The ")" + INTEGRATION_METHOD_NAME + R"(" property has an invalid value (")" + mProperties[INTEGRATION_METHOD_ID] + R"("). It must be equal to either ")" + ADAMS_MOULTON_METHOD + R"(" or ")" + BDF_METHOD + R"(".)");
    }

    iterationType = mProperties[ITERATION_TYPE_ID];

    if (std::find(ITERATION_TYPE_LIST.begin(), ITERATION_TYPE_LIST.end(), iterationType) == ITERATION_TYPE_LIST.end()) {
        addError(R"(The ")" + ITERATION_TYPE_NAME + R"(" property has an invalid value (")" + mProperties[ITERATION_TYPE_ID] + R"("). It must be equal to either ")" + FUNCTIONAL_ITERATION_TYPE + R"(" or ")" + NEWTON_ITERATION_TYPE + R"(".)");
    } else if (iterationType == NEWTON_ITERATION_TYPE) {
        // We are dealing with a Newton iteration type, so we need a linear solver.

        linearSolver = mProperties[LINEAR_SOLVER_ID];

        if (std::find(LINEAR_SOLVER_LIST.begin(), LINEAR_SOLVER_LIST.end(), linearSolver) == LINEAR_SOLVER_LIST.end()) {
            addError(R"(The ")" + LINEAR_SOLVER_NAME + R"(" property has an invalid value (")" + mProperties[LINEAR_SOLVER_ID] + R"("). It must be equal to either ")" + DENSE_LINEAR_SOLVER + R"(", ")" + BANDED_LINEAR_SOLVER + R"(", ")" + DIAGONAL_LINEAR_SOLVER + R"(", ")" + GMRES_LINEAR_SOLVER + R"(", ")" + BICGSTAB_LINEAR_SOLVER + R"(", or ")" + TFQMR_LINEAR_SOLVER + R"(".)");
        } else {
            bool needUpperAndLowerHalfBandwidths = false;

            if (linearSolver == BANDED_LINEAR_SOLVER) {
                // We are dealing with a banded linear solver, so we need both an upper and a lower half bandwidth.

                needUpperAndLowerHalfBandwidths = true;
            } else if ((linearSolver == GMRES_LINEAR_SOLVER)
                       || (linearSolver == BICGSTAB_LINEAR_SOLVER)
                       || (linearSolver == TFQMR_LINEAR_SOLVER)) {
                // We are dealing with a GMRES/Bi-CGStab/TFQMR linear solver, so we may need a preconditioner.

                preconditioner = mProperties[PRECONDITIONER_ID];

                if (std::find(PRECONDITIONER_LIST.begin(), PRECONDITIONER_LIST.end(), preconditioner) == PRECONDITIONER_LIST.end()) {
                    addError(R"(The ")" + PRECONDITIONER_NAME + R"(" property has an invalid value (")" + mProperties[PRECONDITIONER_ID] + R"("). It must be equal to either ")" + NO_PRECONDITIONER + R"(" or ")" + BANDED_PRECONDITIONER + R"(".)");
                } else if (preconditioner == BANDED_PRECONDITIONER) {
                    // We are dealing with a banded preconditioner, so we need both an upper and a lower half bandwidth.

                    needUpperAndLowerHalfBandwidths = true;
                }
            }

            if (needUpperAndLowerHalfBandwidths) {
                upperHalfBandwidth = toInt(mProperties[UPPER_HALF_BANDWIDTH_ID], ok);

                if (!ok || (upperHalfBandwidth < 0) || (upperHalfBandwidth >= static_cast<int>(pSize))) {
                    addError(R"(The ")" + UPPER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[UPPER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pSize) + R"(.)");
                }

                lowerHalfBandwidth = toInt(mProperties[LOWER_HALF_BANDWIDTH_ID], ok);

                if (!ok || (lowerHalfBandwidth < 0) || (lowerHalfBandwidth >= static_cast<int>(pSize))) {
                    addError(R"(The ")" + LOWER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[LOWER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pSize) + R"(.)");
                }
            }
        }
    }

    relativeTolerance = toDouble(mProperties[RELATIVE_TOLERANCE_ID], ok);

    if (!ok || (relativeTolerance < 0.0)) {
        addError(R"(The ")" + RELATIVE_TOLERANCE_NAME + R"(" property has an invalid value (")" + mProperties[RELATIVE_TOLERANCE_ID] + R"("). It must be a floating point number greater than zero.)");
    }

    absoluteTolerance = toDouble(mProperties[ABSOLUTE_TOLERANCE_ID], ok);

    if (!ok || (absoluteTolerance < 0.0)) {
        addError(R"(The ")" + ABSOLUTE_TOLERANCE_NAME + R"(" property has an invalid value (")" + mProperties[ABSOLUTE_TOLERANCE_ID] + R"("). It must be a floating point number greater than zero.)");
    }

    mInterpolateSolution = toBool(mProperties[INTERPOLATE_SOLUTION_ID], ok);

    if (!ok) {
        addError(R"(The ")" + INTERPOLATE_SOLUTION_NAME + R"(" property has an invalid value (")" + mProperties[INTERPOLATE_SOLUTION_ID] + R"("). It must be equal to either "Yes" or "No".)");
    }

    // Check whether we have had issues and, if so, then leave.

    if (!mIssues.empty()) {
        return false;
    }

    // Create our SUNDIALS context.

    SUNContext_Create(nullptr, &mContext);

    // Create our states vector.

    mStatesVector = N_VMake_Serial(static_cast<int64_t>(pSize), pStates, mContext);

    // Create our CVODES solver.

    mSolver = CVodeCreate((integrationMethod == BDF_METHOD) ? CV_BDF : CV_ADAMS, mContext);

    // Use our own error handler.

    //---GRY---
    // CVodeSetErrHandlerFn(mSolver, errorHandler, this);

    // Initialise our CVODES solver.

    CVodeInit(mSolver, rhsFunction, pVoi, mStatesVector);

    // Set our user data.

    mUserData = new SolverCvodeUserData(pVariables, pComputeRates);

    CVodeSetUserData(mSolver, mUserData);

    // Set our maximum step.

    CVodeSetMaxStep(mSolver, maximumStep);

    // Set our maximum number of steps.

    CVodeSetMaxNumSteps(mSolver, maximumNumberOfSteps);

    // Set our linear solver, if needed.

    if (iterationType == NEWTON_ITERATION_TYPE) {
        if (linearSolver == DENSE_LINEAR_SOLVER) {
            mMatrix = SUNDenseMatrix(static_cast<int64_t>(pSize), static_cast<int64_t>(pSize), mContext);
            mLinearSolver = SUNLinSol_Dense(mStatesVector, mMatrix, mContext);

            CVodeSetLinearSolver(mSolver, mLinearSolver, mMatrix);
        } else if (linearSolver == BANDED_LINEAR_SOLVER) {
            mMatrix = SUNBandMatrix(static_cast<int64_t>(pSize),
                                    static_cast<int64_t>(upperHalfBandwidth), static_cast<int64_t>(lowerHalfBandwidth),
                                    mContext);
            mLinearSolver = SUNLinSol_Band(mStatesVector, mMatrix, mContext);

            CVodeSetLinearSolver(mSolver, mLinearSolver, mMatrix);
        } else if (linearSolver == DIAGONAL_LINEAR_SOLVER) {
            CVDiag(mSolver);
        } else {
            // We are dealing with a GMRES/Bi-CGStab/TFQMR linear solver, so we may need a preconditioner.

            if (preconditioner == BANDED_PRECONDITIONER) {
                if (linearSolver == GMRES_LINEAR_SOLVER) {
                    mLinearSolver = SUNLinSol_SPGMR(mStatesVector, PREC_LEFT, 0, mContext);
                } else if (linearSolver == BICGSTAB_LINEAR_SOLVER) {
                    mLinearSolver = SUNLinSol_SPBCGS(mStatesVector, PREC_LEFT, 0, mContext);
                } else {
                    mLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, PREC_LEFT, 0, mContext);
                }

                CVodeSetLinearSolver(mSolver, mLinearSolver, mMatrix);
                CVBandPrecInit(mSolver, static_cast<int64_t>(pSize),
                               static_cast<int64_t>(upperHalfBandwidth), static_cast<int64_t>(lowerHalfBandwidth));
            } else {
                if (linearSolver == GMRES_LINEAR_SOLVER) {
                    mLinearSolver = SUNLinSol_SPGMR(mStatesVector, PREC_NONE, 0, mContext);
                } else if (linearSolver == BICGSTAB_LINEAR_SOLVER) {
                    mLinearSolver = SUNLinSol_SPBCGS(mStatesVector, PREC_NONE, 0, mContext);
                } else {
                    mLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, PREC_NONE, 0, mContext);
                }

                CVodeSetLinearSolver(mSolver, mLinearSolver, mMatrix);
            }
        }
    } else {
        mNonLinearSolver = SUNNonlinSol_FixedPoint(mStatesVector, 0, mContext);

        CVodeSetNonlinearSolver(mSolver, mNonLinearSolver);
    }

    // Set our relative and absolute tolerances.

    CVodeSStolerances(mSolver, relativeTolerance, absoluteTolerance);

    // Initialise the ODE solver itself.

    return SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverCvode::Impl::solve(double &pVoi, double pVoiEnd) const
{
    //---GRY--- TO BE DONE.

    pVoi = pVoiEnd;

    return true;
}

SolverCvode::SolverCvode()
    : SolverOde(new Impl())
{
}

SolverCvode::~SolverCvode()
{
    delete pimpl();
}

SolverCvode::Impl *SolverCvode::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

const SolverCvode::Impl *SolverCvode::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}

bool SolverCvode::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                             ComputeRates pComputeRates)
{
    return pimpl()->initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeRates);
}

bool SolverCvode::solve(double &pVoi, double pVoiEnd) const
{
    return pimpl()->solve(pVoi, pVoiEnd);
}

} // namespace libOpenCOR

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

#include "libsedmlbegin.h"
#include "sedml/SedAlgorithm.h"
#include "libsedmlend.h"

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

#include <utility>

namespace libOpenCOR {

// Some utilities.

namespace {

std::string toString(SolverCvode::IntegrationMethod pIntegrationMethod)
{
    return (pIntegrationMethod == SolverCvode::IntegrationMethod::BDF) ?
               "BDF" :
               "Adams-Moulton";
}

std::string toString(SolverCvode::IterationType pIterationType)
{
    return (pIterationType == SolverCvode::IterationType::FUNCTIONAL) ?
               "Functional" :
               "Newton";
}

std::string toString(SolverCvode::LinearSolver pLinearSolver)
{
    return (pLinearSolver == SolverCvode::LinearSolver::DENSE) ?
               "Dense" :
           (pLinearSolver == SolverCvode::LinearSolver::BANDED) ?
               "Banded" :
           (pLinearSolver == SolverCvode::LinearSolver::DIAGONAL) ?
               "Diagonal" :
           (pLinearSolver == SolverCvode::LinearSolver::GMRES) ?
               "GMRES" :
           (pLinearSolver == SolverCvode::LinearSolver::BICGSTAB) ?
               "BiCGStab" :
               "TFQMR";
}

std::string toString(SolverCvode::Preconditioner pPreconditioner)
{
    return (pPreconditioner == SolverCvode::Preconditioner::NO) ?
               "No" :
               "Banded";
}

} // namespace

// Right-hand side function.

namespace {

void errorHandler(int pLine, const char *pFunction, const char *pFile, const char *pErrorMessage, SUNErrCode pErrorCode,
                  void *pUserData, SUNContext pSunContext)
{
    (void)pLine;
    (void)pFunction;
    (void)pFile;
    (void)pSunContext;

#ifdef CODE_COVERAGE_ENABLED
    (void)pErrorCode;
#else
    if (pErrorCode != CV_WARNING) {
#endif
    *static_cast<std::string *>(pUserData) = pErrorMessage;
#ifndef CODE_COVERAGE_ENABLED
}
#endif
}

int rhsFunction(double pVoi, N_Vector pStates, N_Vector pRates, void *pUserData)
{
    auto *userData = static_cast<SolverCvodeUserData *>(pUserData);

    if (userData->computeCompiledRates != nullptr) {
        userData->computeCompiledRates(pVoi, N_VGetArrayPointer_Serial(pStates), N_VGetArrayPointer_Serial(pRates),
                                       userData->constants, userData->computedConstants, userData->algebraic);
    } else {
        userData->computeInterpretedRates(pVoi, N_VGetArrayPointer_Serial(pStates), N_VGetArrayPointer_Serial(pRates),
                                          userData->constants, userData->computedConstants, userData->algebraic);
    }

    return 0;
}

} // namespace

// Solver.

SolverCvode::Impl::Impl()
    : SolverOde::Impl("KISAO:0000019", "CVODE")
{
}

SolverCvode::Impl::~Impl()
{
    resetInternals();
}

void SolverCvode::Impl::populate(libsedml::SedAlgorithm *pAlgorithm)
{
    for (unsigned int i = 0; i < pAlgorithm->getNumAlgorithmParameters(); ++i) {
        auto *algorithmParameter = pAlgorithm->getAlgorithmParameter(i);
        auto kisaoId = algorithmParameter->getKisaoID();
        auto value = algorithmParameter->getValue();

        if (kisaoId == "KISAO:0000467") {
            mMaximumStep = toDouble(value);

            if ((mMaximumStep < 0.0) || std::isnan(mMaximumStep)) {
                addWarning(std::string("The maximum step ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. A maximum step of ").append(toString(DEFAULT_MAXIMUM_STEP)).append(" will be used instead."));

                mMaximumStep = DEFAULT_MAXIMUM_STEP;
            }
        } else if (kisaoId == "KISAO:0000415") {
            mMaximumNumberOfSteps = toInt(value);

            if (!isInt(value) || (mMaximumNumberOfSteps <= 0)) {
                addWarning(std::string("The maximum number of steps ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater than 0. A maximum number of steps of ").append(toString(DEFAULT_MAXIMUM_NUMBER_OF_STEPS)).append(" will be used instead."));

                mMaximumNumberOfSteps = DEFAULT_MAXIMUM_NUMBER_OF_STEPS;
            }
        } else if (kisaoId == "KISAO:0000475") {
            if ((value != "BDF") && (value != "Adams-Moulton")) {
                addWarning(std::string("The integration method ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'BDF' or 'Adams-Moulton'. A ").append(toString(DEFAULT_INTEGRATION_METHOD)).append(" integration method will be used instead."));

                value = toString(DEFAULT_INTEGRATION_METHOD);
            }

            mIntegrationMethod = (value == "BDF") ?
                                     SolverCvode::IntegrationMethod::BDF :
                                     SolverCvode::IntegrationMethod::ADAMS_MOULTON;
        } else if (kisaoId == "KISAO:0000476") {
            if ((value != "Functional") && (value != "Newton")) {
                addWarning(std::string("The iteration type ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'Functional' or 'Newton'. A ").append(toString(DEFAULT_ITERATION_TYPE)).append(" iteration type will be used instead."));

                value = toString(DEFAULT_ITERATION_TYPE);
            }

            mIterationType = (value == "Functional") ?
                                 SolverCvode::IterationType::FUNCTIONAL :
                                 SolverCvode::IterationType::NEWTON;
        } else if (kisaoId == "KISAO:0000477") {
            if ((value != "Dense") && (value != "Banded") && (value != "Diagonal") && (value != "GMRES") && (value != "BiCGStab") && (value != "TFQMR")) {
                addWarning(std::string("The linear solver ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'Dense', 'Banded', 'Diagonal', 'GMRES', 'BiCGStab', or 'TFQMR'. A ").append(toString(DEFAULT_LINEAR_SOLVER)).append(" linear solver will be used instead."));

                value = toString(DEFAULT_LINEAR_SOLVER);
            }

            mLinearSolver = (value == "Dense") ?
                                SolverCvode::LinearSolver::DENSE :
                            (value == "Banded") ?
                                SolverCvode::LinearSolver::BANDED :
                            (value == "Diagonal") ?
                                SolverCvode::LinearSolver::DIAGONAL :
                            (value == "GMRES") ?
                                SolverCvode::LinearSolver::GMRES :
                            (value == "BiCGStab") ?
                                SolverCvode::LinearSolver::BICGSTAB :
                                SolverCvode::LinearSolver::TFQMR;
        } else if (kisaoId == "KISAO:0000478") {
            if ((value != "No") && (value != "Banded")) {
                addWarning(std::string("The preconditioner ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'No' or 'Banded'. A ").append(toString(DEFAULT_PRECONDITIONER)).append(" preconditioner will be used instead."));

                value = toString(DEFAULT_PRECONDITIONER);
            }

            mPreconditioner = (value == "No") ?
                                  SolverCvode::Preconditioner::NO :
                                  SolverCvode::Preconditioner::BANDED;
        } else if (kisaoId == "KISAO:0000479") {
            mUpperHalfBandwidth = toInt(value);

            if (!isInt(value) || (mUpperHalfBandwidth < 0)) {
                addWarning(std::string("The upper half-bandwidth ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. An upper half-bandwidth of ").append(toString(DEFAULT_UPPER_HALF_BANDWIDTH)).append(" will be used instead."));

                mUpperHalfBandwidth = DEFAULT_UPPER_HALF_BANDWIDTH;
            }
        } else if (kisaoId == "KISAO:0000480") {
            mLowerHalfBandwidth = toInt(value);

            if (!isInt(value) || (mLowerHalfBandwidth < 0)) {
                addWarning(std::string("The lower half-bandwidth ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. A lower half-bandwidth of ").append(toString(DEFAULT_LOWER_HALF_BANDWIDTH)).append(" will be used instead."));

                mLowerHalfBandwidth = DEFAULT_LOWER_HALF_BANDWIDTH;
            }
        } else if (kisaoId == "KISAO:0000209") {
            mRelativeTolerance = toDouble(value);

            if ((mRelativeTolerance < 0.0) || std::isnan(mRelativeTolerance)) {
                addWarning(std::string("The relative tolerance ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. A relative tolerance of ").append(toString(DEFAULT_RELATIVE_TOLERANCE)).append(" will be used instead."));

                mRelativeTolerance = DEFAULT_RELATIVE_TOLERANCE;
            }
        } else if (kisaoId == "KISAO:0000211") {
            mAbsoluteTolerance = toDouble(value);

            if ((mAbsoluteTolerance < 0.0) || std::isnan(mAbsoluteTolerance)) {
                addWarning(std::string("The absolute tolerance ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater or equal to 0. An absolute tolerance of ").append(toString(DEFAULT_ABSOLUTE_TOLERANCE)).append(" will be used instead."));

                mAbsoluteTolerance = DEFAULT_ABSOLUTE_TOLERANCE;
            }
        } else if (kisaoId == "KISAO:0000481") {
            if ((value != "true") && (value != "false")) {
                addWarning(std::string("The interpolate solution parameter ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'true' or 'false'. A value of ").append(toString(DEFAULT_INTERPOLATE_SOLUTION)).append(" will be used instead."));

                value = toString(DEFAULT_INTERPOLATE_SOLUTION);
            }

            mInterpolateSolution = toBool(value);
        } else {
            addWarning(std::string("The parameter '").append(kisaoId).append("' is not recognised. It will be ignored."));
        }
    }
}

SolverPtr SolverCvode::Impl::duplicate()
{
    auto solver = SolverCvode::create();
    auto *solverPimpl = solver->pimpl();

    solverPimpl->mMaximumStep = mMaximumStep;
    solverPimpl->mMaximumNumberOfSteps = mMaximumNumberOfSteps;
    solverPimpl->mIntegrationMethod = mIntegrationMethod;
    solverPimpl->mIterationType = mIterationType;
    solverPimpl->mLinearSolver = mLinearSolver;
    solverPimpl->mPreconditioner = mPreconditioner;
    solverPimpl->mUpperHalfBandwidth = mUpperHalfBandwidth;
    solverPimpl->mLowerHalfBandwidth = mLowerHalfBandwidth;
    solverPimpl->mRelativeTolerance = mRelativeTolerance;
    solverPimpl->mAbsoluteTolerance = mAbsoluteTolerance;
    solverPimpl->mInterpolateSolution = mInterpolateSolution;

    return solver;
}

void SolverCvode::Impl::resetInternals()
{
    if (mSunContext != nullptr) {
        N_VDestroy_Serial(mStatesVector);
        SUNLinSolFree(mSunLinearSolver);
        SUNNonlinSolFree(mSunNonLinearSolver);
        SUNMatDestroy(mSunMatrix);

        CVodeFree(&mSolver);

        SUNContext_PopErrHandler(mSunContext);

        SUNContext_Free(&mSunContext);
    }
}

StringStringMap SolverCvode::Impl::properties() const
{
    StringStringMap res;

    res["KISAO:0000467"] = toString(mMaximumStep);
    res["KISAO:0000415"] = toString(mMaximumNumberOfSteps);
    res["KISAO:0000475"] = toString(mIntegrationMethod);
    res["KISAO:0000476"] = toString(mIterationType);
    res["KISAO:0000477"] = toString(mLinearSolver);
    res["KISAO:0000478"] = toString(mPreconditioner);
    res["KISAO:0000479"] = toString(mUpperHalfBandwidth);
    res["KISAO:0000480"] = toString(mLowerHalfBandwidth);
    res["KISAO:0000209"] = toString(mRelativeTolerance);
    res["KISAO:0000211"] = toString(mAbsoluteTolerance);
    res["KISAO:0000481"] = toString(mInterpolateSolution);

    return res;
}

bool SolverCvode::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                                   double *pConstants, double *pComputedConstants, double *pAlgebraic,
                                   CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                                   CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates)
{
    resetInternals();
    removeAllIssues();

    // Initialise the ODE solver itself.

    SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates,
                                pConstants, pComputedConstants, pAlgebraic,
                                pComputeCompiledRates, pComputeInterpretedRates);

    // Check the solver's properties.

    if (mMaximumStep < 0.0) {
        addError(std::string("The maximum step cannot be equal to ").append(toString(mMaximumStep)).append(". It must be greater or equal to 0."));
    }

    if (mMaximumNumberOfSteps <= 0) {
        addError(std::string("The maximum number of steps cannot be equal to ").append(toString(mMaximumNumberOfSteps)).append(". It must be greater than 0."));
    }

    if (mIterationType == IterationType::NEWTON) {
        // We are dealing with a Newton iteration type, so we need a linear solver.

        bool needUpperAndLowerHalfBandwidths = false;

        if (mLinearSolver == LinearSolver::BANDED) {
            // We are dealing with a banded linear solver, so we need both an upper and a lower half-bandwidth.

            needUpperAndLowerHalfBandwidths = true;
        } else if ((mLinearSolver == LinearSolver::GMRES)
                   || (mLinearSolver == LinearSolver::BICGSTAB)
                   || (mLinearSolver == LinearSolver::TFQMR)) {
            // We are dealing with a GMRES/Bi-CGStab/TFQMR linear solver, so we may need a preconditioner.

            if (mPreconditioner == Preconditioner::BANDED) {
                // We are dealing with a banded preconditioner, so we need both an upper and a lower half-bandwidth.

                needUpperAndLowerHalfBandwidths = true;
            }
        }

        if (needUpperAndLowerHalfBandwidths) {
            if ((mUpperHalfBandwidth < 0) || std::cmp_greater_equal(mUpperHalfBandwidth, pSize)) {
                addError(std::string("The upper half-bandwidth cannot be equal to ").append(toString(mUpperHalfBandwidth)).append(". It must be between 0 and ").append(toString(pSize - 1)).append("."));
            }

            if ((mLowerHalfBandwidth < 0) || std::cmp_greater_equal(mLowerHalfBandwidth, pSize)) {
                addError(std::string("The lower half-bandwidth cannot be equal to ").append(toString(mLowerHalfBandwidth)).append(". It must be between 0 and ").append(toString(pSize - 1)).append("."));
            }
        }
    }

    if (mRelativeTolerance < 0.0) {
        addError(std::string("The relative tolerance cannot be equal to ").append(toString(mRelativeTolerance)).append(". It must be greater or equal to 0."));
    }

    if (mAbsoluteTolerance < 0.0) {
        addError(std::string("The absolute tolerance cannot be equal to ").append(toString(mAbsoluteTolerance)).append(". It must be greater or equal to 0."));
    }

    // Check whether we got some errors.

    if (hasErrors()) {
        return false;
    }

    // Create our SUNDIALS context.

    ASSERT_EQ(SUNContext_Create(SUN_COMM_NULL, &mSunContext), 0);

    // Create our CVODE solver.

    mSolver = CVodeCreate((mIntegrationMethod == IntegrationMethod::BDF) ? CV_BDF : CV_ADAMS, mSunContext);

    ASSERT_NE(mSolver, nullptr);

    // Use our own error handler and disable the logger.

    ASSERT_EQ(SUNContext_PushErrHandler(mSunContext, errorHandler, &mErrorMessage), CV_SUCCESS);
    ASSERT_EQ(SUNContext_SetLogger(mSunContext, nullptr), CV_SUCCESS);

    // Initialise our CVODE solver.

    mStatesVector = N_VMake_Serial(static_cast<int64_t>(pSize), pStates, mSunContext);

    ASSERT_NE(mStatesVector, nullptr);
    ASSERT_EQ(CVodeInit(mSolver, rhsFunction, pVoi, mStatesVector), CV_SUCCESS);

    // Set our user data.

    mUserData.constants = pConstants;
    mUserData.computedConstants = pComputedConstants;
    mUserData.algebraic = pAlgebraic;
    mUserData.computeCompiledRates = pComputeCompiledRates;
    mUserData.computeInterpretedRates = pComputeInterpretedRates;

    ASSERT_EQ(CVodeSetUserData(mSolver, &mUserData), CV_SUCCESS);

    // Set our maximum step.

    ASSERT_EQ(CVodeSetMaxStep(mSolver, mMaximumStep), CV_SUCCESS);

    // Set our maximum number of steps.

    ASSERT_EQ(CVodeSetMaxNumSteps(mSolver, mMaximumNumberOfSteps), CV_SUCCESS);

    // Set our linear solver, if needed.

    if (mIterationType == IterationType::NEWTON) {
        if (mLinearSolver == LinearSolver::DENSE) {
            mSunMatrix = SUNDenseMatrix(static_cast<int64_t>(pSize), static_cast<int64_t>(pSize), mSunContext);

            ASSERT_NE(mSunMatrix, nullptr);

            mSunLinearSolver = SUNLinSol_Dense(mStatesVector, mSunMatrix, mSunContext);

            ASSERT_NE(mSunLinearSolver, nullptr);

            ASSERT_EQ(CVodeSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), CVLS_SUCCESS);
        } else if (mLinearSolver == LinearSolver::BANDED) {
            mSunMatrix = SUNBandMatrix(static_cast<int64_t>(pSize),
                                       static_cast<int64_t>(mUpperHalfBandwidth), static_cast<int64_t>(mLowerHalfBandwidth),
                                       mSunContext);

            ASSERT_NE(mSunMatrix, nullptr);

            mSunLinearSolver = SUNLinSol_Band(mStatesVector, mSunMatrix, mSunContext);

            ASSERT_NE(mSunLinearSolver, nullptr);

            ASSERT_EQ(CVodeSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), CVLS_SUCCESS);
        } else if (mLinearSolver == LinearSolver::DIAGONAL) {
            ASSERT_EQ(CVDiag(mSolver), CVDIAG_SUCCESS);
        } else {
            // We are dealing with a GMRES/Bi-CGStab/TFQMR linear solver, so we may need a preconditioner.

            if (mPreconditioner == Preconditioner::BANDED) {
                if (mLinearSolver == LinearSolver::GMRES) {
                    mSunLinearSolver = SUNLinSol_SPGMR(mStatesVector, SUN_PREC_LEFT, 0, mSunContext);
                } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                    mSunLinearSolver = SUNLinSol_SPBCGS(mStatesVector, SUN_PREC_LEFT, 0, mSunContext);
                } else {
                    mSunLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, SUN_PREC_LEFT, 0, mSunContext);
                }

                ASSERT_NE(mSunLinearSolver, nullptr);

                ASSERT_EQ(CVodeSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), CVLS_SUCCESS);
                ASSERT_EQ(CVBandPrecInit(mSolver, static_cast<int64_t>(pSize),
                                         static_cast<int64_t>(mUpperHalfBandwidth),
                                         static_cast<int64_t>(mLowerHalfBandwidth)),
                          CVLS_SUCCESS);
            } else {
                if (mLinearSolver == LinearSolver::GMRES) {
                    mSunLinearSolver = SUNLinSol_SPGMR(mStatesVector, SUN_PREC_NONE, 0, mSunContext);
                } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                    mSunLinearSolver = SUNLinSol_SPBCGS(mStatesVector, SUN_PREC_NONE, 0, mSunContext);
                } else {
                    mSunLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, SUN_PREC_NONE, 0, mSunContext);
                }

                ASSERT_NE(mSunLinearSolver, nullptr);

                ASSERT_EQ(CVodeSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), CVLS_SUCCESS);
            }
        }
    } else {
        mSunNonLinearSolver = SUNNonlinSol_FixedPoint(mStatesVector, 0, mSunContext);

        ASSERT_NE(mSunNonLinearSolver, nullptr);

        CVodeSetNonlinearSolver(mSolver, mSunNonLinearSolver);
    }

    // Set our relative and absolute tolerances.

    ASSERT_EQ(CVodeSStolerances(mSolver, mRelativeTolerance, mAbsoluteTolerance), CV_SUCCESS);

    return true;
}

/*---GRY--- TO BE UNCOMMENTED ONCE WE ACTUALLY NEED IT.
bool SolverCvode::Impl::reinitialise(double pVoi)
{
    // Reinitialise the ODE solver itself.

    SolverOde::Impl::reinitialise(pVoi);

    // Reinitialise our CVODE solver.

    ASSERT_EQ(CVodeReInit(mSolver, pVoi, mStatesVector), CV_SUCCESS);

    return true;
}
*/

double SolverCvode::Impl::maximumStep() const
{
    return mMaximumStep;
}

void SolverCvode::Impl::setMaximumStep(double pMaximumStep)
{
    mMaximumStep = pMaximumStep;
}

int SolverCvode::Impl::maximumNumberOfSteps() const
{
    return mMaximumNumberOfSteps;
}

void SolverCvode::Impl::setMaximumNumberOfSteps(int pMaximumNumberOfSteps)
{
    mMaximumNumberOfSteps = pMaximumNumberOfSteps;
}

SolverCvode::IntegrationMethod SolverCvode::Impl::integrationMethod() const
{
    return mIntegrationMethod;
}

void SolverCvode::Impl::setIntegrationMethod(SolverCvode::IntegrationMethod pIntegrationMethod)
{
    mIntegrationMethod = pIntegrationMethod;
}

SolverCvode::IterationType SolverCvode::Impl::iterationType() const
{
    return mIterationType;
}

void SolverCvode::Impl::setIterationType(SolverCvode::IterationType pIterationType)
{
    mIterationType = pIterationType;
}

SolverCvode::LinearSolver SolverCvode::Impl::linearSolver() const
{
    return mLinearSolver;
}

void SolverCvode::Impl::setLinearSolver(SolverCvode::LinearSolver pLinearSolver)
{
    mLinearSolver = pLinearSolver;
}

SolverCvode::Preconditioner SolverCvode::Impl::preconditioner() const
{
    return mPreconditioner;
}

void SolverCvode::Impl::setPreconditioner(SolverCvode::Preconditioner pPreconditioner)
{
    mPreconditioner = pPreconditioner;
}

int SolverCvode::Impl::upperHalfBandwidth() const
{
    return mUpperHalfBandwidth;
}

void SolverCvode::Impl::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    mUpperHalfBandwidth = pUpperHalfBandwidth;
}

int SolverCvode::Impl::lowerHalfBandwidth() const
{
    return mLowerHalfBandwidth;
}

void SolverCvode::Impl::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    mLowerHalfBandwidth = pLowerHalfBandwidth;
}

double SolverCvode::Impl::relativeTolerance() const
{
    return mRelativeTolerance;
}

void SolverCvode::Impl::setRelativeTolerance(double pRelativeTolerance)
{
    mRelativeTolerance = pRelativeTolerance;
}

double SolverCvode::Impl::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void SolverCvode::Impl::setAbsoluteTolerance(double pAbsoluteTolerance)
{
    mAbsoluteTolerance = pAbsoluteTolerance;
}

bool SolverCvode::Impl::interpolateSolution() const
{
    return mInterpolateSolution;
}

void SolverCvode::Impl::setInterpolateSolution(bool pInterpolateSolution)
{
    mInterpolateSolution = pInterpolateSolution;
}

bool SolverCvode::Impl::solve(double &pVoi, double pVoiEnd)
{
    // Note: rate values are computed and handled internally by CVODE, so we can't access them and therefore need to
    //       compute them ourselves. To do so, we keep track of the old state values (in mRates, to save memory) and
    //       then update mRates once we have the new state values.

    auto *oldStates = mRates;
    auto oneOverdVoi = 1.0 / (pVoiEnd - pVoi);

    std::copy(mStates, mStates + mSize, oldStates); // NOLINT

    // Solve the model using interpolation, if needed.

    if (!mInterpolateSolution) {
        ASSERT_EQ(CVodeSetStopTime(mSolver, pVoiEnd), CV_SUCCESS);
    }

    auto res = CVode(mSolver, pVoiEnd, mStatesVector, &pVoi, CV_NORMAL);

    // Compute the rate values.

    for (size_t i = 0; i < mSize; ++i) {
        mRates[i] = oneOverdVoi * (mStates[i] - oldStates[i]); // NOLINT
    }

    // Make sure that everything went fine.

    if (res < CV_SUCCESS) {
#ifndef CODE_COVERAGE_ENABLED
        if (mErrorMessage.back() != '.') {
            mErrorMessage += '.';
        }
#endif

        addError(mErrorMessage);

        return false;
    }

    return true;
}

SolverCvode::SolverCvode()
    : SolverOde(new Impl {})
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

SolverCvodePtr SolverCvode::create()
{
    return SolverCvodePtr {new SolverCvode {}};
}

double SolverCvode::maximumStep() const
{
    return pimpl()->maximumStep();
}

void SolverCvode::setMaximumStep(double pMaximumStep)
{
    pimpl()->setMaximumStep(pMaximumStep);
}

int SolverCvode::maximumNumberOfSteps() const
{
    return pimpl()->maximumNumberOfSteps();
}

void SolverCvode::setMaximumNumberOfSteps(int pMaximumNumberOfSteps)
{
    pimpl()->setMaximumNumberOfSteps(pMaximumNumberOfSteps);
}

SolverCvode::IntegrationMethod SolverCvode::integrationMethod() const
{
    return pimpl()->integrationMethod();
}

void SolverCvode::setIntegrationMethod(SolverCvode::IntegrationMethod pIntegrationMethod)
{
    pimpl()->setIntegrationMethod(pIntegrationMethod);
}

SolverCvode::IterationType SolverCvode::iterationType() const
{
    return pimpl()->iterationType();
}

void SolverCvode::setIterationType(SolverCvode::IterationType pIterationType)
{
    pimpl()->setIterationType(pIterationType);
}

SolverCvode::LinearSolver SolverCvode::linearSolver() const
{
    return pimpl()->linearSolver();
}

void SolverCvode::setLinearSolver(SolverCvode::LinearSolver pLinearSolver)
{
    pimpl()->setLinearSolver(pLinearSolver);
}

SolverCvode::Preconditioner SolverCvode::preconditioner() const
{
    return pimpl()->preconditioner();
}

void SolverCvode::setPreconditioner(SolverCvode::Preconditioner pPreconditioner)
{
    pimpl()->setPreconditioner(pPreconditioner);
}

int SolverCvode::upperHalfBandwidth() const
{
    return pimpl()->upperHalfBandwidth();
}

void SolverCvode::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    pimpl()->setUpperHalfBandwidth(pUpperHalfBandwidth);
}

int SolverCvode::lowerHalfBandwidth() const
{
    return pimpl()->lowerHalfBandwidth();
}

void SolverCvode::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    pimpl()->setLowerHalfBandwidth(pLowerHalfBandwidth);
}

double SolverCvode::relativeTolerance() const
{
    return pimpl()->relativeTolerance();
}

void SolverCvode::setRelativeTolerance(double pRelativeTolerance)
{
    pimpl()->setRelativeTolerance(pRelativeTolerance);
}

double SolverCvode::absoluteTolerance() const
{
    return pimpl()->absoluteTolerance();
}

void SolverCvode::setAbsoluteTolerance(double pAbsoluteTolerance)
{
    pimpl()->setAbsoluteTolerance(pAbsoluteTolerance);
}

bool SolverCvode::interpolateSolution() const
{
    return pimpl()->interpolateSolution();
}

void SolverCvode::setInterpolateSolution(bool pInterpolateSolution)
{
    pimpl()->setInterpolateSolution(pInterpolateSolution);
}

} // namespace libOpenCOR

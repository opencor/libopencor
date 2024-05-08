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

// Right-hand side function.

namespace {

void errorHandler(int pErrorCode, const char *pModule, const char *pFunction, char *pErrorMessage, void *pUserData)
{
    (void)pModule;
    (void)pFunction;

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
        userData->computeCompiledRates(pVoi, N_VGetArrayPointer_Serial(pStates), N_VGetArrayPointer_Serial(pRates), userData->variables);
    } else {
        userData->computeInterpretedRates(pVoi, N_VGetArrayPointer_Serial(pStates), N_VGetArrayPointer_Serial(pRates), userData->variables);
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

        SUNContext_Free(&mSunContext);
    }
}

StringStringMap SolverCvode::Impl::properties() const
{
    StringStringMap res;

    res["KISAO:0000467"] = toString(mMaximumStep);
    res["KISAO:0000415"] = toString(mMaximumNumberOfSteps);
    res["KISAO:0000475"] = (mIntegrationMethod == IntegrationMethod::BDF) ? "BDF" : "Adams-Moulton";
    res["KISAO:0000476"] = (mIterationType == IterationType::FUNCTIONAL) ? "Functional" : "Newton";
    res["KISAO:0000477"] = (mLinearSolver == LinearSolver::DENSE)    ? "Dense" :
                           (mLinearSolver == LinearSolver::BANDED)   ? "Banded" :
                           (mLinearSolver == LinearSolver::DIAGONAL) ? "Diagonal" :
                           (mLinearSolver == LinearSolver::GMRES)    ? "GMRES" :
                           (mLinearSolver == LinearSolver::BICGSTAB) ? "BiCGStab" :
                                                                       "TFQMR";
    res["KISAO:0000478"] = (mPreconditioner == Preconditioner::NO) ? "No" : "Banded";
    res["KISAO:0000479"] = toString(mUpperHalfBandwidth);
    res["KISAO:0000480"] = toString(mLowerHalfBandwidth);
    res["KISAO:0000209"] = toString(mRelativeTolerance);
    res["KISAO:0000211"] = toString(mAbsoluteTolerance);
    res["KISAO:0000481"] = mInterpolateSolution ? "true" : "false";

    return res;
}

bool SolverCvode::Impl::initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                                   CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                                   CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates)
{
    resetInternals();
    removeAllIssues();

    // Initialise the ODE solver itself.

    SolverOde::Impl::initialise(pVoi, pSize, pStates, pRates, pVariables, pComputeCompiledRates, pComputeInterpretedRates);

    // Check the solver's properties.

    if (mMaximumStep < 0.0) {
        addError("The maximum step cannot be equal to " + toString(mMaximumStep) + ". It must be greater or equal to 0.");
    }

    if (mMaximumNumberOfSteps <= 0) {
        addError("The maximum number of steps cannot be equal to " + toString(mMaximumNumberOfSteps) + ". It must be greater than 0.");
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
            if ((mUpperHalfBandwidth < 0) || (mUpperHalfBandwidth >= static_cast<int>(pSize))) {
                addError("The upper half-bandwidth cannot be equal to " + toString(mUpperHalfBandwidth) + ". It must be between 0 and " + toString(pSize - 1) + ".");
            }

            if ((mLowerHalfBandwidth < 0) || (mLowerHalfBandwidth >= static_cast<int>(pSize))) {
                addError("The lower half-bandwidth cannot be equal to " + toString(mLowerHalfBandwidth) + ". It must be between 0 and " + toString(pSize - 1) + ".");
            }
        }
    }

    if (mRelativeTolerance < 0.0) {
        addError("The relative tolerance cannot be equal to " + toString(mRelativeTolerance) + ". It must be greater or equal to 0.");
    }

    if (mAbsoluteTolerance < 0.0) {
        addError("The absolute tolerance cannot be equal to " + toString(mAbsoluteTolerance) + ". It must be greater or equal to 0.");
    }

    // Check whether we got some errors.

    if (hasErrors()) {
        return false;
    }

    // Create our SUNDIALS context.

    ASSERT_EQ(SUNContext_Create(nullptr, &mSunContext), 0);

    // Create our CVODE solver.

    mSolver = CVodeCreate((mIntegrationMethod == IntegrationMethod::BDF) ? CV_BDF : CV_ADAMS, mSunContext);

    ASSERT_NE(mSolver, nullptr);

    // Use our own error handler.

    ASSERT_EQ(CVodeSetErrHandlerFn(mSolver, errorHandler, &mErrorMessage), CV_SUCCESS);

    // Initialise our CVODE solver.

    mStatesVector = N_VMake_Serial(static_cast<int64_t>(pSize), pStates, mSunContext);

    ASSERT_NE(mStatesVector, nullptr);
    ASSERT_EQ(CVodeInit(mSolver, rhsFunction, pVoi, mStatesVector), CV_SUCCESS);

    // Set our user data.

    mUserData.variables = pVariables;
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
                    mSunLinearSolver = SUNLinSol_SPGMR(mStatesVector, PREC_LEFT, 0, mSunContext);
                } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                    mSunLinearSolver = SUNLinSol_SPBCGS(mStatesVector, PREC_LEFT, 0, mSunContext);
                } else {
                    mSunLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, PREC_LEFT, 0, mSunContext);
                }

                ASSERT_NE(mSunLinearSolver, nullptr);

                ASSERT_EQ(CVodeSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), CVLS_SUCCESS);
                ASSERT_EQ(CVBandPrecInit(mSolver, static_cast<int64_t>(pSize),
                                         static_cast<int64_t>(mUpperHalfBandwidth),
                                         static_cast<int64_t>(mLowerHalfBandwidth)),
                          CVLS_SUCCESS);
            } else {
                if (mLinearSolver == LinearSolver::GMRES) {
                    mSunLinearSolver = SUNLinSol_SPGMR(mStatesVector, PREC_NONE, 0, mSunContext);
                } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                    mSunLinearSolver = SUNLinSol_SPBCGS(mStatesVector, PREC_NONE, 0, mSunContext);
                } else {
                    mSunLinearSolver = SUNLinSol_SPTFQMR(mStatesVector, PREC_NONE, 0, mSunContext);
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

bool SolverCvode::Impl::solve(double &pVoi, double pVoiEnd)
{
    // Note: rate values are computed and handled internally by CVODE, so we can't access them and therefore need to
    //       compute them ourselves. To do so, we keep track of the old state values (in mRates, to save memory) and
    //       then update mRates once we have the new state values.

    auto oldStates = mRates;
    auto oneOverdVoi = 1.0 / (pVoiEnd - pVoi);

    std::copy(mStates, mStates + mSize, oldStates);

    // Solve the model using interpolation, if needed.

    if (!mInterpolateSolution) {
        ASSERT_EQ(CVodeSetStopTime(mSolver, pVoiEnd), CV_SUCCESS);
    }

    auto res = CVode(mSolver, pVoiEnd, mStatesVector, &pVoi, CV_NORMAL);

    // Compute the rate values.

    for (size_t i = 0; i < mSize; ++i) {
        mRates[i] = oneOverdVoi * (mStates[i] - oldStates[i]);
    }

    // Make sure that everything went fine.

    if (res < CV_SUCCESS) {
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
    return pimpl()->mMaximumStep;
}

void SolverCvode::setMaximumStep(double pMaximumStep)
{
    pimpl()->mMaximumStep = pMaximumStep;
}

int SolverCvode::maximumNumberOfSteps() const
{
    return pimpl()->mMaximumNumberOfSteps;
}

void SolverCvode::setMaximumNumberOfSteps(int pMaximumNumberOfSteps)
{
    pimpl()->mMaximumNumberOfSteps = pMaximumNumberOfSteps;
}

SolverCvode::IntegrationMethod SolverCvode::integrationMethod() const
{
    return pimpl()->mIntegrationMethod;
}

void SolverCvode::setIntegrationMethod(SolverCvode::IntegrationMethod pIntegrationMethod)
{
    pimpl()->mIntegrationMethod = pIntegrationMethod;
}

SolverCvode::IterationType SolverCvode::iterationType() const
{
    return pimpl()->mIterationType;
}

void SolverCvode::setIterationType(SolverCvode::IterationType pIterationType)
{
    pimpl()->mIterationType = pIterationType;
}

SolverCvode::LinearSolver SolverCvode::linearSolver() const
{
    return pimpl()->mLinearSolver;
}

void SolverCvode::setLinearSolver(SolverCvode::LinearSolver pLinearSolver)
{
    pimpl()->mLinearSolver = pLinearSolver;
}

SolverCvode::Preconditioner SolverCvode::preconditioner() const
{
    return pimpl()->mPreconditioner;
}

void SolverCvode::setPreconditioner(SolverCvode::Preconditioner pPreconditioner)
{
    pimpl()->mPreconditioner = pPreconditioner;
}

int SolverCvode::upperHalfBandwidth() const
{
    return pimpl()->mUpperHalfBandwidth;
}

void SolverCvode::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    pimpl()->mUpperHalfBandwidth = pUpperHalfBandwidth;
}

int SolverCvode::lowerHalfBandwidth() const
{
    return pimpl()->mLowerHalfBandwidth;
}

void SolverCvode::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    pimpl()->mLowerHalfBandwidth = pLowerHalfBandwidth;
}

double SolverCvode::relativeTolerance() const
{
    return pimpl()->mRelativeTolerance;
}

void SolverCvode::setRelativeTolerance(double pRelativeTolerance)
{
    pimpl()->mRelativeTolerance = pRelativeTolerance;
}

double SolverCvode::absoluteTolerance() const
{
    return pimpl()->mAbsoluteTolerance;
}

void SolverCvode::setAbsoluteTolerance(double pAbsoluteTolerance)
{
    pimpl()->mAbsoluteTolerance = pAbsoluteTolerance;
}

bool SolverCvode::interpolateSolution() const
{
    return pimpl()->mInterpolateSolution;
}

void SolverCvode::setInterpolateSolution(bool pInterpolateSolution)
{
    pimpl()->mInterpolateSolution = pInterpolateSolution;
}

} // namespace libOpenCOR

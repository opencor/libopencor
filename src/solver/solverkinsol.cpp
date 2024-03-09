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

#include "solverkinsol_p.h"

#include <algorithm>

#include "sundialsbegin.h"
#include "kinsol/kinsol.h"
#include "nvector/nvector_serial.h"
#include "sunlinsol/sunlinsol_band.h"
#include "sunlinsol/sunlinsol_dense.h"
#include "sunlinsol/sunlinsol_spbcgs.h"
#include "sunlinsol/sunlinsol_spgmr.h"
#include "sunlinsol/sunlinsol_sptfqmr.h"
#include "sundialsend.h"

namespace libOpenCOR {

// Compute system.

namespace {

void errorHandler(int pErrorCode, const char *pModule, const char *pFunction, char *pErrorMessage, void *pUserData)
{
    (void)pModule;
    (void)pFunction;

#ifdef CODE_COVERAGE_ENABLED
    (void)pErrorCode;
#else
    if (pErrorCode != KIN_WARNING) {
#endif
    *static_cast<std::string *>(pUserData) = pErrorMessage;
#ifndef CODE_COVERAGE_ENABLED
}
#endif
}

int computeSystem(N_Vector pU, N_Vector pF, void *pUserData)
{
    auto *userData = static_cast<SolverKinsolUserData *>(pUserData);

    userData->computeSystem(N_VGetArrayPointer_Serial(pU), N_VGetArrayPointer_Serial(pF), userData->userData);

    return 0;
}

} // namespace

// Solver.

SolverKinsol::Impl::Impl()
    : SolverNla::Impl("KISAO:0000282", "KINSOL")
{
}

SolverKinsol::Impl::~Impl()
{
    for (auto &data : mData) {
        N_VDestroy_Serial(data.second.uVector);
        N_VDestroy_Serial(data.second.onesVector);
        SUNMatDestroy(data.second.sunMatrix);
        SUNLinSolFree(data.second.sunLinearSolver);

        KINFree(&data.second.solver);

        SUNContext_Free(&data.second.context);

        delete data.second.userData;
    }
}

StringStringMap SolverKinsol::Impl::properties() const
{
    StringStringMap res;

    res["KISAO:0000486"] = toString(mMaximumNumberOfIterations);
    res["KISAO:0000477"] = (mLinearSolver == LinearSolver::DENSE)    ? "Dense" :
                           (mLinearSolver == LinearSolver::BANDED)   ? "Banded" :
                           (mLinearSolver == LinearSolver::GMRES)    ? "GMRES" :
                           (mLinearSolver == LinearSolver::BICGSTAB) ? "BiCGStab" :
                                                                       "TFQMR";
    res["KISAO:0000479"] = toString(mUpperHalfBandwidth);
    res["KISAO:0000480"] = toString(mLowerHalfBandwidth);

    return res;
}

bool SolverKinsol::Impl::solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    removeAllIssues();

    // Check whether we need to initialise our data or whether we can just reuse it.

    SolverKinsolData data;

    if (auto iter = mData.find(pComputeSystem); iter == mData.end()) {
        // We don't have any data associated with the given pComputeSystem, so get some by first making sure that the
        // solver's properties are all valid.

        if (mMaximumNumberOfIterations <= 0) {
            addError("The maximum number of iterations cannot be equal to " + toString(mMaximumNumberOfIterations) + ". It must be greater than 0.");
        }

        bool needUpperAndLowerHalfBandwidths = false;

        if (mLinearSolver == LinearSolver::BANDED) {
            // We are dealing with a banded linear solver, so we need both an upper and a lower half-bandwidth.

            needUpperAndLowerHalfBandwidths = true;
        }

        if (needUpperAndLowerHalfBandwidths) {
            if ((mUpperHalfBandwidth < 0) || (mUpperHalfBandwidth >= static_cast<int>(pN))) {
                addError("The upper half-bandwidth cannot be equal to " + toString(mUpperHalfBandwidth) + ". It must be between 0 and " + toString(pN - 1) + ".");
            }

            if ((mLowerHalfBandwidth < 0) || (mLowerHalfBandwidth >= static_cast<int>(pN))) {
                addError("The lower half-bandwidth cannot be equal to " + toString(mLowerHalfBandwidth) + ". It must be between 0 and " + toString(pN - 1) + ".");
            }
        }

        // Check whether we had errors and, if so, then leave.

        if (!mErrors.empty()) {
            return false;
        }

        // Create our SUNDIALS context.

        ASSERT_EQ(SUNContext_Create(nullptr, &data.context), 0);

        // Create our KINSOL solver.

        data.solver = KINCreate(data.context);

        ASSERT_NE(data.solver, nullptr);

        // Use our own error handler.

        ASSERT_EQ(KINSetErrHandlerFn(data.solver, errorHandler, &mErrorMessage), KIN_SUCCESS);

        // Initialise our KINSOL solver.

        data.uVector = N_VMake_Serial(static_cast<int64_t>(pN), pU, data.context);
        data.onesVector = N_VNew_Serial(static_cast<int64_t>(pN), data.context);

        ASSERT_NE(data.uVector, nullptr);
        ASSERT_NE(data.onesVector, nullptr);

        N_VConst(1.0, data.onesVector);

        ASSERT_EQ(KINInit(data.solver, computeSystem, data.uVector), KIN_SUCCESS);

        // Set our linear solver.

        if (mLinearSolver == LinearSolver::DENSE) {
            data.sunMatrix = SUNDenseMatrix(static_cast<int64_t>(pN), static_cast<int64_t>(pN), data.context);

            ASSERT_NE(data.sunMatrix, nullptr);

            data.sunLinearSolver = SUNLinSol_Dense(data.uVector, data.sunMatrix, data.context);
        } else if (mLinearSolver == LinearSolver::BANDED) {
            data.sunMatrix = SUNBandMatrix(static_cast<int64_t>(pN),
                                           static_cast<int64_t>(mUpperHalfBandwidth), static_cast<int64_t>(mLowerHalfBandwidth),
                                           data.context);

            ASSERT_NE(data.sunMatrix, nullptr);

            data.sunLinearSolver = SUNLinSol_Band(data.uVector, data.sunMatrix, data.context);
        } else {
            data.sunMatrix = nullptr;

            if (mLinearSolver == LinearSolver::GMRES) {
                data.sunLinearSolver = SUNLinSol_SPGMR(data.uVector, PREC_NONE, 0, data.context);
            } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                data.sunLinearSolver = SUNLinSol_SPBCGS(data.uVector, PREC_NONE, 0, data.context);
            } else {
                data.sunLinearSolver = SUNLinSol_SPTFQMR(data.uVector, PREC_NONE, 0, data.context);
            }
        }

        ASSERT_NE(data.sunLinearSolver, nullptr);

        ASSERT_EQ(KINSetLinearSolver(data.solver, data.sunLinearSolver, data.sunMatrix), KINLS_SUCCESS);

        // Set our user data.

        data.userData = new SolverKinsolUserData {pComputeSystem, pUserData};

        ASSERT_EQ(KINSetUserData(data.solver, data.userData), KIN_SUCCESS);

        // Set our maximum number of iterations.

        ASSERT_EQ(KINSetNumMaxIters(data.solver, mMaximumNumberOfIterations), KIN_SUCCESS);

        // Keep track of our data.

        mData[pComputeSystem] = data;
    } else {
        // We are already initialised, so just reuse our previous data, although we need to:
        //  - provide a new ones vector since the previous one will have been released; and
        //  - update the user data since it can (somehow) be located at a different address from one call to another.

        data = iter->second;

        data.onesVector = N_VNew_Serial(static_cast<int64_t>(pN), data.context);
        data.userData->userData = pUserData;
    }

    // Solve the model.

    auto res = KINSol(data.solver, data.uVector, KIN_LINESEARCH, data.onesVector, data.onesVector);

    if (res < KIN_SUCCESS) {
        addError(mErrorMessage);

        return false;
    }

    return true;
}

SolverKinsol::SolverKinsol()
    : SolverNla(new Impl {})
{
}

SolverKinsol::~SolverKinsol()
{
    delete pimpl();
}

SolverKinsol::Impl *SolverKinsol::pimpl()
{
    return static_cast<Impl *>(SolverNla::pimpl());
}

const SolverKinsol::Impl *SolverKinsol::pimpl() const
{
    return static_cast<const Impl *>(SolverNla::pimpl());
}

SolverKinsolPtr SolverKinsol::create()
{
    return SolverKinsolPtr {new SolverKinsol {}};
}

int SolverKinsol::maximumNumberOfIterations() const
{
    return pimpl()->mMaximumNumberOfIterations;
}

void SolverKinsol::setMaximumNumberOfIterations(int pMaximumNumberOfIterations)
{
    pimpl()->mMaximumNumberOfIterations = pMaximumNumberOfIterations;
}

SolverKinsol::LinearSolver SolverKinsol::linearSolver() const
{
    return pimpl()->mLinearSolver;
}

void SolverKinsol::setLinearSolver(LinearSolver pLinearSolver)
{
    pimpl()->mLinearSolver = pLinearSolver;
}

int SolverKinsol::upperHalfBandwidth() const
{
    return pimpl()->mUpperHalfBandwidth;
}

void SolverKinsol::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    pimpl()->mUpperHalfBandwidth = pUpperHalfBandwidth;
}

int SolverKinsol::lowerHalfBandwidth() const
{
    return pimpl()->mLowerHalfBandwidth;
}

void SolverKinsol::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    pimpl()->mLowerHalfBandwidth = pLowerHalfBandwidth;
}

bool SolverKinsol::solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->solve(pComputeSystem, pU, pN, pUserData);
}

} // namespace libOpenCOR

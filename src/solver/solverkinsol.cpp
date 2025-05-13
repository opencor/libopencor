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

#include "libsedmlbegin.h"
#include "sedml/SedAlgorithm.h"
#include "libsedmlend.h"

#include "sundialsbegin.h"
#include "kinsol/kinsol.h"
#include "nvector/nvector_serial.h"
#include "sunlinsol/sunlinsol_band.h"
#include "sunlinsol/sunlinsol_dense.h"
#include "sunlinsol/sunlinsol_spbcgs.h"
#include "sunlinsol/sunlinsol_spgmr.h"
#include "sunlinsol/sunlinsol_sptfqmr.h"
#include "sundialsend.h"

#include <utility>

namespace libOpenCOR {

// Compute system.

namespace {

#ifndef CODE_COVERAGE_ENABLED
void errorHandler(int pLine, const char *pFunction, const char *pFile, const char *pErrorMessage, SUNErrCode pErrorCode,
                  void *pUserData, SUNContext pSunContext)
{
    (void)pLine;
    (void)pFunction;
    (void)pFile;
    (void)pSunContext;

    if (pErrorCode != KIN_WARNING) {
        *static_cast<std::string *>(pUserData) = pErrorMessage;
    }
}
#endif

struct SolverKinsolUserData
{
    SolverNla::ComputeSystem computeSystem = nullptr;

    void *userData = nullptr;
};

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

void SolverKinsol::Impl::populate(libsedml::SedAlgorithm *pAlgorithm)
{
    for (unsigned int i = 0; i < pAlgorithm->getNumAlgorithmParameters(); ++i) {
        auto *algorithmParameter = pAlgorithm->getAlgorithmParameter(i);
        auto kisaoId = algorithmParameter->getKisaoID();
        auto value = algorithmParameter->getValue();

        if (kisaoId == "KISAO:0000486") {
            mMaximumNumberOfIterations = toInt(value);

            if (!isInt(value) || (mMaximumNumberOfIterations <= 0)) {
                addWarning(std::string("The maximum number of iterations ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be greater than 0. A maximum number of iterations of ").append(toString(DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS)).append(" will be used instead."));

                mMaximumNumberOfIterations = DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS;
            }
        } else if (kisaoId == "KISAO:0000477") {
            if ((value != "Dense") && (value != "Banded") && (value != "GMRES") && (value != "BiCGStab") && (value != "TFQMR")) {
                addWarning(std::string("The linear solver ('").append(kisaoId).append("') cannot be equal to '").append(value).append("'. It must be equal to 'Dense', 'Banded', 'GMRES', 'BiCGStab', or 'TFQMR'. A ").append(toString(DEFAULT_LINEAR_SOLVER)).append(" linear solver will be used instead."));

                value = toString(DEFAULT_LINEAR_SOLVER);
            }

            mLinearSolver = toKinsolLinearSolver(value);
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
        } else {
            addWarning(std::string("The parameter '").append(kisaoId).append("' is not recognised. It will be ignored."));
        }
    }
}

SolverPtr SolverKinsol::Impl::duplicate()
{
    auto solver = SolverKinsol::create();
    auto *solverPimpl = solver->pimpl();

    solverPimpl->mMaximumNumberOfIterations = mMaximumNumberOfIterations;
    solverPimpl->mLinearSolver = mLinearSolver;
    solverPimpl->mUpperHalfBandwidth = mUpperHalfBandwidth;
    solverPimpl->mLowerHalfBandwidth = mLowerHalfBandwidth;

    return solver;
}

StringStringMap SolverKinsol::Impl::properties() const
{
    StringStringMap res;

    res["KISAO:0000486"] = toString(mMaximumNumberOfIterations);
    res["KISAO:0000477"] = toString(mLinearSolver);
    res["KISAO:0000479"] = toString(mUpperHalfBandwidth);
    res["KISAO:0000480"] = toString(mLowerHalfBandwidth);

    return res;
}

int SolverKinsol::Impl::maximumNumberOfIterations() const
{
    return mMaximumNumberOfIterations;
}

void SolverKinsol::Impl::setMaximumNumberOfIterations(int pMaximumNumberOfIterations)
{
    mMaximumNumberOfIterations = pMaximumNumberOfIterations;
}

SolverKinsol::LinearSolver SolverKinsol::Impl::linearSolver() const
{
    return mLinearSolver;
}

void SolverKinsol::Impl::setLinearSolver(LinearSolver pLinearSolver)
{
    mLinearSolver = pLinearSolver;
}

int SolverKinsol::Impl::upperHalfBandwidth() const
{
    return mUpperHalfBandwidth;
}

void SolverKinsol::Impl::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    mUpperHalfBandwidth = pUpperHalfBandwidth;
}

int SolverKinsol::Impl::lowerHalfBandwidth() const
{
    return mLowerHalfBandwidth;
}

void SolverKinsol::Impl::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    mLowerHalfBandwidth = pLowerHalfBandwidth;
}

bool SolverKinsol::Impl::solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    removeAllIssues();

    // We don't have any data associated with the given pComputeSystem, so get some by first making sure that the
    // solver's properties are all valid.

    if (mMaximumNumberOfIterations <= 0) {
        addError(std::string("The maximum number of iterations cannot be equal to ").append(toString(mMaximumNumberOfIterations)).append(". It must be greater than 0."));
    }

    bool needUpperAndLowerHalfBandwidths = false;

    if (mLinearSolver == LinearSolver::BANDED) {
        // We are dealing with a banded linear solver, so we need both an upper and a lower half-bandwidth.

        needUpperAndLowerHalfBandwidths = true;
    }

    if (needUpperAndLowerHalfBandwidths) {
        if ((mUpperHalfBandwidth < 0) || std::cmp_greater_equal(mUpperHalfBandwidth, pN)) {
            addError(std::string("The upper half-bandwidth cannot be equal to ").append(toString(mUpperHalfBandwidth)).append(". It must be between 0 and ").append(toString(pN - 1)).append("."));
        }

        if ((mLowerHalfBandwidth < 0) || std::cmp_greater_equal(mLowerHalfBandwidth, pN)) {
            addError(std::string("The lower half-bandwidth cannot be equal to ").append(toString(mLowerHalfBandwidth)).append(". It must be between 0 and ").append(toString(pN - 1)).append("."));
        }
    }

    // Check whether we got some errors.

    if (hasErrors()) {
        return false;
    }

    // Create our SUNDIALS context.

    SUNContext context = nullptr;

    ASSERT_EQ(SUNContext_Create(SUN_COMM_NULL, &context), 0);

    // Create our KINSOL solver.

    auto *solver = KINCreate(context);

    ASSERT_NE(solver, nullptr);

    // Use our own error handler and disable the logger.

#ifndef CODE_COVERAGE_ENABLED
    ASSERT_EQ(SUNContext_PushErrHandler(context, errorHandler, &mErrorMessage), KIN_SUCCESS);
    ASSERT_EQ(SUNContext_SetLogger(context, nullptr), KIN_SUCCESS);
#endif

    // Initialise our KINSOL solver.

    auto *u = N_VMake_Serial(static_cast<int64_t>(pN), pU, context);
    auto *ones = N_VNew_Serial(static_cast<int64_t>(pN), context);

    ASSERT_NE(u, nullptr);
    ASSERT_NE(ones, nullptr);

    N_VConst(1.0, ones);

    ASSERT_EQ(KINInit(solver, computeSystem, u), KIN_SUCCESS);

    // Set our linear solver.

    SUNMatrix sunMatrix = nullptr;
    SUNLinearSolver sunLinearSolver = nullptr;

    if (mLinearSolver == LinearSolver::DENSE) {
        sunMatrix = SUNDenseMatrix(static_cast<int64_t>(pN), static_cast<int64_t>(pN), context);

        ASSERT_NE(sunMatrix, nullptr);

        sunLinearSolver = SUNLinSol_Dense(u, sunMatrix, context);
    } else if (mLinearSolver == LinearSolver::BANDED) {
        sunMatrix = SUNBandMatrix(static_cast<int64_t>(pN),
                                  static_cast<int64_t>(mUpperHalfBandwidth), static_cast<int64_t>(mLowerHalfBandwidth),
                                  context);

        ASSERT_NE(sunMatrix, nullptr);

        sunLinearSolver = SUNLinSol_Band(u, sunMatrix, context);
    } else {
        sunMatrix = nullptr;

        if (mLinearSolver == LinearSolver::GMRES) {
            sunLinearSolver = SUNLinSol_SPGMR(u, SUN_PREC_NONE, 0, context);
        } else if (mLinearSolver == LinearSolver::BICGSTAB) {
            sunLinearSolver = SUNLinSol_SPBCGS(u, SUN_PREC_NONE, 0, context);
        } else {
            sunLinearSolver = SUNLinSol_SPTFQMR(u, SUN_PREC_NONE, 0, context);
        }
    }

    ASSERT_NE(sunLinearSolver, nullptr);

    ASSERT_EQ(KINSetLinearSolver(solver, sunLinearSolver, sunMatrix), KINLS_SUCCESS);

    // Set our user data.

    SolverKinsolUserData userData;

    userData.computeSystem = pComputeSystem;
    userData.userData = pUserData;

    ASSERT_EQ(KINSetUserData(solver, &userData), KIN_SUCCESS);

    // Set our maximum number of iterations.

    ASSERT_EQ(KINSetNumMaxIters(solver, mMaximumNumberOfIterations), KIN_SUCCESS);

    // Solve the model.

#ifndef CODE_COVERAGE_ENABLED
    auto res =
#endif
        KINSol(solver, u, KIN_LINESEARCH, ones, ones);

    // Release some memory.

    N_VDestroy_Serial(u);
    N_VDestroy_Serial(ones);
    SUNMatDestroy(sunMatrix);
    SUNLinSolFree(sunLinearSolver);

    KINFree(&solver);

    SUNContext_PopErrHandler(context);

    SUNContext_Free(&context);

    // Check whether everything went fine.

#ifndef CODE_COVERAGE_ENABLED
    if (res < KIN_SUCCESS) {
        addError(mErrorMessage);

        return false;
    }
#endif

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
    return pimpl()->maximumNumberOfIterations();
}

void SolverKinsol::setMaximumNumberOfIterations(int pMaximumNumberOfIterations)
{
    pimpl()->setMaximumNumberOfIterations(pMaximumNumberOfIterations);
}

SolverKinsol::LinearSolver SolverKinsol::linearSolver() const
{
    return pimpl()->linearSolver();
}

void SolverKinsol::setLinearSolver(LinearSolver pLinearSolver)
{
    pimpl()->setLinearSolver(pLinearSolver);
}

int SolverKinsol::upperHalfBandwidth() const
{
    return pimpl()->upperHalfBandwidth();
}

void SolverKinsol::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    pimpl()->setUpperHalfBandwidth(pUpperHalfBandwidth);
}

int SolverKinsol::lowerHalfBandwidth() const
{
    return pimpl()->lowerHalfBandwidth();
}

void SolverKinsol::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    pimpl()->setLowerHalfBandwidth(pLowerHalfBandwidth);
}

} // namespace libOpenCOR

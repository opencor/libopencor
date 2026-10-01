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

#include "kinsol/kinsol.h"
#include "nvector/nvector_serial.h"
#include "sedml/SedAlgorithm.h"
#include "sunlinsol/sunlinsol_band.h"
#include "sunlinsol/sunlinsol_dense.h"
#include "sunlinsol/sunlinsol_spbcgs.h"
#include "sunlinsol/sunlinsol_spgmr.h"
#include "sunlinsol/sunlinsol_sptfqmr.h"

#include <utility>

namespace libOpenCOR {

// Some utilities.

namespace {

std::string toString(SolverKinsol::LinearSolver pLinearSolver)
{
    return (pLinearSolver == SolverKinsol::LinearSolver::DENSE) ?
               "Dense" :
           (pLinearSolver == SolverKinsol::LinearSolver::BANDED) ?
               "Banded" :
           (pLinearSolver == SolverKinsol::LinearSolver::GMRES) ?
               "GMRES" :
           (pLinearSolver == SolverKinsol::LinearSolver::BICGSTAB) ?
               "BiCGStab" :
               "TFQMR";
}

} // namespace

// Compute system.

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
    if (pErrorCode != KIN_WARNING) {
#endif
    *static_cast<std::string *>(pUserData) = pErrorMessage;
#ifndef CODE_COVERAGE_ENABLED
}
#endif
}

struct SolverKinsolUserData
{
    SolverNla::ComputeObjectiveFunction computeObjectiveFunction {nullptr};
    void *userData {nullptr};
    bool infOrNanFound {false};
};

int computeObjectiveFunction(N_Vector pU, N_Vector pF, void *pUserData)
{
    // Make sure that our input vector doesn't contain any Inf or NaN values.

    auto iMax {NV_LENGTH_S(pU)};
    auto *userData {static_cast<SolverKinsolUserData *>(pUserData)};

    for (sunindextype i = 0; i < iMax; ++i) {
        if (isInfOrNan(NV_Ith_S(pU, i))) {
            userData->infOrNanFound = true;

            return -1;
        }
    }

    userData->computeObjectiveFunction(N_VGetArrayPointer_Serial(pU), N_VGetArrayPointer_Serial(pF), userData->userData);

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
    if (mSunContext != nullptr) {
        freeSolverObjects();

        SUNContext_Free(&mSunContext);
    }
}

void SolverKinsol::Impl::freeSolverObjects()
{
    if (mSolver != nullptr) {
        N_VDestroy_Serial(mU);
        N_VDestroy_Serial(mOnes);

        SUNMatDestroy(mSunMatrix);
        SUNLinSolFree(mSunLinearSolver);

        KINFree(&mSolver);

        SUNContext_PopErrHandler(mSunContext);
    }
}

void SolverKinsol::Impl::populate(libsedml::SedAlgorithm *pAlgorithm)
{
    auto addUnknownParameterWarning = [this](const std::string &pKisaoId) {
        std::string warning;

        warning.reserve(pKisaoId.size() + 49); // NOLINT

        warning += "The parameter '";
        warning += pKisaoId;
        warning += "' is not recognised. It will be ignored.";

        addWarning(warning);
    };

    for (unsigned int i {0}; i < pAlgorithm->getNumAlgorithmParameters(); ++i) {
        auto *algorithmParameter {pAlgorithm->getAlgorithmParameter(i)};
        const auto &kisaoId {algorithmParameter->getKisaoID()};
        auto value {algorithmParameter->getValue()};

        if (kisaoId == "KISAO:0000486") {
            mMaximumNumberOfIterations = toInt(value);

            if (!isInt(value) || (mMaximumNumberOfIterations <= 0)) {
                const auto defaultIterations {toString(DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS)};
                std::string warning;

                warning.reserve(kisaoId.size() + value.size() + defaultIterations.size() + 130); // NOLINT

                warning += "The maximum number of iterations ('";
                warning += kisaoId;
                warning += "') cannot be equal to '";
                warning += value;
                warning += "'. It must be greater than 0. A maximum number of iterations of ";
                warning += defaultIterations;
                warning += " will be used instead.";

                addWarning(warning);

                mMaximumNumberOfIterations = DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS;
            }
        } else if (kisaoId == "KISAO:0000477") {
            if ((value != "Dense") && (value != "Banded") && (value != "GMRES") && (value != "BiCGStab") && (value != "TFQMR")) {
                const auto defaultLinearSolver {toString(DEFAULT_LINEAR_SOLVER)};
                std::string warning;

                warning.reserve(kisaoId.size() + value.size() + defaultLinearSolver.size() + 146); // NOLINT

                warning += "The linear solver ('";
                warning += kisaoId;
                warning += "') cannot be equal to '";
                warning += value;
                warning += "'. It must be equal to 'Dense', 'Banded', 'GMRES', 'BiCGStab', or 'TFQMR'. A ";
                warning += defaultLinearSolver;
                warning += " linear solver will be used instead.";

                addWarning(warning);

                value = toString(DEFAULT_LINEAR_SOLVER);
            }

            mLinearSolver = (value == "Dense") ?
                                SolverKinsol::LinearSolver::DENSE :
                            (value == "Banded") ?
                                SolverKinsol::LinearSolver::BANDED :
                            (value == "GMRES") ?
                                SolverKinsol::LinearSolver::GMRES :
                            (value == "BiCGStab") ?
                                SolverKinsol::LinearSolver::BICGSTAB :
                                SolverKinsol::LinearSolver::TFQMR;
        } else if (kisaoId == "KISAO:0000479") {
            mUpperHalfBandwidth = toInt(value);

            if (!isInt(value) || (mUpperHalfBandwidth < 0)) {
                const auto defaultUpperHalfBandwidth {toString(DEFAULT_UPPER_HALF_BANDWIDTH)};
                std::string warning;

                warning.reserve(kisaoId.size() + value.size() + defaultUpperHalfBandwidth.size() + 113); // NOLINT

                warning += "The upper half-bandwidth ('";
                warning += kisaoId;
                warning += "') cannot be equal to '";
                warning += value;
                warning += "'. It must be greater or equal to 0. An upper half-bandwidth of ";
                warning += defaultUpperHalfBandwidth;
                warning += " will be used instead.";

                addWarning(warning);

                mUpperHalfBandwidth = DEFAULT_UPPER_HALF_BANDWIDTH;
            }
        } else if (kisaoId == "KISAO:0000480") {
            mLowerHalfBandwidth = toInt(value);

            if (!isInt(value) || (mLowerHalfBandwidth < 0)) {
                const auto defaultLowerHalfBandwidth {toString(DEFAULT_LOWER_HALF_BANDWIDTH)};
                std::string warning;

                warning.reserve(kisaoId.size() + value.size() + defaultLowerHalfBandwidth.size() + 112); // NOLINT

                warning += "The lower half-bandwidth ('";
                warning += kisaoId;
                warning += "') cannot be equal to '";
                warning += value;
                warning += "'. It must be greater or equal to 0. A lower half-bandwidth of ";
                warning += defaultLowerHalfBandwidth;
                warning += " will be used instead.";

                addWarning(warning);

                mLowerHalfBandwidth = DEFAULT_LOWER_HALF_BANDWIDTH;
            }
        } else {
            addUnknownParameterWarning(kisaoId);
        }
    }
}

SolverPtr SolverKinsol::Impl::duplicate()
{
    auto solver {SolverKinsol::create()};
    auto *solverPimpl {solver->pimpl()};

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

int SolverKinsol::Impl::maximumNumberOfIterations() const noexcept
{
    return mMaximumNumberOfIterations;
}

void SolverKinsol::Impl::setMaximumNumberOfIterations(int pMaximumNumberOfIterations)
{
    mMaximumNumberOfIterations = pMaximumNumberOfIterations;
}

SolverKinsol::LinearSolver SolverKinsol::Impl::linearSolver() const noexcept
{
    return mLinearSolver;
}

void SolverKinsol::Impl::setLinearSolver(LinearSolver pLinearSolver)
{
    mLinearSolver = pLinearSolver;
}

int SolverKinsol::Impl::upperHalfBandwidth() const noexcept
{
    return mUpperHalfBandwidth;
}

void SolverKinsol::Impl::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    mUpperHalfBandwidth = pUpperHalfBandwidth;
}

int SolverKinsol::Impl::lowerHalfBandwidth() const noexcept
{
    return mLowerHalfBandwidth;
}

void SolverKinsol::Impl::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    mLowerHalfBandwidth = pLowerHalfBandwidth;
}

bool SolverKinsol::Impl::solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData)
{
    removeAllIssues();

    // We don't have any data associated with the given objective function, so get some by first making sure that the
    // solver's properties are all valid.

    if (mMaximumNumberOfIterations <= 0) {
        const auto maximumNumberOfIterations {toString(mMaximumNumberOfIterations)};
        std::string error;

        error.reserve(maximumNumberOfIterations.size() + 72); // NOLINT

        error += "The maximum number of iterations cannot be equal to ";
        error += maximumNumberOfIterations;
        error += ". It must be greater than 0.";

        addError(error);
    }

    bool needUpperAndLowerHalfBandwidths = false;

    if (mLinearSolver == LinearSolver::BANDED) {
        // We are dealing with a banded linear solver, so we need both an upper and a lower half-bandwidth.

        needUpperAndLowerHalfBandwidths = true;
    }

    if (needUpperAndLowerHalfBandwidths) {
        if ((mUpperHalfBandwidth < 0) || std::cmp_greater_equal(mUpperHalfBandwidth, pN)) {
            const auto upperHalfBandwidth {toString(mUpperHalfBandwidth)};
            const auto maximumUpperHalfBandwidth {toString(pN - 1)};
            std::string error;

            error.reserve(upperHalfBandwidth.size() + maximumUpperHalfBandwidth.size() + 62); // NOLINT

            error += "The upper half-bandwidth cannot be equal to ";
            error += upperHalfBandwidth;
            error += ". It must be between 0 and ";
            error += maximumUpperHalfBandwidth;
            error += ".";

            addError(error);
        }

        if ((mLowerHalfBandwidth < 0) || std::cmp_greater_equal(mLowerHalfBandwidth, pN)) {
            const auto lowerHalfBandwidth {toString(mLowerHalfBandwidth)};
            const auto maximumLowerHalfBandwidth {toString(pN - 1)};
            std::string error;

            error.reserve(lowerHalfBandwidth.size() + maximumLowerHalfBandwidth.size() + 62); // NOLINT

            error += "The lower half-bandwidth cannot be equal to ";
            error += lowerHalfBandwidth;
            error += ". It must be between 0 and ";
            error += maximumLowerHalfBandwidth;
            error += ".";

            addError(error);
        }
    }

    // Check whether we got some errors.

    if (hasErrors()) {
        return false;
    }

    // Create our SUNDIALS context, or reuse the cached one.

    if (mSunContext == nullptr) {
        ASSERT_EQ(SUNContext_Create(SUN_COMM_NULL, &mSunContext), 0);
    }

    // (Re)create our KINSOL solver and its associated objects if we have never created them, if the size of the NLA
    // system has changed, or if the linear solver settings have changed. Otherwise, reuse them since creating them is
    // expensive.

    if ((mSolver == nullptr)
        || (mCachedN != pN)
        || (mCachedLinearSolver != mLinearSolver)
        || (mCachedUpperHalfBandwidth != mUpperHalfBandwidth)
        || (mCachedLowerHalfBandwidth != mLowerHalfBandwidth)) {
        // Free our current KINSOL objects, if any.

        freeSolverObjects();

        // Create our KINSOL solver.

        mSolver = KINCreate(mSunContext);

        ASSERT_NE(mSolver, nullptr);

        // Use our own error handler and disable the logger.

        ASSERT_EQ(SUNContext_PushErrHandler(mSunContext, errorHandler, &mErrorMessage), KIN_SUCCESS);
        ASSERT_EQ(SUNContext_SetLogger(mSunContext, nullptr), KIN_SUCCESS);

        // Initialise our KINSOL solver.

        mU = N_VMake_Serial(static_cast<int64_t>(pN), pU, mSunContext);
        mOnes = N_VNew_Serial(static_cast<int64_t>(pN), mSunContext);

        ASSERT_NE(mU, nullptr);
        ASSERT_NE(mOnes, nullptr);

        N_VConst(1.0, mOnes);

        ASSERT_EQ(KINInit(mSolver, computeObjectiveFunction, mU), KIN_SUCCESS);

        // Set our linear solver.

        if (mLinearSolver == LinearSolver::DENSE) {
            mSunMatrix = SUNDenseMatrix(static_cast<int64_t>(pN), static_cast<int64_t>(pN), mSunContext);

            ASSERT_NE(mSunMatrix, nullptr);

            mSunLinearSolver = SUNLinSol_Dense(mU, mSunMatrix, mSunContext);
        } else if (mLinearSolver == LinearSolver::BANDED) {
            mSunMatrix = SUNBandMatrix(static_cast<int64_t>(pN),
                                       static_cast<int64_t>(mUpperHalfBandwidth), static_cast<int64_t>(mLowerHalfBandwidth),
                                       mSunContext);

            ASSERT_NE(mSunMatrix, nullptr);

            mSunLinearSolver = SUNLinSol_Band(mU, mSunMatrix, mSunContext);
        } else {
            mSunMatrix = nullptr;

            if (mLinearSolver == LinearSolver::GMRES) {
                mSunLinearSolver = SUNLinSol_SPGMR(mU, SUN_PREC_NONE, 0, mSunContext);
            } else if (mLinearSolver == LinearSolver::BICGSTAB) {
                mSunLinearSolver = SUNLinSol_SPBCGS(mU, SUN_PREC_NONE, 0, mSunContext);
            } else {
                mSunLinearSolver = SUNLinSol_SPTFQMR(mU, SUN_PREC_NONE, 0, mSunContext);
            }
        }

        ASSERT_NE(mSunLinearSolver, nullptr);

        ASSERT_EQ(KINSetLinearSolver(mSolver, mSunLinearSolver, mSunMatrix), KINLS_SUCCESS);

        // Keep track of what our KINSOL objects are associated with.

        mCachedN = pN;
        mCachedLinearSolver = mLinearSolver;
        mCachedUpperHalfBandwidth = mUpperHalfBandwidth;
        mCachedLowerHalfBandwidth = mLowerHalfBandwidth;
    }

    // Make our solution vector wrap the given solution array (which may differ from one call to another, e.g. if our
    // NLA system is solved from different places), which is much cheaper than recreating our solution vector.

    N_VSetArrayPointer_Serial(pU, mU);

    // Set our user data.

    SolverKinsolUserData userData;

    userData.computeObjectiveFunction = pComputeObjectiveFunction;
    userData.userData = pUserData;

    ASSERT_EQ(KINSetUserData(mSolver, &userData), KIN_SUCCESS);

    // Set our maximum number of iterations.

    ASSERT_EQ(KINSetNumMaxIters(mSolver, mMaximumNumberOfIterations), KIN_SUCCESS);

    // Solve the model.

    auto res = KINSol(mSolver, mU, KIN_LINESEARCH, mOnes, mOnes);

    // KINSOL limits the (scaled) length of a Newton step to 1,000 times the (scaled) norm of the initial guess (or to 1
    // if that norm is smaller than 1) and it gives up after five consecutive steps of that maximum length. So, if the
    // initial guess is (close to) zero, which is typically the case the first time that an NLA system is solved, then
    // KINSOL cannot move by more than about 5 from it and it fails, even if the NLA system is linear. If that happens,
    // then we try again from where KINSOL stopped, which means that the maximum length of a Newton step will be much
    // bigger.

    if (res == KIN_MXNEWT_5X_EXCEEDED) {
        res = KINSol(mSolver, mU, KIN_LINESEARCH, mOnes, mOnes);
    }

    // Check whether everything went fine.

    if (res < KIN_SUCCESS) {
        if (userData.infOrNanFound) {
            addError("The NLA system could not be solved (it contains some Inf and/or NaN values).");
        } else {
            addError(mErrorMessage);
        }

        return false;
    }

    return true;
}

SolverKinsol::SolverKinsol()
    : SolverNla(std::make_unique<Impl>())
{
}

SolverKinsol::~SolverKinsol() = default;

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

int SolverKinsol::maximumNumberOfIterations() const noexcept
{
    return pimpl()->maximumNumberOfIterations();
}

void SolverKinsol::setMaximumNumberOfIterations(int pMaximumNumberOfIterations)
{
    pimpl()->setMaximumNumberOfIterations(pMaximumNumberOfIterations);
}

SolverKinsol::LinearSolver SolverKinsol::linearSolver() const noexcept
{
    return pimpl()->linearSolver();
}

void SolverKinsol::setLinearSolver(LinearSolver pLinearSolver)
{
    pimpl()->setLinearSolver(pLinearSolver);
}

int SolverKinsol::upperHalfBandwidth() const noexcept
{
    return pimpl()->upperHalfBandwidth();
}

void SolverKinsol::setUpperHalfBandwidth(int pUpperHalfBandwidth)
{
    pimpl()->setUpperHalfBandwidth(pUpperHalfBandwidth);
}

int SolverKinsol::lowerHalfBandwidth() const noexcept
{
    return pimpl()->lowerHalfBandwidth();
}

void SolverKinsol::setLowerHalfBandwidth(int pLowerHalfBandwidth)
{
    pimpl()->setLowerHalfBandwidth(pLowerHalfBandwidth);
}

} // namespace libOpenCOR

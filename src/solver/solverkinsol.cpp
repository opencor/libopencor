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
#include "utils.h"

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

// Linear solvers.

const std::string SolverKinsol::Impl::DENSE_LINEAR_SOLVER = "Dense"; // NOLINT
const std::string SolverKinsol::Impl::BANDED_LINEAR_SOLVER = "Banded"; // NOLINT
const std::string SolverKinsol::Impl::GMRES_LINEAR_SOLVER = "GMRES"; // NOLINT
const std::string SolverKinsol::Impl::BICGSTAB_LINEAR_SOLVER = "BiCGStab"; // NOLINT
const std::string SolverKinsol::Impl::TFQMR_LINEAR_SOLVER = "TFQMR"; // NOLINT

// Properties information.

const std::string SolverKinsol::Impl::ID = "KISAO:0000282"; // NOLINT
const std::string SolverKinsol::Impl::NAME = "KINSOL"; // NOLINT

const std::string SolverKinsol::Impl::MAXIMUM_NUMBER_OF_ITERATIONS_ID = "KISAO:0000486"; // NOLINT
const std::string SolverKinsol::Impl::MAXIMUM_NUMBER_OF_ITERATIONS_NAME = "Maximum number of iterations"; // NOLINT

const std::string SolverKinsol::Impl::LINEAR_SOLVER_ID = "KISAO:0000477"; // NOLINT
const std::string SolverKinsol::Impl::LINEAR_SOLVER_NAME = "Linear solver"; // NOLINT
const StringVector SolverKinsol::Impl::LINEAR_SOLVER_LIST = {DENSE_LINEAR_SOLVER, BANDED_LINEAR_SOLVER, GMRES_LINEAR_SOLVER, BICGSTAB_LINEAR_SOLVER, TFQMR_LINEAR_SOLVER}; // NOLINT
const std::string SolverKinsol::Impl::LINEAR_SOLVER_DEFAULT_VALUE = DENSE_LINEAR_SOLVER; // NOLINT

const std::string SolverKinsol::Impl::UPPER_HALF_BANDWIDTH_ID = "KISAO:0000479"; // NOLINT
const std::string SolverKinsol::Impl::UPPER_HALF_BANDWIDTH_NAME = "Upper half-bandwidth"; // NOLINT

const std::string SolverKinsol::Impl::LOWER_HALF_BANDWIDTH_ID = "KISAO:0000480"; // NOLINT
const std::string SolverKinsol::Impl::LOWER_HALF_BANDWIDTH_NAME = "Lower half-bandwidth"; // NOLINT

// Compute system.

namespace {

int computeSystem(N_Vector pU, N_Vector pF, void *pUserData)
{
    auto *userData = static_cast<SolverKinsolUserData *>(pUserData);

    userData->computeSystem()(N_VGetArrayPointer_Serial(pU), N_VGetArrayPointer_Serial(pF), userData->userData());

    return 0;
}

} // namespace

// Solver user data.

SolverKinsolUserData::SolverKinsolUserData(SolverNla::ComputeSystem pComputeSystem, void *pUserData)
    : mComputeSystem(pComputeSystem)
    , mUserData(pUserData)
{
}

SolverNla::ComputeSystem SolverKinsolUserData::computeSystem() const
{
    return mComputeSystem;
}

void *SolverKinsolUserData::userData() const
{
    return mUserData;
}

// Solver.

SolverPtr SolverKinsol::Impl::create()
{
    return std::shared_ptr<SolverKinsol> {new SolverKinsol {}};
}

SolverPropertyPtrVector SolverKinsol::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGt0, MAXIMUM_NUMBER_OF_ITERATIONS_ID, MAXIMUM_NUMBER_OF_ITERATIONS_NAME,
                                     {},
                                     toString(MAXIMUM_NUMBER_OF_ITERATIONS_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::List, LINEAR_SOLVER_ID, LINEAR_SOLVER_NAME,
                                     LINEAR_SOLVER_LIST,
                                     LINEAR_SOLVER_DEFAULT_VALUE,
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGe0, UPPER_HALF_BANDWIDTH_ID, UPPER_HALF_BANDWIDTH_NAME,
                                     {},
                                     toString(UPPER_HALF_BANDWIDTH_DEFAULT_VALUE),
                                     false),
        Solver::Impl::createProperty(SolverProperty::Type::IntegerGe0, LOWER_HALF_BANDWIDTH_ID, LOWER_HALF_BANDWIDTH_NAME,
                                     {},
                                     toString(LOWER_HALF_BANDWIDTH_DEFAULT_VALUE),
                                     false),
    };
}

StringVector SolverKinsol::Impl::hiddenProperties(const StringStringMap &pProperties)
{
    StringVector res;
    auto linearSolver = valueFromProperties(LINEAR_SOLVER_ID, LINEAR_SOLVER_NAME, pProperties);

    if ((linearSolver == DENSE_LINEAR_SOLVER)
        || (linearSolver == GMRES_LINEAR_SOLVER)
        || (linearSolver == BICGSTAB_LINEAR_SOLVER)
        || (linearSolver == TFQMR_LINEAR_SOLVER)) {
        res.push_back(UPPER_HALF_BANDWIDTH_ID);
        res.push_back(LOWER_HALF_BANDWIDTH_ID);
    }

    return res;
}

SolverKinsol::Impl::Impl()
    : SolverNla::Impl()
{
    mIsValid = true;

    mProperties[MAXIMUM_NUMBER_OF_ITERATIONS_ID] = toString(MAXIMUM_NUMBER_OF_ITERATIONS_DEFAULT_VALUE);
    mProperties[LINEAR_SOLVER_ID] = LINEAR_SOLVER_DEFAULT_VALUE;
    mProperties[UPPER_HALF_BANDWIDTH_ID] = toString(UPPER_HALF_BANDWIDTH_DEFAULT_VALUE);
    mProperties[LOWER_HALF_BANDWIDTH_ID] = toString(LOWER_HALF_BANDWIDTH_DEFAULT_VALUE);
}

SolverKinsol::Impl::~Impl()
{
    if (mContext != nullptr) {
        N_VDestroy_Serial(mUVector);
        N_VDestroy_Serial(mOnesVector);
        SUNLinSolFree(mLinearSolver);
        SUNMatDestroy(mMatrix);

        KINFree(&mSolver);

        SUNContext_Free(&mContext);

        delete mKinsolUserData;
    }
}

StringStringMap SolverKinsol::Impl::propertiesId() const
{
    static const StringStringMap PROPERTIES_ID = {
        {MAXIMUM_NUMBER_OF_ITERATIONS_NAME, MAXIMUM_NUMBER_OF_ITERATIONS_ID},
        {LINEAR_SOLVER_NAME, LINEAR_SOLVER_ID},
        {UPPER_HALF_BANDWIDTH_NAME, UPPER_HALF_BANDWIDTH_ID},
        {LOWER_HALF_BANDWIDTH_NAME, LOWER_HALF_BANDWIDTH_ID},
    };

    return PROPERTIES_ID;
}

bool SolverKinsol::Impl::initialise(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    removeAllIssues();

    // Retrieve the solver's properties.

    bool ok = true;
    auto maximumNumberOfIterations = MAXIMUM_NUMBER_OF_ITERATIONS_DEFAULT_VALUE;
    auto linearSolver = LINEAR_SOLVER_DEFAULT_VALUE;
    auto upperHalfBandwidth = UPPER_HALF_BANDWIDTH_DEFAULT_VALUE;
    auto lowerHalfBandwidth = LOWER_HALF_BANDWIDTH_DEFAULT_VALUE;

    maximumNumberOfIterations = toInt(mProperties[MAXIMUM_NUMBER_OF_ITERATIONS_ID], ok);

    if (!ok || (maximumNumberOfIterations <= 0)) {
        addError(R"(The ")" + MAXIMUM_NUMBER_OF_ITERATIONS_NAME + R"(" property has an invalid value (")" + mProperties[MAXIMUM_NUMBER_OF_ITERATIONS_ID] + R"("). It must be a floating point number greater than zero.)");
    }

    linearSolver = mProperties[LINEAR_SOLVER_ID];

    if (std::find(LINEAR_SOLVER_LIST.begin(), LINEAR_SOLVER_LIST.end(), linearSolver) == LINEAR_SOLVER_LIST.end()) {
        addError(R"(The ")" + LINEAR_SOLVER_NAME + R"(" property has an invalid value (")" + mProperties[LINEAR_SOLVER_ID] + R"("). It must be equal to either ")" + DENSE_LINEAR_SOLVER + R"(", ")" + BANDED_LINEAR_SOLVER + R"(", ")" + GMRES_LINEAR_SOLVER + R"(", ")" + BICGSTAB_LINEAR_SOLVER + R"(", or ")" + TFQMR_LINEAR_SOLVER + R"(".)");
    } else {
        bool needUpperAndLowerHalfBandwidths = false;

        if (linearSolver == BANDED_LINEAR_SOLVER) {
            // We are dealing with a banded linear solver, so we need both an upper and a lower half bandwidth.

            needUpperAndLowerHalfBandwidths = true;
        }

        if (needUpperAndLowerHalfBandwidths) {
            upperHalfBandwidth = toInt(mProperties[UPPER_HALF_BANDWIDTH_ID], ok);

            if (!ok || (upperHalfBandwidth < 0) || (upperHalfBandwidth >= static_cast<int>(pN))) {
                addError(R"(The ")" + UPPER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[UPPER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pN) + R"(.)");
            }

            lowerHalfBandwidth = toInt(mProperties[LOWER_HALF_BANDWIDTH_ID], ok);

            if (!ok || (lowerHalfBandwidth < 0) || (lowerHalfBandwidth >= static_cast<int>(pN))) {
                addError(R"(The ")" + LOWER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[LOWER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pN) + R"(.)");
            }
        }
    }

    // Check whether we have had issues and, if so, then leave.

    if (!mIssues.empty()) {
        return false;
    }

    // Create our SUNDIALS context.

    ASSERT_EQ(SUNContext_Create(nullptr, &mContext), 0);

    // Create our KINSOL solver.

    mSolver = KINCreate(mContext);

    ASSERT_NE(mSolver, nullptr);

    // Initialise our CVODES solver.

    mUVector = N_VMake_Serial(static_cast<int64_t>(pN), pU, mContext);
    mOnesVector = N_VNew_Serial(static_cast<int64_t>(pN), mContext);

    ASSERT_NE(mUVector, nullptr);
    ASSERT_NE(mOnesVector, nullptr);

    N_VConst(1.0, mOnesVector);

    ASSERT_EQ(KINInit(mSolver, computeSystem, mUVector), KIN_SUCCESS);

    // Set our user data.

    mKinsolUserData = new SolverKinsolUserData(pComputeSystem, pUserData);

    ASSERT_EQ(KINSetUserData(mSolver, mKinsolUserData), KIN_SUCCESS);

    // Set our maximum number of iterations

    ASSERT_EQ(KINSetNumMaxIters(mSolver, maximumNumberOfIterations), KIN_SUCCESS);

    // Set our linear solver.

    if (linearSolver == DENSE_LINEAR_SOLVER) {
        mMatrix = SUNDenseMatrix(static_cast<int64_t>(pN), static_cast<int64_t>(pN), mContext);

        ASSERT_NE(mMatrix, nullptr);

        mLinearSolver = SUNLinSol_Dense(mUVector, mMatrix, mContext);
    } else if (linearSolver == BANDED_LINEAR_SOLVER) {
        mMatrix = SUNBandMatrix(static_cast<int64_t>(pN),
                                static_cast<int64_t>(upperHalfBandwidth), static_cast<int64_t>(lowerHalfBandwidth),
                                mContext);

        ASSERT_NE(mMatrix, nullptr);

        mLinearSolver = SUNLinSol_Band(mUVector, mMatrix, mContext);
    } else {
        mMatrix = nullptr;

        if (linearSolver == GMRES_LINEAR_SOLVER) {
            mLinearSolver = SUNLinSol_SPGMR(mUVector, PREC_NONE, 0, mContext);
        } else if (linearSolver == BICGSTAB_LINEAR_SOLVER) {
            mLinearSolver = SUNLinSol_SPBCGS(mUVector, PREC_NONE, 0, mContext);
        } else {
            mLinearSolver = SUNLinSol_SPTFQMR(mUVector, PREC_NONE, 0, mContext);
        }
    }

    ASSERT_NE(mLinearSolver, nullptr);

    ASSERT_EQ(KINSetLinearSolver(mSolver, mLinearSolver, mMatrix), KINLS_SUCCESS);

    // Initialise the NLA solver itself.

    return SolverNla::Impl::initialise(pComputeSystem, pU, pN, pUserData);
}

bool SolverKinsol::Impl::solve()
{
    // Solve the model.

    ASSERT_EQ(KINSol(mSolver, mUVector, KIN_LINESEARCH, mOnesVector, mOnesVector), KIN_SUCCESS);

    return true;
}

SolverKinsol::SolverKinsol()
    : SolverNla(new Impl())
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

SolverInfoPtr SolverKinsol::info() const
{
    return Solver::solversInfo()[Solver::Impl::SolversIndex[SolverKinsol::Impl::ID]];
}

bool SolverKinsol::initialise(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->initialise(pComputeSystem, pU, pN, pUserData);
}

bool SolverKinsol::solve()
{
    return pimpl()->solve();
}

} // namespace libOpenCOR

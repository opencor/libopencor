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

    userData->computeSystem(N_VGetArrayPointer_Serial(pU), N_VGetArrayPointer_Serial(pF), userData->userData);

    return 0;
}

} // namespace

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
    mProperties[MAXIMUM_NUMBER_OF_ITERATIONS_ID] = toString(MAXIMUM_NUMBER_OF_ITERATIONS_DEFAULT_VALUE);
    mProperties[LINEAR_SOLVER_ID] = LINEAR_SOLVER_DEFAULT_VALUE;
    mProperties[UPPER_HALF_BANDWIDTH_ID] = toString(UPPER_HALF_BANDWIDTH_DEFAULT_VALUE);
    mProperties[LOWER_HALF_BANDWIDTH_ID] = toString(LOWER_HALF_BANDWIDTH_DEFAULT_VALUE);
}

SolverKinsol::Impl::~Impl()
{
    for (auto &data : mData) {
        N_VDestroy_Serial(data.second.uVector);
        N_VDestroy_Serial(data.second.onesVector);
        SUNMatDestroy(data.second.matrix);
        SUNLinSolFree(data.second.linearSolver);

        KINFree(&data.second.solver);

        SUNContext_Free(&data.second.context);

        delete data.second.userData;
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

bool SolverKinsol::Impl::solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    removeAllIssues();

    // Check whether we need to initialise our data or whether we can just reuse it.

    SolverKinsolData data;

    if (auto iter = mData.find(pComputeSystem); iter == mData.end()) {
        // We don't have any data associated with the given pComputeSystem, so get some by first making sure that the
        // solver's properties are all valid.

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
                    addError(R"(The ")" + UPPER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[UPPER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pN - 1) + R"(.)");
                }

                lowerHalfBandwidth = toInt(mProperties[LOWER_HALF_BANDWIDTH_ID], ok);

                if (!ok || (lowerHalfBandwidth < 0) || (lowerHalfBandwidth >= static_cast<int>(pN))) {
                    addError(R"(The ")" + LOWER_HALF_BANDWIDTH_NAME + R"(" property has an invalid value (")" + mProperties[LOWER_HALF_BANDWIDTH_ID] + R"("). It must be an integer number between 0 and )" + toString(pN - 1) + R"(.)");
                }
            }
        }

        // Check whether we had issues and, if so, then leave.

        if (!mIssues.empty()) {
            return false;
        }

        // Create our SUNDIALS context.

        ASSERT_EQ(SUNContext_Create(nullptr, &data.context), 0);

        // Create our KINSOL solver.

        data.solver = KINCreate(data.context);

        ASSERT_NE(data.solver, nullptr);

        // Initialise our KINSOL solver.

        data.uVector = N_VMake_Serial(static_cast<int64_t>(pN), pU, data.context);
        data.onesVector = N_VNew_Serial(static_cast<int64_t>(pN), data.context);

        ASSERT_NE(data.uVector, nullptr);
        ASSERT_NE(data.onesVector, nullptr);

        N_VConst(1.0, data.onesVector);

        ASSERT_EQ(KINInit(data.solver, computeSystem, data.uVector), KIN_SUCCESS);

        // Set our linear solver.

        if (linearSolver == DENSE_LINEAR_SOLVER) {
            data.matrix = SUNDenseMatrix(static_cast<int64_t>(pN), static_cast<int64_t>(pN), data.context);

            ASSERT_NE(data.matrix, nullptr);

            data.linearSolver = SUNLinSol_Dense(data.uVector, data.matrix, data.context);
        } else if (linearSolver == BANDED_LINEAR_SOLVER) {
            data.matrix = SUNBandMatrix(static_cast<int64_t>(pN),
                                        static_cast<int64_t>(upperHalfBandwidth), static_cast<int64_t>(lowerHalfBandwidth),
                                        data.context);

            ASSERT_NE(data.matrix, nullptr);

            data.linearSolver = SUNLinSol_Band(data.uVector, data.matrix, data.context);
        } else {
            data.matrix = nullptr;

            if (linearSolver == GMRES_LINEAR_SOLVER) {
                data.linearSolver = SUNLinSol_SPGMR(data.uVector, PREC_NONE, 0, data.context);
            } else if (linearSolver == BICGSTAB_LINEAR_SOLVER) {
                data.linearSolver = SUNLinSol_SPBCGS(data.uVector, PREC_NONE, 0, data.context);
            } else {
                data.linearSolver = SUNLinSol_SPTFQMR(data.uVector, PREC_NONE, 0, data.context);
            }
        }

        ASSERT_NE(data.linearSolver, nullptr);

        ASSERT_EQ(KINSetLinearSolver(data.solver, data.linearSolver, data.matrix), KINLS_SUCCESS);

        // Set our user data.

        data.userData = new SolverKinsolUserData {pComputeSystem, pUserData};

        ASSERT_EQ(KINSetUserData(data.solver, data.userData), KIN_SUCCESS);

        // Set our maximum number of iterations.

        ASSERT_EQ(KINSetNumMaxIters(data.solver, maximumNumberOfIterations), KIN_SUCCESS);

        // Keep track of our data.

        mData[pComputeSystem] = data;
    } else {
        // We are already initialised, so just reuse our previous data.

        data = iter->second;
    }

    // Solve the model.
    // Note: we use ASSERT_GE() since upon success KINSol() will return either KIN_SUCCESS, KIN_INITIAL_GUESS_OK, or
    //       KIN_STEP_LT_STPTOL.

    ASSERT_GE(KINSol(data.solver, data.uVector, KIN_LINESEARCH, data.onesVector, data.onesVector), KIN_SUCCESS);

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

/*---GRY---
const SolverKinsol::Impl *SolverKinsol::pimpl() const
{
    return static_cast<const Impl *>(SolverNla::pimpl());
}
*/

SolverInfoPtr SolverKinsol::info() const
{
    return Solver::solversInfo()[Solver::Impl::SolversIndex[SolverKinsol::Impl::ID]];
}

Solver::Type SolverKinsol::type() const
{
    return Type::NLA;
}

std::string SolverKinsol::id() const
{
    return Impl::ID;
}

std::string SolverKinsol::name() const
{
    return Impl::NAME;
}

bool SolverKinsol::solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData)
{
    return pimpl()->solve(pComputeSystem, pU, pN, pUserData);
}

} // namespace libOpenCOR

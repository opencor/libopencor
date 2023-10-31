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

#pragma once

#include "solvernla_p.h"

#include "libopencor/solverkinsol.h"

#include "sundialsbegin.h"
#include "sundials/sundials_linearsolver.h"
#include "sundials/sundials_matrix.h"
#include "sundialsend.h"

namespace libOpenCOR {

struct SolverKinsolUserData
{
    SolverNla::ComputeSystem computeSystem = nullptr;

    void *userData = nullptr;
};

struct SolverKinsolData
{
    SUNContext context = nullptr;

    void *solver = nullptr;

    N_Vector uVector = nullptr;
    N_Vector onesVector = nullptr;

    SUNMatrix matrix = nullptr;
    SUNLinearSolver linearSolver = nullptr;

    SolverKinsolUserData *userData = nullptr;
};

class SolverKinsol::Impl: public SolverNla::Impl
{
public:
    // Linear solvers.

    static const std::string DENSE_LINEAR_SOLVER;
    static const std::string BANDED_LINEAR_SOLVER;
    static const std::string GMRES_LINEAR_SOLVER;
    static const std::string BICGSTAB_LINEAR_SOLVER;
    static const std::string TFQMR_LINEAR_SOLVER;

    // Properties information.

    static const Solver::Type TYPE = Solver::Type::NLA;
    static const std::string ID;
    static const std::string NAME;

    static const std::string MAXIMUM_NUMBER_OF_ITERATIONS_ID;
    static const std::string MAXIMUM_NUMBER_OF_ITERATIONS_NAME;
    static constexpr int MAXIMUM_NUMBER_OF_ITERATIONS_DEFAULT_VALUE = 200;

    static const std::string LINEAR_SOLVER_ID;
    static const std::string LINEAR_SOLVER_NAME;
    static const StringVector LINEAR_SOLVER_LIST;
    static const std::string LINEAR_SOLVER_DEFAULT_VALUE;

    static const std::string UPPER_HALF_BANDWIDTH_ID;
    static const std::string UPPER_HALF_BANDWIDTH_NAME;
    static constexpr int UPPER_HALF_BANDWIDTH_DEFAULT_VALUE = 0;

    static const std::string LOWER_HALF_BANDWIDTH_ID;
    static const std::string LOWER_HALF_BANDWIDTH_NAME;
    static constexpr int LOWER_HALF_BANDWIDTH_DEFAULT_VALUE = 0;

    // Solver.

    static SolverPtr create();
    static SolverPropertyPtrVector propertiesInfo();
    static StringVector hiddenProperties(const StringStringMap &pProperties);

    std::map<ComputeSystem, SolverKinsolData> mData;

    explicit Impl();
    ~Impl() override;

    StringStringMap propertiesId() const override;

    bool solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData) override;
};

} // namespace libOpenCOR

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

#include "solverode_p.h"
#include "solvercvode.h"

#include "sundialsbegin.h"
#include "sundials/sundials_linearsolver.h"
#include "sundials/sundials_matrix.h"
#include "sundials/sundials_nonlinearsolver.h"
#include "sundialsend.h"

namespace libOpenCOR {

class SolverCvodeUserData
{
public:
    explicit SolverCvodeUserData(double *pVariables, SolverOde::ComputeRates pComputeRates);

    double *variables() const;
    SolverOde::ComputeRates computeRates() const;

private:
    double *mVariables = nullptr;
    SolverOde::ComputeRates mComputeRates = nullptr;
};

class SolverCvode::Impl: public SolverOde::Impl
{
public:
    // Integration methods.

    static const std::string ADAMS_MOULTON_METHOD;
    static const std::string BDF_METHOD;

    // Iteration types.

    static const std::string FUNCTIONAL_ITERATION_TYPE;
    static const std::string NEWTON_ITERATION_TYPE;

    // Linear solvers.

    static const std::string DENSE_LINEAR_SOLVER;
    static const std::string BANDED_LINEAR_SOLVER;
    static const std::string DIAGONAL_LINEAR_SOLVER;
    static const std::string GMRES_LINEAR_SOLVER;
    static const std::string BICGSTAB_LINEAR_SOLVER;
    static const std::string TFQMR_LINEAR_SOLVER;

    // Preconditioners.

    static const std::string NO_PRECONDITIONER;
    static const std::string BANDED_PRECONDITIONER;

    // Properties information.

    static const Solver::Type TYPE = Solver::Type::ODE;
    static const std::string ID;
    static const std::string NAME;

    static const std::string MAXIMUM_STEP_ID;
    static const std::string MAXIMUM_STEP_NAME;
    static constexpr double MAXIMUM_STEP_DEFAULT_VALUE = 0.0;

    static const std::string MAXIMUM_NUMBER_OF_STEPS_ID;
    static const std::string MAXIMUM_NUMBER_OF_STEPS_NAME;
    static constexpr int MAXIMUM_NUMBER_OF_STEPS_DEFAULT_VALUE = 500;

    static const std::string INTEGRATION_METHOD_ID;
    static const std::string INTEGRATION_METHOD_NAME;
    static const std::vector<std::string> INTEGRATION_METHOD_LIST;
    static const std::string INTEGRATION_METHOD_DEFAULT_VALUE;

    static const std::string ITERATION_TYPE_ID;
    static const std::string ITERATION_TYPE_NAME;
    static const std::vector<std::string> ITERATION_TYPE_LIST;
    static const std::string ITERATION_TYPE_DEFAULT_VALUE;

    static const std::string LINEAR_SOLVER_ID;
    static const std::string LINEAR_SOLVER_NAME;
    static const std::vector<std::string> LINEAR_SOLVER_LIST;
    static const std::string LINEAR_SOLVER_DEFAULT_VALUE;

    static const std::string PRECONDITIONER_ID;
    static const std::string PRECONDITIONER_NAME;
    static const std::vector<std::string> PRECONDITIONER_LIST;
    static const std::string PRECONDITIONER_DEFAULT_VALUE;

    static const std::string UPPER_HALF_BANDWIDTH_ID;
    static const std::string UPPER_HALF_BANDWIDTH_NAME;
    static constexpr int UPPER_HALF_BANDWIDTH_DEFAULT_VALUE = 0;

    static const std::string LOWER_HALF_BANDWIDTH_ID;
    static const std::string LOWER_HALF_BANDWIDTH_NAME;
    static constexpr int LOWER_HALF_BANDWIDTH_DEFAULT_VALUE = 0;

    static const std::string RELATIVE_TOLERANCE_ID;
    static const std::string RELATIVE_TOLERANCE_NAME;
    static constexpr double RELATIVE_TOLERANCE_DEFAULT_VALUE = 1.0e-7;

    static const std::string ABSOLUTE_TOLERANCE_ID;
    static const std::string ABSOLUTE_TOLERANCE_NAME;
    static constexpr double ABSOLUTE_TOLERANCE_DEFAULT_VALUE = 1.0e-7;

    static const std::string INTERPOLATE_SOLUTION_ID;
    static const std::string INTERPOLATE_SOLUTION_NAME;
    static constexpr bool INTERPOLATE_SOLUTION_DEFAULT_VALUE = true;

    // Solver.

    static SolverPtr create();
    static std::vector<SolverPropertyPtr> propertiesInfo();
    static std::vector<std::string> hiddenProperties(const std::map<std::string, std::string> &pProperties);

    SUNContext mContext = nullptr;

    void *mSolver = nullptr;

    N_Vector mStatesVector = nullptr;

    SUNMatrix mMatrix = nullptr;
    SUNLinearSolver mLinearSolver = nullptr;
    SUNNonlinearSolver mNonLinearSolver = nullptr;

    SolverCvodeUserData *mUserData = nullptr;

    bool mInterpolateSolution = INTERPOLATE_SOLUTION_DEFAULT_VALUE;

    explicit Impl();
    ~Impl() override;

    std::map<std::string, std::string> propertiesId() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;
    bool reinitialise(double pVoi) override;

    bool solve(double &pVoi, double pVoiEnd) const override;
};

} // namespace libOpenCOR

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

#include "utils.h"

#include "tests/utils.h"

#include <array>
#include <libopencor>
#include <span>
#include <vector>

TEST(CoverageSolverTest, odeChanges)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::WARNING, "Task instance | Change attribute: the variable of integration 'time' in component 'environment' cannot be changed. Only state variables and constants can be changed."},
        {libOpenCOR::Issue::Type::WARNING, "Task instance | Change attribute: the variable 'X' in component 'membrane' could not be found and therefore could not be changed."},
        {libOpenCOR::Issue::Type::WARNING, "Task instance | Change attribute: the computed constant 'E_Na' in component 'sodium_channel' cannot be changed. Only state variables and constants can be changed."},
        {libOpenCOR::Issue::Type::WARNING, "Task instance | Change attribute: the algebraic variable 'i_Stim' in component 'membrane' cannot be changed. Only state variables and constants can be changed."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/ode_sed_changes.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(CoverageSolverTest, algebraicChanges)
{
    // We want to solve a system of three unknowns:
    // Variables:
    //  • a: 0
    //  • x: 1 ->  3
    //  • y: 1 -> -5
    //  • z: 1 ->  7
    // Equations:
    //  • k = a
    //  •  x +  y +  z +  k =  5
    //  • 6x - 4y + 5z - 3k = 73
    //  • 5x + 2y + 2z - 5k = 19
    //
    // When a = 0, we have:
    //  • x =  3
    //  • y = -5
    //  • z =  7
    // but our SED-ML file has a change attribute that sets a to 13, so we should now have:
    //  • x =  100/3  =  33.333333333333336
    //  • y = -356/27 = -13.185185185185185
    //  • z = -760/27 = -28.148148148148145

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/algebraic_sed_changes.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    static const auto ABS_TOL {1e-05};

    const auto &instanceTask {instance->tasks()[0]};

    EXPECT_EQ(instanceTask->stateCount(), 0U);
    EXPECT_EQ(instanceTask->rateCount(), 0U);
    EXPECT_EQ(instanceTask->constantCount(), 1U);
    EXPECT_EQ(instanceTask->computedConstantCount(), 1U);
    EXPECT_EQ(instanceTask->algebraicVariableCount(), 3U);

    EXPECT_NEAR(instanceTask->algebraicVariable(0)[0], -28.14815, ABS_TOL);
    EXPECT_NEAR(instanceTask->algebraicVariable(1)[0], -13.18519, ABS_TOL);
    EXPECT_NEAR(instanceTask->algebraicVariable(2)[0], 33.33333, ABS_TOL);
}

namespace {

const auto FIRST_TARGET {1.0};
const auto SECOND_TARGET {2.0};
const auto THIRD_TARGET {3.0};

struct KinsolSolveData
{
    std::vector<double> targets;
};

void computeObjectiveFunction(double *pU, double *pF, void *pUserData) // NOLINT
{
    const auto &targets {static_cast<const KinsolSolveData *>(pUserData)->targets};
    const std::span<double> f {pF, targets.size()};
    const std::span<const double> u {pU, targets.size()};

    for (size_t i {0}; i < targets.size(); ++i) {
        f[i] = u[i] - targets[i];
    }
}

void expectKinsolSolveSolution(std::span<const double> pU, const std::vector<double> &pExpected)
{
    static const auto ABS_TOL {1e-05};

    for (size_t i {0}; i < pExpected.size(); ++i) {
        EXPECT_NEAR(pU[i], pExpected[i], ABS_TOL); // NOLINT
    }
}

} // namespace

TEST(CoverageSolverTest, kinsolSolveWithChangedSettings)
{
    // Solve different NLA systems (both in terms of size and settings) using the same solver instance to make sure that
    // the underlying KINSOL objects of each NLA system are created, reused, and recreated as needed.

    auto solver {libOpenCOR::SolverKinsol::create()};

    // Solve our first NLA system (2 unknowns) using the default settings: create the KINSOL objects.

    KinsolSolveData data2 {{FIRST_TARGET, SECOND_TARGET}};
    std::array<double, 2> u2 {0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u2.data(), 2, &data2));

    expectKinsolSolveSolution(u2, data2.targets);

    // Solve our first NLA system again, using the same settings: reuse its KINSOL objects (and Jacobian).

    std::array<double, 2> u2b {0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u2b.data(), 2, &data2));

    expectKinsolSolveSolution(u2b, data2.targets);

    // Solve our second NLA system (3 unknowns), still using the default settings: create its KINSOL objects.

    KinsolSolveData data3 {{FIRST_TARGET, SECOND_TARGET, THIRD_TARGET}};
    std::array<double, 3> u3 {0.0, 0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u3.data(), 3, &data3));

    expectKinsolSolveSolution(u3, data3.targets);

    // Solve our first NLA system again: reuse its KINSOL objects (and Jacobian), even though we solved our second NLA
    // system in between.

    std::array<double, 2> u2c {0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u2c.data(), 2, &data2));

    expectKinsolSolveSolution(u2c, data2.targets);

    // Solve our second NLA system with a different linear solver: recreate its KINSOL objects.

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    std::array<double, 3> u3b {0.0, 0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u3b.data(), 3, &data3));

    expectKinsolSolveSolution(u3b, data3.targets);

    // Solve our second NLA system with a different upper half-bandwidth: recreate its KINSOL objects.

    solver->setUpperHalfBandwidth(1);

    std::array<double, 3> u3c {0.0, 0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u3c.data(), 3, &data3));

    expectKinsolSolveSolution(u3c, data3.targets);

    // Solve our second NLA system with a different lower half-bandwidth: recreate its KINSOL objects.

    solver->setLowerHalfBandwidth(2);

    std::array<double, 3> u3d {0.0, 0.0, 0.0};

    EXPECT_TRUE(solver->solve(computeObjectiveFunction, u3d.data(), 3, &data3));

    expectKinsolSolveSolution(u3d, data3.targets);
}

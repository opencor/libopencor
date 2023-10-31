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

#include "odemodel.h"
#include "solvers.h"
#include "utils.h"

#include "tests/utils.h"

#include <libopencor>

TEST(CvodeSolverTest, basic)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverOde>(libOpenCOR::Solver::create("CVODE"));

    checkCvodeSolver(solver->info());
}

TEST(CvodeSolverTest, maximumStepValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum step" property has an invalid value ("abc"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Maximum step", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, maximumStepValueWithInvalidNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum step" property has an invalid value ("-1.234"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Maximum step", "-1.234");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of steps" property has an invalid value ("abc"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of steps", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, maximumNumberOfStepsValueWithInvalidNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Maximum number of steps" property has an invalid value ("0"). It must be a floating point number greater than zero.)"},
    };

    solver->setProperty("Maximum number of steps", "0");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, integrationMethodValueWithUnknownValue)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Integration method" property has an invalid value ("Unknown integration method"). It must be equal to either "Adams-Moulton" or "BDF".)"},
    };

    solver->setProperty("Integration method", "Unknown integration method");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, iterationTypeValueWithUnknownValue)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Iteration type" property has an invalid value ("Unknown iteration type"). It must be equal to either "Functional" or "Newton".)"},
    };

    solver->setProperty("Iteration type", "Unknown iteration type");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, linearSolverValueWithUnknownValue)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Linear solver" property has an invalid value ("Unknown linear solver"). It must be equal to either "Dense", "Banded", "Diagonal", "GMRES", "BiCGStab", or "TFQMR".)"},
    };

    solver->setProperty("Linear solver", "Unknown linear solver");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, preconditionerValueWithUnknownValue)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Preconditioner" property has an invalid value ("Unknown preconditioner"). It must be equal to either "None" or "Banded".)"},
    };

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "Unknown preconditioner");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "-1");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("4"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "4");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "-1");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("4"). It must be an integer number between 0 and 3.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "4");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, relativeToleranceValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Relative tolerance" property has an invalid value ("abc"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Relative tolerance", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, relativeToleranceValueWithInvalidNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Relative tolerance" property has an invalid value ("-1.234"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Relative tolerance", "-1.234");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Absolute tolerance" property has an invalid value ("abc"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Absolute tolerance", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, absoluteToleranceValueWithInvalidNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Absolute tolerance" property has an invalid value ("-1.234"). It must be a floating point number greater or equal to zero.)"},
    };

    solver->setProperty("Absolute tolerance", "-1.234");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, interpolateSolutionValueWithNonBoolean)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Interpolate solution" property has an invalid value ("Non boolean"). It must be equal to either "True" or "False".)"},
    };

    solver->setProperty("Interpolate solution", "Non boolean");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solve)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419, 0.5960555, 0.053035143, 0.317770590};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.000001, 0.0000001, 0.000000001, 0.000000001};

    solver->setProperty("Interpolate solution", "False");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419, 0.5960555, 0.0530351, 0.3177705};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.000001, 0.0000001, 0.0000001, 0.0000001};

    solver->setProperty("Integration method", "Adams-Moulton");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.01541, 0.5960555, 0.0530351, 0.317770};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.00001, 0.0000001, 0.0000001, 0.000001};

    solver->setProperty("Iteration type", "Functional");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.015, 0.596, 0.0530, 0.317};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.001, 0.001, 0.0001, 0.001};

    solver->setProperty("Linear solver", "Banded");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.053035, 0.317770};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.000001, 0.000001};

    solver->setProperty("Linear solver", "Diagonal");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.053032, 0.317773};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.000001, 0.000001};

    solver->setProperty("Linear solver", "GMRES");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.0530327, 0.317773};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.0000001, 0.000001};

    solver->setProperty("Linear solver", "BiCGStab");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0150, 0.59605, 0.053032, 0.317773};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.00001, 0.000001, 0.000001};

    solver->setProperty("Linear solver", "TFQMR");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "None");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.0530351, 0.3177705};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.0000001, 0.0000001};

    solver->setProperty("Linear solver", "BiCGStab");
    solver->setProperty("Preconditioner", "None");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

    static const libOpenCOR::Doubles FINAL_STATES = {-0.0154, 0.596055, 0.053035, 0.3177705};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0001, 0.000001, 0.000001, 0.0000001};

    solver->setProperty("Linear solver", "TFQMR");
    solver->setProperty("Preconditioner", "None");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, ABSOLUTE_ERRORS);

    OdeModel::finalise(states, rates, variables);
}

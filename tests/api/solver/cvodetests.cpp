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
#include "solvercvode.h"
#include "solvers.h"
#include "utils.h"

#include "gtest/gtest.h"

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
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Upper half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, gmresLinearSolverBandedPreconditionerAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "Banded");
    solver->setProperty("Upper half-bandwidth", "-1");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, gmresLinearSolverBandedPreconditionerAndUpperHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Upper half-bandwidth" property has an invalid value ("5"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "Banded");
    solver->setProperty("Upper half-bandwidth", "5");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNonNumber)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("abc"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "Banded");
    solver->setProperty("Lower half-bandwidth", "abc");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, gmresLinearSolverBandedPreconditionerAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("-1"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "Banded");
    solver->setProperty("Lower half-bandwidth", "-1");

    EXPECT_FALSE(solver->initialise(0.0, OdeModel::STATE_COUNT, states, rates, variables, OdeModel::computeRates));
    EXPECT_EQ_ISSUES(solver, EXPECTED_ISSUES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, gmresLinearSolverBandedPreconditionerAndLowerHalfBandwidthValueWithNumberTooBig)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Customise and initialise our solver using a step value that is not a floating point number.

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, R"(The "Lower half-bandwidth" property has an invalid value ("5"). It must be an integer number between 0 and 4.)"},
    };

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "Banded");
    solver->setProperty("Lower half-bandwidth", "5");

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

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015422246861901662, 0.59605559394022689, 0.053035160895529773, 0.31777058539774627};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01542143979535311, 0.59605559223195803, 0.053035155728368123, 0.31777058661072155};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015423275579583704, 0.5960556037980016, 0.053035167225244692, 0.31777058077104786};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015423275401558252, 0.59605560379714717, 0.053035167223952032, 0.31777058077138343};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420735373646405, 0.59605558367032696, 0.05303515120051977, 0.31777058753168086};
#endif

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithoutInterpolateSolution)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419488982115767, 0.59605557837764489, 0.053035143314278957, 0.31777059042454153};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419505744664089, 0.59605558108520129, 0.053035143420552169, 0.31777059048157241};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01541949677292458, 0.59605558049794471, 0.05303514336586436, 0.31777059050965351};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419508480756496, 0.59605557817998434, 0.053035143433755365, 0.31777059031732957};
#endif

    solver->setProperty("Interpolate solution", "False");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithAdamsMoultonIntegrationMethod)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419476274389506, 0.5960555805060318, 0.053035143146875303, 0.3177705899261562};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419107686697044, 0.59605558582776363, 0.053035134081144138, 0.31777058234874039};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419651193701917, 0.59605558139371528, 0.053035144284069587, 0.31777058731428287};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.01541862843163708, 0.5960555690051027, 0.053035138073828716, 0.31777059521202838};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419524192581236, 0.59605557879092774, 0.053035145613795749, 0.31777058994460156};
#endif

    solver->setProperty("Integration method", "Adams-Moulton");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithFunctionalIterationType)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015417168379188925, 0.59605554922958837, 0.053035128431690226, 0.31777060793435058};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015418081015203026, 0.59605558654292023, 0.053035137377108223, 0.31777058389761448};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419225488609868, 0.59605557966989142, 0.053035141505482788, 0.31777058855693807};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419450943615409, 0.5960555863622764, 0.053035126367920661, 0.31777058514038992};
#endif

    solver->setProperty("Iteration type", "Functional");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBandedLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015436049186730106, 0.59614348051893584, 0.053035340907827341, 0.31777538958064028};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015332371199014691, 0.59614086367837016, 0.053034787528920539, 0.31777690024220973};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015510601949815407, 0.59602537447035719, 0.053041753926196041, 0.31784343269969317};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015454674826158126, 0.59614393857546732, 0.053035446412259296, 0.31777511340167913};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015297069679523696, 0.5961402839536516, 0.053034618540828818, 0.3177776000625106};
#endif

    solver->setProperty("Linear solver", "Banded");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithDiagonalLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015433543514261655, 0.5960557856251123, 0.053035228191243039, 0.31777057192188574};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015417730690056964, 0.59605567217327449, 0.053035116423762914, 0.31777033986574205};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015427989244415146, 0.59605561222348091, 0.053035201291268194, 0.31777061620839886};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420959503828912, 0.59605530714545696, 0.053035148843076509, 0.31777055784271463};
#endif

    solver->setProperty("Linear solver", "Diagonal");

    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015018415770354697, 0.59605799037870133, 0.053032724544783298, 0.31777320687476535};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015026018016690759, 0.59605796529385202, 0.053032767447661895, 0.31777323057832618};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015023436195144091, 0.59605799587317765, 0.053032755470967828, 0.31777321272156883};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015012037023967591, 0.59605795455930988, 0.053032685692110247, 0.31777320945924542};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015021211583899895, 0.59605801825378757, 0.053032741816368641, 0.31777318543482858};
#endif

    solver->setProperty("Linear solver", "GMRES");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015018683481544955, 0.5960579468265822, 0.053032728909756646, 0.31777324560552661};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015021086237854139, 0.59605805426938774, 0.053032747273425669, 0.31777316657221261};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015022069947637299, 0.59605796799537947, 0.053032749659952375, 0.31777321697215377};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015023701783099076, 0.59605795783371862, 0.053032771718052256, 0.31777322807855024};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.01501735489017168, 0.59605794815048185, 0.053032731566004181, 0.31777323174139266};
#endif

    solver->setProperty("Linear solver", "BiCGStab");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolver)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015023277852779513, 0.59605789570716972, 0.053032760685114241, 0.31777319523611247};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015019292878253831, 0.59605790954266469, 0.053032741312881798, 0.31777331240608214};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015022260204111017, 0.59605798792073117, 0.053032739944760703, 0.31777319808053672};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.01503263232877855, 0.59605806342894863, 0.05303281178107061, 0.31777316676647616};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015019630087747961, 0.59605796103564324, 0.053032734038507313, 0.3177732093055472};
#endif

    solver->setProperty("Linear solver", "TFQMR");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithGmresLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420241462898464, 0.5960556173818069, 0.053035147575641185, 0.31777058072809794};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015418551780382105, 0.59605558999793573, 0.053035139566581964, 0.31777057984596135};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015421120988824191, 0.59605561310286714, 0.053035154765445257, 0.31777056515715035};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015421100653792983, 0.59605561275721741, 0.053035154636183142, 0.31777056538913895};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015419131974789706, 0.5960556246035319, 0.053035133913277126, 0.31777055729945081};
#endif

    solver->setProperty("Linear solver", "GMRES");
    solver->setProperty("Preconditioner", "None");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithBicgstabLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015426160811306358, 0.59605556211856814, 0.0530351873280605, 0.31777056973560031};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015415434929722006, 0.59605555488716266, 0.05303511180865704, 0.3177705841606504};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420012921211236, 0.59605557656156427, 0.053035147184900162, 0.31777058433284211};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015423176141823008, 0.59605565484251111, 0.053035157889579979, 0.3177705661864616};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015427314383742497, 0.59605566758093353, 0.053035189395094454, 0.31777054276865774};
#endif

    solver->setProperty("Linear solver", "BiCGStab");
    solver->setProperty("Preconditioner", "None");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

TEST(CvodeSolverTest, solveWithTfqmrLinearSolverAndNoPreconditioner)
{
    const auto [solver, states, rates, variables] = OdeModel::initialise("CVODE");

    // Compute our model.

#if defined(BUILDING_ON_WINDOWS)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420329039865358, 0.59605558939744285, 0.05303514570766775, 0.31777058417358589};
#elif defined(BUILDING_ON_LINUX)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015423244611025172, 0.59605559992263824, 0.053035167502874167, 0.31777058076971293};
#elif defined(BUILDING_ON_INTEL)
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015421515168224747, 0.59605559157697641, 0.053035157008747044, 0.31777058519681284};
    static const libOpenCOR::Doubles FINAL_STATES_2 = {-0.015419456183592662, 0.59605558226749378, 0.053035143053885993, 0.31777058899266469};
#else
    static const libOpenCOR::Doubles FINAL_STATES = {-0.015420448454150297, 0.59605559894084548, 0.05303514861517649, 0.31777057679851456};
#endif

    solver->setProperty("Linear solver", "TFQMR");
    solver->setProperty("Preconditioner", "None");

#if defined(BUILDING_ON_MACOS) && defined(BUILDING_ON_INTEL)
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES, FINAL_STATES_2);
#else
    OdeModel::compute(solver, states, rates, variables, FINAL_STATES);
#endif

    OdeModel::finalise(states, rates, variables);
}

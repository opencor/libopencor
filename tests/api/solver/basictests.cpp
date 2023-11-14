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

#include "tests/utils.h"

#include <libcellml>
#include <libopencor>

namespace {

void checkModel(const std::string &pModelType, const std::string &pVersion = {}) // NOLINT
{
    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(libOpenCOR::textFileContents("api/solver/" + pModelType + "/model" + pVersion + ".cellml"));

    EXPECT_EQ(parser->issueCount(), size_t(0));

    auto analyser = libcellml::Analyser::create();

    analyser->analyseModel(model);

    EXPECT_EQ(analyser->issueCount(), size_t(0));
    for (size_t i = 0; i < analyser->issueCount(); ++i) {
        printf(">>> %s\n", analyser->issue(i)->description().c_str()); // NOLINT
    }

    auto analyserModel = analyser->model();
    auto generator = libcellml::Generator::create();

    generator->setModel(analyserModel);

    auto profile = generator->profile();

    profile->setInterfaceHeaderString("#pragma once\n");
    profile->setImplementationHeaderString("#include \"[INTERFACE_FILE_NAME]\"\n");

    if (pModelType == "ode") {
        profile->setImplementationCreateStatesArrayMethodString("double * createStatesArray()\n"
                                                                "{\n"
                                                                "    double *res = (double *) malloc(STATE_COUNT*sizeof(double));\n"
                                                                "\n"
                                                                "    for (size_t i = 0; i < STATE_COUNT; ++i) {\n"
                                                                "        res[i] = strtod(\"NAN\", nullptr);\n"
                                                                "    }\n"
                                                                "\n"
                                                                "    return res;\n"
                                                                "}\n");
        profile->setImplementationCreateVariablesArrayMethodString("double * createVariablesArray()\n"
                                                                   "{\n"
                                                                   "    double *res = (double *) malloc(VARIABLE_COUNT*sizeof(double));\n"
                                                                   "\n"
                                                                   "    for (size_t i = 0; i < VARIABLE_COUNT; ++i) {\n"
                                                                   "        res[i] = strtod(\"NAN\", nullptr);\n"
                                                                   "    }\n"
                                                                   "\n"
                                                                   "    return res;\n"
                                                                   "}\n");
    } else {
        profile->setInterfaceFileNameString("model" + pVersion + ".h");
        profile->setInterfaceVariableCountString("extern const size_t VARIABLE_COUNT_" + pVersion + ";\n");
        profile->setImplementationVariableCountString("const size_t VARIABLE_COUNT_" + pVersion + " = [VARIABLE_COUNT];\n");
        profile->setImplementationCreateVariablesArrayMethodString("double * createVariablesArray()\n"
                                                                   "{\n"
                                                                   "    double *res = (double *) malloc(VARIABLE_COUNT_"
                                                                   + pVersion + "*sizeof(double));\n"
                                                                                "\n"
                                                                                "    for (size_t i = 0; i < VARIABLE_COUNT_"
                                                                   + pVersion + "; ++i) {\n"
                                                                                "        res[i] = strtod(\"NAN\", nullptr);\n"
                                                                                "    }\n"
                                                                                "\n"
                                                                                "    return res;\n"
                                                                                "}\n");
        profile->setExternNlaSolveMethodString("");
        profile->setNlaSolveCallString(false, "libOpenCOR::nlaSolve(KINSOL_NLA_SOLVER, objectiveFunction[INDEX], u, [SIZE], &rfi);\n");
    }

    EXPECT_EQ(generator->interfaceCode(), libOpenCOR::textFileContents("api/solver/" + pModelType + "/model" + pVersion + ".h"));
    EXPECT_EQ(generator->implementationCode(), libOpenCOR::textFileContents("api/solver/" + pModelType + "/model" + pVersion + ".c"));
}

} // namespace

TEST(BasicSolverTest, checkOdeModel)
{
    checkModel("ode");
}

TEST(BasicSolverTest, checkNlaModel1)
{
    checkModel("nla", "1");
}

TEST(BasicSolverTest, checkNlaModel2)
{
    checkModel("nla", "2");
}

TEST(BasicSolverTest, SolverCvode)
{
    static const auto MAXIMUM_STEP = 1.23;
    static const auto MAXIMUM_NUMBER_OF_STEPS = 123;
    static const auto INTEGRATION_METHOD = libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON;
    static const auto ITERATION_TYPE = libOpenCOR::SolverCvode::IterationType::FUNCTIONAL;
    static const auto LINEAR_SOLVER = libOpenCOR::SolverCvode::LinearSolver::GMRES;
    static const auto PRECONDITIONER = libOpenCOR::SolverCvode::Preconditioner::NO;
    static const auto UPPER_HALF_BANDWIDTH = 3;
    static const auto LOWER_HALF_BANDWIDTH = 5;
    static const auto RELATIVE_TOLERANCE = 1.23e-5;
    static const auto ABSOLUTE_TOLERANCE = 3.45e-7;
    static const auto INTERPOLATE_SOLUTION = false;

    auto solver = libOpenCOR::SolverCvode::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000019");
    EXPECT_EQ(solver->name(), "CVODE");

    EXPECT_EQ(solver->maximumStep(), 0.0);
    EXPECT_EQ(solver->maximumNumberOfSteps(), 500);
    EXPECT_EQ(solver->integrationMethod(), libOpenCOR::SolverCvode::IntegrationMethod::BDF);
    EXPECT_EQ(solver->iterationType(), libOpenCOR::SolverCvode::IterationType::NEWTON);
    EXPECT_EQ(solver->linearSolver(), libOpenCOR::SolverCvode::LinearSolver::DENSE);
    EXPECT_EQ(solver->preconditioner(), libOpenCOR::SolverCvode::Preconditioner::BANDED);
    EXPECT_EQ(solver->upperHalfBandwidth(), 0);
    EXPECT_EQ(solver->lowerHalfBandwidth(), 0);
    EXPECT_EQ(solver->relativeTolerance(), 1.0e-7);
    EXPECT_EQ(solver->absoluteTolerance(), 1.0e-7);
    EXPECT_EQ(solver->interpolateSolution(), true);

    solver->setMaximumStep(MAXIMUM_STEP);
    solver->setMaximumNumberOfSteps(MAXIMUM_NUMBER_OF_STEPS);
    solver->setIntegrationMethod(INTEGRATION_METHOD);
    solver->setIterationType(ITERATION_TYPE);
    solver->setLinearSolver(LINEAR_SOLVER);
    solver->setPreconditioner(PRECONDITIONER);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);
    solver->setRelativeTolerance(RELATIVE_TOLERANCE);
    solver->setAbsoluteTolerance(ABSOLUTE_TOLERANCE);
    solver->setInterpolateSolution(INTERPOLATE_SOLUTION);

    EXPECT_EQ(solver->maximumStep(), MAXIMUM_STEP);
    EXPECT_EQ(solver->maximumNumberOfSteps(), MAXIMUM_NUMBER_OF_STEPS);
    EXPECT_EQ(solver->integrationMethod(), INTEGRATION_METHOD);
    EXPECT_EQ(solver->iterationType(), ITERATION_TYPE);
    EXPECT_EQ(solver->linearSolver(), LINEAR_SOLVER);
    EXPECT_EQ(solver->preconditioner(), PRECONDITIONER);
    EXPECT_EQ(solver->upperHalfBandwidth(), UPPER_HALF_BANDWIDTH);
    EXPECT_EQ(solver->lowerHalfBandwidth(), LOWER_HALF_BANDWIDTH);
    EXPECT_EQ(solver->relativeTolerance(), RELATIVE_TOLERANCE);
    EXPECT_EQ(solver->absoluteTolerance(), ABSOLUTE_TOLERANCE);
    EXPECT_EQ(solver->interpolateSolution(), INTERPOLATE_SOLUTION);
}

TEST(BasicSolverTest, SolverForwardEuler)
{
    static const auto STEP = 0.123;

    auto solver = libOpenCOR::SolverForwardEuler::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000030");
    EXPECT_EQ(solver->name(), "Forward Euler");

    EXPECT_EQ(solver->step(), 1.0);

    solver->setStep(STEP);

    EXPECT_EQ(solver->step(), STEP);
}

TEST(BasicSolverTest, SolverFourthOrderRungeKutta)
{
    static const auto STEP = 0.123;

    auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000032");
    EXPECT_EQ(solver->name(), "Fourth-order Runge-Kutta");

    EXPECT_EQ(solver->step(), 1.0);

    solver->setStep(STEP);

    EXPECT_EQ(solver->step(), STEP);
}

TEST(BasicSolverTest, SolverHeun)
{
    static const auto STEP = 0.123;

    auto solver = libOpenCOR::SolverHeun::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000301");
    EXPECT_EQ(solver->name(), "Heun");

    EXPECT_EQ(solver->step(), 1.0);

    solver->setStep(STEP);

    EXPECT_EQ(solver->step(), STEP);
}

TEST(BasicSolverTest, SolverKinsol)
{
    static const auto MAXIMUM_NUMBER_OF_ITERATIONS = 123;
    static const auto LINEAR_SOLVER = libOpenCOR::SolverKinsol::LinearSolver::GMRES;
    static const auto UPPER_HALF_BANDWIDTH = 3;
    static const auto LOWER_HALF_BANDWIDTH = 5;

    auto solver = libOpenCOR::SolverKinsol::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::NLA);
    EXPECT_EQ(solver->id(), "KISAO:0000282");
    EXPECT_EQ(solver->name(), "KINSOL");

    EXPECT_EQ(solver->maximumNumberOfIterations(), 200);
    EXPECT_EQ(solver->linearSolver(), libOpenCOR::SolverKinsol::LinearSolver::DENSE);
    EXPECT_EQ(solver->upperHalfBandwidth(), 0);
    EXPECT_EQ(solver->lowerHalfBandwidth(), 0);

    solver->setMaximumNumberOfIterations(MAXIMUM_NUMBER_OF_ITERATIONS);
    solver->setLinearSolver(LINEAR_SOLVER);
    solver->setUpperHalfBandwidth(UPPER_HALF_BANDWIDTH);
    solver->setLowerHalfBandwidth(LOWER_HALF_BANDWIDTH);

    EXPECT_EQ(solver->maximumNumberOfIterations(), MAXIMUM_NUMBER_OF_ITERATIONS);
    EXPECT_EQ(solver->linearSolver(), LINEAR_SOLVER);
    EXPECT_EQ(solver->upperHalfBandwidth(), UPPER_HALF_BANDWIDTH);
    EXPECT_EQ(solver->lowerHalfBandwidth(), LOWER_HALF_BANDWIDTH);
}

TEST(BasicSolverTest, SolverSecondOrderRungeKutta)
{
    static const auto STEP = 0.123;

    auto solver = libOpenCOR::SolverSecondOrderRungeKutta::create();

    EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    EXPECT_EQ(solver->id(), "KISAO:0000381");
    EXPECT_EQ(solver->name(), "Second-order Runge-Kutta");

    EXPECT_EQ(solver->step(), 1.0);

    solver->setStep(STEP);

    EXPECT_EQ(solver->step(), STEP);
}

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

    if (pModelType == "nla") {
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
                                                                                "        res[i] = NAN;\n"
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

TEST(BasicSolverTest, cvodeById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000019");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, cvodeByName)
{
    auto solver = libOpenCOR::Solver::create("CVODE");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, forwardEulerById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000030");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, forwardEulerByName)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, fourthOrderRungeKuttaById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000032");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, fourthOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Fourth-order Runge-Kutta");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, heunById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000301");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, heunByName)
{
    auto solver = libOpenCOR::Solver::create("Heun");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, kinsolById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000282");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, kinsolByName)
{
    auto solver = libOpenCOR::Solver::create("KINSOL");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, secondOrderRungeKuttaById)
{
    auto solver = libOpenCOR::Solver::create("KISAO:0000381");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, secondOrderRungeKuttaByName)
{
    auto solver = libOpenCOR::Solver::create("Second-order Runge-Kutta");

    EXPECT_NE(solver, nullptr);
}

TEST(BasicSolverTest, properties)
{
    auto solver = libOpenCOR::Solver::create("Forward Euler");
    auto properties = solver->properties();

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("KISAO:0000483"), "1");
    EXPECT_EQ(solver->property("Step"), "1");

    solver->setProperty("Step", "1.23");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("KISAO:0000483"), "1.23");

    solver->setProperty("KISAO:0000483", "7.89");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");

    solver->setProperty("Unknown", "1.23");

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "7.89");
    EXPECT_EQ(solver->property("Unknown"), "");

    properties["Step"] = "1.23";

    EXPECT_EQ(properties.size(), 2);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");

    properties["Unknown"] = "1.23";

    EXPECT_EQ(properties.size(), 3);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1");

    properties["KISAO:0000483"] = "1.23";

    EXPECT_EQ(properties.size(), 3);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1.23");

    properties.erase("KISAO:0000483");

    EXPECT_EQ(properties.size(), 2);

    solver->setProperties(properties);

    EXPECT_EQ(solver->properties().size(), 1);
    EXPECT_EQ(solver->property("Step"), "1.23");
}

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

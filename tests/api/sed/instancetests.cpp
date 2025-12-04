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

#include <libopencor>

TEST(InstanceSedTest, noFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "The simulation experiment description does not contain any tasks to run."},
    }};

    auto document {libOpenCOR::SedDocument::create()};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, invalidCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is invalid."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_CELLML_FILE))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, overconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: variable 'x' in component 'my_component' is overconstrained."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/overconstrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, underconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is underconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/underconstrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, unsuitablyConstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Model: the CellML file is unsuitably constrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: variable 'y' in component 'my_component' is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Model | CellML | Analyser: the type of variable 'x' in component 'my_component' is unknown."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unsuitably_constrained.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, algebraicModel)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, odeModel)
{
    const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
#ifdef BUILDING_ON_INTEL
        {libOpenCOR::Issue::Type::ERROR, "Task | CVODE: at t = 0.00140013827899996, mxstep steps taken before reaching tout."},
#else
        {libOpenCOR::Issue::Type::ERROR,
#    ifdef BUILDING_ON_WINDOWS
         "Task | CVODE: at t = 0.00140013827899821, mxstep steps taken before reaching tout."
#    else
         "Task | CVODE: at t = 0.00140013827899707, mxstep steps taken before reaching tout."
#    endif
        },
#endif
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    static const auto NOK_MAXIMUM_NUMBER_OF_STEPS {10};

    auto cvode {std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver())};

    cvode->setMaximumNumberOfSteps(NOK_MAXIMUM_NUMBER_OF_STEPS);

    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    static const auto OK_MAXIMUM_NUMBER_OF_STEPS {500};

    cvode->setMaximumNumberOfSteps(OK_MAXIMUM_NUMBER_OF_STEPS);

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, odeModelWithNoOdeSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE))};
    auto document {libOpenCOR::SedDocument::create(file)};

    document->simulations()[0]->setOdeSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, nlaModel)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0])};
    auto kinsol {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver())};

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->instantiate();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, nlaModelWithNoNlaSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};

    document->simulations()[0]->setNlaSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, daeModel)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task instance | KINSOL: the upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};
    auto kinsol {std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver())};

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    instance->run();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->instantiate();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelWithNoOdeOrNlaSolver)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
        {libOpenCOR::Issue::Type::ERROR, "Task | Simulation: simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    }};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto simulation {document->simulations()[0]};

    simulation->setOdeSolver(nullptr);
    simulation->setNlaSolver(nullptr);

    auto instance {document->instantiate()};

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(InstanceSedTest, combineArchive)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, combineArchiveWithCellmlFileAsMasterFile)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/cellml_file_as_master_file.omex"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    auto instance {document->instantiate()};

    EXPECT_FALSE(instance->hasIssues());
}

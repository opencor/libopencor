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

#include "gtest/gtest.h"

#include "tests/utils.h"

#include <libopencor>

TEST(InstanceSedTest, noFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The simulation experiment description does not contain any tasks to run."},
    };

    auto document = libOpenCOR::SedDocument::create();
    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, invalidCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is invalid."},
        {libOpenCOR::Issue::Type::ERROR, "Equation 'x+y+z' in component 'my_component' is not an equality statement (i.e. LHS = RHS)."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_CELLML_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, overconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is overconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "Variable 'x' in component 'my_component' is computed more than once."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/overconstrained.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, underconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is underconstrained."},
        {libOpenCOR::Issue::Type::ERROR, "The type of variable 'x' in component 'my_component' is unknown."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/underconstrained.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, unsuitablyConstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is unsuitably constrained."},
        {libOpenCOR::Issue::Type::ERROR, "Variable 'y' in component 'my_component' is computed more than once."},
        {libOpenCOR::Issue::Type::ERROR, "The type of variable 'x' in component 'my_component' is unknown."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unsuitably_constrained.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

namespace {

void runAlgebraicModel(bool pCompiled)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance(pCompiled);

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

} // namespace

TEST(InstanceSedTest, compiledAlgebraicModel)
{
    runAlgebraicModel(true);
}

TEST(InstanceSedTest, interpretedAlgebraicModel)
{
    runAlgebraicModel(false);
}

namespace {

void runOdeModel(bool pCompiled)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
#if defined(BUILDING_ON_WINDOWS)
        {libOpenCOR::Issue::Type::ERROR, "At t = 0.00140014, mxstep steps taken before reaching tout."},
#elif defined(BUILDING_ON_LINUX)
        {libOpenCOR::Issue::Type::ERROR, "At t = 1, mxstep steps taken before reaching tout."},
#else
#    ifdef BUILDING_ON_INTEL
        {libOpenCOR::Issue::Type::ERROR, "At t = 5.00577e+06, mxstep steps taken before reaching tout."},
#    else
        {libOpenCOR::Issue::Type::ERROR, "At t = 0.00140014, mxstep steps taken before reaching tout."},
#    endif
#endif
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);

    static const auto NOK_MAXIMUM_NUMBER_OF_STEPS = 10;

    auto cvode = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    cvode->setMaximumNumberOfSteps(NOK_MAXIMUM_NUMBER_OF_STEPS);

    auto instance = document->createInstance(pCompiled);

    EXPECT_FALSE(instance->hasIssues());

    instance->run();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    static const auto OK_MAXIMUM_NUMBER_OF_STEPS = 500;

    cvode->setMaximumNumberOfSteps(OK_MAXIMUM_NUMBER_OF_STEPS);

    instance = document->createInstance(pCompiled);

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

} // namespace

TEST(InstanceSedTest, compiledOdeModel)
{
    runOdeModel(true);
}

TEST(InstanceSedTest, interpretedOdeModel)
{
    runOdeModel(false);
}

TEST(InstanceSedTest, odeModelWithNoOdeSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    document->simulations()[0]->setOdeSolver(nullptr);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, nlaModel)
{
    //---GRY--- AS FOR THE ALGEBRAIC AND ODE MODELS, WE WILL NEED TO ADD AN INTERPRETED VERSION OF THIS TEST.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto kinsol = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->createInstance();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, nlaModelWithNoNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    document->simulations()[0]->setNlaSolver(nullptr);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, daeModel)
{
    //---GRY--- AS FOR THE ALGEBRAIC AND ODE MODELS, WE WILL NEED TO ADD AN INTERPRETED VERSION OF THIS TEST.

    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);
    auto kinsol = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    kinsol->setUpperHalfBandwidth(-1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    instance->run();

    EXPECT_EQ_ISSUES(instance, expectedIssues);

    kinsol->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::DENSE);

    instance = document->createInstance();

    instance->run();

    EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, daeModelWithNoOdeOrNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an ODE solver but none is provided."},
        {libOpenCOR::Issue::Type::ERROR, "Simulation 'simulation1' is to be used with model 'model1' which requires an NLA solver but none is provided."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];

    simulation->setOdeSolver(nullptr);
    simulation->setNlaSolver(nullptr);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, expectedIssues);
}

TEST(InstanceSedTest, combineArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    instance->run();

    //---GRY--- TO BE UNCOMMENTED ONCE WE CAN RUN A COMBINE ARCHIVE.
    // EXPECT_FALSE(instance->hasIssues());
}

TEST(InstanceSedTest, combineArchiveWithCellmlFileAsMasterFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/cellml_file_as_master_file.omex"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    EXPECT_FALSE(instance->hasIssues());
}

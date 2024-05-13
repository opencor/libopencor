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

TEST(KinsolSolverTest, maximumNumberOfIterationsValueWithInvalidNumber)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The maximum number of iterations cannot be equal to 0. It must be greater than 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setMaximumNumberOfIterations(0);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooSmall)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to -1. It must be between 0 and 2."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model2.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(-1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(KinsolSolverTest, bandedLinearSolverAndUpperHalfBandwidthValueWithNumberTooBig)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The upper half-bandwidth cannot be equal to 1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooSmall)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to -1. It must be between 0 and 2."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model2.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(-1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

TEST(KinsolSolverTest, bandedLinearSolverAndLowerHalfBandwidthValueWithNumberTooBig)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::ERROR, "The lower half-bandwidth cannot be equal to 1. It must be between 0 and 0."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setLowerHalfBandwidth(1);

    auto instance = document->createInstance();

    EXPECT_EQ_ISSUES(instance, EXPECTED_ISSUES);
}

//---GRY--- RENAME THIS TEST compiledSolve AND CREATE ONE CALLED interpretedSolve ONCE WE CAN INTERPRET NLA-BASED
//          MODELS.

TEST(KinsolSolverTest, solve)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto instance = document->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(KinsolSolverTest, solveWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model2.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);
    solver->setUpperHalfBandwidth(2);
    solver->setLowerHalfBandwidth(2);

    auto instance = document->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(KinsolSolverTest, solveWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    auto instance = document->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(KinsolSolverTest, solveWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model2.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB);

    auto instance = document->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

TEST(KinsolSolverTest, solveWithTfqmrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/solver/nla/model1.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = dynamic_pointer_cast<libOpenCOR::SedSteadyState>(document->simulations()[0]);
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    auto instance = document->createInstance();

    instance->run();

    //---GRY--- CHECK THE FINAL VALUE OF THE STATES, RATES, AND VARIABLES.
}

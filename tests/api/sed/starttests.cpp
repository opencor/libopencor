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

TEST(StartSedDocumentTest, noFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description must (currently) have exactly one model."},
        {libOpenCOR::Issue::Type::ERROR, "A simulation experiment description must (currently) have exactly one simulation."},
    };

    auto sed = libOpenCOR::SedDocument::create();

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, cellmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_TRUE(sed->start());
}

TEST(StartSedDocumentTest, invalidCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is invalid."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_CELLML_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, overconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is overconstrained."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/overconstrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, underconstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is underconstrained."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/underconstrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, unsuitablyConstrainedCellmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The CellML file is unsuitably constrained."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/unsuitably_constrained.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, odeModelWithNoOdeSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The simulation is to be linked to an ODE model and must therefore specify an ODE solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];

    EXPECT_TRUE(sed->start());

    simulation->setOdeSolver(nullptr);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, nlaModelWithNoNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The simulation is to be linked to an NLA model and must therefore specify an NLA solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];

    EXPECT_TRUE(sed->start());

    simulation->setNlaSolver(nullptr);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

TEST(StartSedDocumentTest, daeModelWithNoOdeOrNlaSolver)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::ERROR, "The simulation is to be linked to a DAE model and must therefore specify an ODE solver."},
        {libOpenCOR::Issue::Type::ERROR, "The simulation is to be linked to a DAE model and must therefore specify an NLA solver."},
    };

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];

    EXPECT_TRUE(sed->start());

    simulation->setOdeSolver(nullptr);
    simulation->setNlaSolver(nullptr);

    EXPECT_FALSE(sed->start());
    EXPECT_EQ_ISSUES(sed, expectedIssues);
}

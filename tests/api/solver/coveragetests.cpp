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

#include <libopencor>

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

    auto instanceTask {instance->tasks()[0]};

    EXPECT_EQ(instanceTask->stateCount(), 0);
    EXPECT_EQ(instanceTask->rateCount(), 0);
    EXPECT_EQ(instanceTask->constantCount(), 1);
    EXPECT_EQ(instanceTask->computedConstantCount(), 1);
    EXPECT_EQ(instanceTask->algebraicCount(), 3);

    EXPECT_NEAR(instanceTask->algebraic(0)[0], -28.14815, ABS_TOL);
    EXPECT_NEAR(instanceTask->algebraic(1)[0], -13.18519, ABS_TOL);
    EXPECT_NEAR(instanceTask->algebraic(2)[0], 33.33333, ABS_TOL);
}

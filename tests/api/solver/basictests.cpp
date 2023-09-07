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

TEST(BasicSolverTest, solversInfo)
{
}

TEST(BasicSolverTest, ForwardEulerMethod)
{
    auto solver = libOpenCOR::Solver::create(libOpenCOR::Solver::Method::FORWARD_EULER);

    //---GRY---
    // EXPECT_EQ(solver->type(), libOpenCOR::Solver::Type::ODE);
    // EXPECT_EQ(solver->name(), "Forward Euler");
    // EXPECT_EQ_STRINGS(solver->properties(), {"Step"});
}

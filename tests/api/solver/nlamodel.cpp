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

#include "nlamodel.h"
#include "utils.h"

#include "gtest/gtest.h"

#include <cmath>

namespace NlaModel {

namespace {

libOpenCOR::SolverNla *KINSOL_NLA_SOLVER = nullptr;

} // namespace

#if defined(BUILDING_USING_MSVC)
#    pragma warning(push)
#    pragma warning(disable: 4100)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdouble-promotion"
#    pragma clang diagnostic ignored "-Wmissing-prototypes"
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#include "../../res/api/solver/nla/model.c" // NOLINT

#if defined(BUILDING_USING_MSVC)
#    pragma warning(pop)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic pop
#else
#    pragma clang diagnostic pop
#endif

std::tuple<std::shared_ptr<libOpenCOR::SolverNla>, double *> initialise(const std::string &pSolverName)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverNla>(libOpenCOR::Solver::create(pSolverName));
    auto *variables = createVariablesArray();

    KINSOL_NLA_SOLVER = solver.get();

    initialiseVariables(variables);
    computeComputedConstants(variables);

    static const libOpenCOR::Doubles GUESSES = {1.0, 1.0, 1.0};
    static const libOpenCOR::Doubles ABSOLUTE_ERRORS = {0.0, 0.0, 0.0};

    EXPECT_EQ_DOUBLES(variables, GUESSES, ABSOLUTE_ERRORS);

    return std::make_tuple(solver, variables);
}

void compute(double *pVariables, const libOpenCOR::Doubles &pSolutions, const libOpenCOR::Doubles &pAbsoluteErrors)
{
    computeVariables(pVariables);

    EXPECT_EQ_DOUBLES(pVariables, pSolutions, pAbsoluteErrors);
}

void finalise(double *pVariables)
{
    deleteArray(pVariables);
}

} // namespace NlaModel

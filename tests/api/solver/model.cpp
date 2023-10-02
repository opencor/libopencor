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

#include "model.h"
#include "utils.h"

#include "gtest/gtest.h"

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
#    pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include "res/model.c" // NOLINT

#if defined(BUILDING_USING_MSVC)
#    pragma warning(pop)
#elif defined(BUILDING_USING_GNU)
#    pragma GCC diagnostic pop
#else
#    pragma clang diagnostic pop
#endif

std::tuple<std::shared_ptr<libOpenCOR::SolverOde>, double *, double *, double *> createAndInitialiseArraysAndCreateSolver(const std::string &pSolverName)
{
    auto solver = std::static_pointer_cast<libOpenCOR::SolverOde>(libOpenCOR::Solver::create(pSolverName));
    auto *states = createStatesArray();
    auto *rates = createStatesArray();
    auto *variables = createVariablesArray();

    initialiseVariables(states, rates, variables);
    computeComputedConstants(variables);
    computeRates(0.0, states, rates, variables);
    computeVariables(0.0, states, rates, variables);

    return std::make_tuple(solver, states, rates, variables);
}

void computeModel(const std::shared_ptr<libOpenCOR::SolverOde> &pSolver,
                  double *pStates, double *pRates, double *pVariables,
                  const libOpenCOR::Doubles &pFinalStates)
{
    // Initialise the model.

    static constexpr auto VOI_START = 0.0;
    static const libOpenCOR::Doubles INITIAL_STATES = {0.0, 0.6, 0.05, 0.325};

    EXPECT_TRUE(pSolver->initialise(VOI_START, STATE_COUNT, pStates, pRates, pVariables, computeRates));
    EXPECT_EQ_DOUBLES(pStates, INITIAL_STATES);

    // Compute the model.

    static constexpr auto VOI_END = 50.0;
    static constexpr auto VOI_INTERVAL = 0.1;

    auto voi = VOI_START;
    size_t voiCounter = 0;

    while (!libOpenCOR::fuzzyCompare(voi, VOI_END)) {
        EXPECT_TRUE(pSolver->solve(voi, std::min(VOI_START + static_cast<double>(++voiCounter) * VOI_INTERVAL, VOI_END)));

        computeVariables(voi, pStates, pRates, pVariables);
    }

    EXPECT_EQ_DOUBLES(pStates, pFinalStates);

    // Reinitialise the model (for coverage reasons).

    EXPECT_TRUE(pSolver->reinitialise(VOI_START));
}

void deleteArrays(double *pStates, double *pRates, double *pVariables)
{
    deleteArray(pStates);
    deleteArray(pRates);
    deleteArray(pVariables);
}

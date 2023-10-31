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

#pragma once

#include "libopencor/solverodefixedstep.h"

namespace libOpenCOR {

/**
 * @brief The SolverFourthOrderRungeKutta class.
 *
 * The SolverFourthOrderRungeKutta class is used to represent the fourth-order Runge-Kutta solver.
 */

class LIBOPENCOR_EXPORT SolverFourthOrderRungeKutta: public SolverOdeFixedStep
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverFourthOrderRungeKutta() override; /**< Destructor, @private. */

    SolverFourthOrderRungeKutta(const SolverFourthOrderRungeKutta &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverFourthOrderRungeKutta(SolverFourthOrderRungeKutta &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverFourthOrderRungeKutta &operator=(const SolverFourthOrderRungeKutta &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverFourthOrderRungeKutta &operator=(SolverFourthOrderRungeKutta &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SolverFourthOrderRungeKutta object.
     *
     * Factory method to create a @ref SolverFourthOrderRungeKutta object:
     *
     * ```
     * auto solver = libOpenCOR::SolverFourthOrderRungeKutta::create();
     * ```
     *
     * @return A smart pointer to a @ref SolverFourthOrderRungeKutta object.
     */

    static SolverFourthOrderRungeKuttaPtr create();

    std::string id() const override;
    std::string name() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;

    bool solve(double &pVoi, double pVoiEnd) const override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverFourthOrderRungeKutta(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

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

#include "solverodefixedstep.h"

namespace libOpenCOR {

/**
 * @brief The SolverSecondOrderRungeKutta class.
 *
 * The SolverSecondOrderRungeKutta class is used to represent the <a href="https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods">second-order Runge-Kutta</a> solver.
 */

class LIBOPENCOR_EXPORT SolverSecondOrderRungeKutta: public SolverOdeFixedStep
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverSecondOrderRungeKutta() override; /**< Destructor, @private. */

    SolverSecondOrderRungeKutta(const SolverSecondOrderRungeKutta &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverSecondOrderRungeKutta(SolverSecondOrderRungeKutta &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverSecondOrderRungeKutta &operator=(const SolverSecondOrderRungeKutta &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverSecondOrderRungeKutta &operator=(SolverSecondOrderRungeKutta &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SolverSecondOrderRungeKutta object.
     *
     * Factory method to create a @ref SolverSecondOrderRungeKutta object:
     *
     * ```
     * auto solver = libOpenCOR::SolverSecondOrderRungeKutta::create();
     * ```
     *
     * @return A smart pointer to a @ref SolverSecondOrderRungeKutta object.
     */

    static SolverSecondOrderRungeKuttaPtr create();

    std::string id() const override;
    std::string name() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;

    bool solve(double &pVoi, double pVoiEnd) const override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverSecondOrderRungeKutta(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

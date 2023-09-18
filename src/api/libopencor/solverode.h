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

#include "libopencor/solver.h"

namespace libOpenCOR {

/**
 * @brief The SolverOde class.
 *
 * The SolverOde class is used as an abstract class for ODE solvers.
 */

class LIBOPENCOR_EXPORT SolverOde: public Solver
{
public:
    using ComputeRates = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables);

    /**
     * Constructors, destructor, and assignment operators.
     */

    SolverOde() = delete; /**< No default constructor allowed, @private. */
    ~SolverOde() override = default; /**< Destructor, @private. */

    SolverOde(const SolverOde &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverOde(SolverOde &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverOde &operator=(const SolverOde &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverOde &operator=(SolverOde &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Initialise the solver.
     *
     * Initialise the solver, which means keeping track of the various arrays and of the compute rates method, as well
     * as checking its properties to initialise the solver itself.
     *
     * @param pStates The array of states.
     * @param pRates The array of rates.
     * @param pVariables The array of variables.
     * @param pComputeRates The compute rates method.
     *
     * @return @c true if the solver is initialised, @c false otherwise.
     */

    virtual bool initialise(double *pStates, double *pRates, double *pVariables, ComputeRates pComputeRates);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverOde(Impl *pPimpl); /**< Constructor, @private. */
};

} // namespace libOpenCOR

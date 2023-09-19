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
    /**
     * @brief Signature of the compute rates method.
     *
     * Signature of the method used to compute the rates of a system of ODEs.
     *
     * @param pVoi The variable of integration, as a @c double.
     * @param pStates The states, as an array of @c double.
     * @param pRates The rates, as an array of @c double.
     * @param pVariables The other variables, as an array of @c double.
     */

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
     * @brief Create a @ref SolverOde object.
     *
     * Factory method to create a @ref SolverOde object which name or KiSAO id is given:
     *
     * ```
     * auto odeSolver = libOpenCOR::SolverOde::create("CVODE");
     * auto anotherOdeSolver = libOpenCOR::SolverOde::create("KISAO:0000019");
     * ```
     *
     * @param pNameOrKisaoId The name of the ODE solver or its KiSAO id.
     *
     * @return A smart pointer to a @ref SolverOde object.
     */

    static SolverOdePtr create(const std::string &pNameOrKisaoId);

    /**
     * @brief Initialise the solver.
     *
     * Initialise the solver, which means keeping track of the various arrays and of the compute rates method, as well
     * as checking its properties to initialise the solver itself.
     *
     * @param pSize The size, as a @c size_t, of the system of ODEs.
     * @param pStates The states, as an array of @c double.
     * @param pRates The rates, as an array of @c double.
     * @param pVariables The other variables, as an array of @c double.
     * @param pComputeRates The compute rates method, as a @ref ComputeRates.
     *
     * @return @c true if the solver is initialised, @c false otherwise.
     */

    virtual bool initialise(size_t pSize, double *pStates, double *pRates, double *pVariables,
                            ComputeRates pComputeRates) = 0;

    /**
     * @brief Solve the system of ODEs.
     *
     * Solve the system of ODEs between @p pVoi and @p pVoiEnd.
     *
     * @param pVoi The variable of integration, as a @c double.
     * @param pVoiEnd The end value of the variable of integration, as a @c double.
     */

    virtual void solve(double &pVoi, double pVoiEnd) const = 0;

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverOde(Impl *pPimpl); /**< Constructor, @private. */
};

} // namespace libOpenCOR

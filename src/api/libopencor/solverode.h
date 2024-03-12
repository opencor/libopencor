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
 * The SolverOde class is used to represent an ODE solver.
 */

class LIBOPENCOR_EXPORT SolverOde: public Solver
{
    friend class SedSimulation;

public:
    using ComputeRates = void (*)(double pVoi, double *pStates, double *pRates, double *pVariables); /**< Signature of the method that computes the rates of the ODE system. */

    /**
     * Constructors, destructor, and assignment operators.
     */

    SolverOde() = delete; /**< No default constructor allowed, @private. */
    ~SolverOde() override = default; /**< Destructor, @private. */

    SolverOde(const SolverOde &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverOde(SolverOde &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverOde &operator=(const SolverOde &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverOde &operator=(SolverOde &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    Solver::Type type() const override;

    /**
     * @brief Initialise the solver.
     *
     * Initialise the solver.
     *
     * @param pVoi the value of the variable of integration.
     * @param pSize the size of the ODE system.
     * @param pStates the array of state values.
     * @param pRates the array of rate values.
     * @param pVariables the array of variable values.
     * @param pComputeRates the method that computes the rates of the ODE system.
     *
     * @return @c true if the solver could be initialised, @c false otherwise.
     */

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates);

    /**
     * @brief Reinitialise the solver.
     *
     * Reinitialise the solver.
     *
     * @param pVoi the value of the variable of integration.
     *
     * @return @c true if the solver could be reinitialised, @c false otherwise.
     */

    bool reinitialise(double pVoi);

    /**
     * @brief Solve the ODE system.
     *
     * Solve the ODE system.
     *
     * @param pVoi the value of the variable of integration.
     * @param pVoiEnd the end value of the variable of integration.
     *
     * @return @c true if the ODE system could be solved, @c false otherwise.
     */

    bool solve(double &pVoi, double pVoiEnd);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverOde(Impl *pPimpl); /**< Constructor, @private. */
};

} // namespace libOpenCOR

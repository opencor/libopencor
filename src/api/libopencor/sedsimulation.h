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

#include "libopencor/sedbase.h"

namespace libOpenCOR {

/**
 * @brief The SedSimulation class.
 *
 * The SedSimulation class is used to describe a simulation in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedSimulation: public SedBase
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedSimulation() override = default; /**< Destructor, @private. */

    SedSimulation(const SedSimulation &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedSimulation(SedSimulation &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedSimulation &operator=(const SedSimulation &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedSimulation &operator=(SedSimulation &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Set the ODE solver for this simulation.
     *
     * Set the ODE solver for this simulation.
     *
     * @param pOdeSolver The ODE solver, as a @ref SolverOdePtr, for this simulation.
     */

    void setOdeSolver(const SolverOdePtr &pOdeSolver);

    /**
     * @brief Get the ODE solver for this simulation.
     *
     * Return the ODE Solver for this simulation.
     *
     * @return The ODE solver for this simulation.
     */

    SolverOdePtr odeSolver() const;

    /**
     * @brief Set the NLA solver for this simulation.
     *
     * Set the NLA solver for this simulation.
     *
     * @param pNlaSolver The NLA solver, as a @ref SolverNlaPtr, for this simulation.
     */

    void setNlaSolver(const SolverNlaPtr &pNlaSolver);

    /**
     * @brief Get the NLA solver for this simulation.
     *
     * Return the NLA Solver for this simulation.
     *
     * @return The NLA solver for this simulation.
     */

    SolverNlaPtr nlaSolver() const;

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedSimulation(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

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
 * @brief The SolverNla class.
 *
 * The SolverNla class is used to represent an NLA solver.
 */

class LIBOPENCOR_EXPORT SolverNla: public Solver
{
    friend class SedInstanceTask;
    friend class SedmlFile;
    friend class SedSimulation;

public:
    using ComputeObjectiveFunction = void (*)(double *pU, double *pF, void *pUserData); /**< Signature of the method that computes the objective function of the NLA system. */

    /**
     * Constructors, destructor, and assignment operators.
     */

    SolverNla() = delete; /**< No default constructor allowed, @private. */

    SolverNla(const SolverNla &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverNla(SolverNla &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverNla &operator=(const SolverNla &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverNla &operator=(SolverNla &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    Solver::Type type() const override;

    /**
     * @brief Solve the NLA system.
     *
     * Solve the NLA system.
     *
     * @param pComputeObjectiveFunction The method that computes the objective function of the NLA system.
     * @param pU The initial guess for the solution of the NLA system.
     * @param pN The number of variables in the NLA system.
     * @param pUserData Some user data that may be passed to @p pComputeObjectiveFunction.
     *
     * @return @c true if the NLA system could be solved, @c false otherwise.
     */

#ifdef __EMSCRIPTEN__
    bool solve(size_t pComputeObjectiveFunctionIndex, double *pU, size_t pN, void *pUserData);
#else
    bool solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData);
#endif

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SolverNla(Impl *pPimpl); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

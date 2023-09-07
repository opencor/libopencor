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

#include "libopencor/logger.h"

namespace libOpenCOR {

/**
 * @brief The Solver class.
 *
 * The Solver class is used to access various information about available solvers, as well as to create a solver.
 */

class LIBOPENCOR_EXPORT Solver: public Logger
{
public:
    /**
     * @brief The type of the @ref Solver.
     *
     * The type of the solver, i.e. whether it is an ODE or an NLA solver.
     */

    enum class Type
    {
        ODE, /**< The solver is for ODE systems. */
        NLA /**< The solver is for NLA systems. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    Solver() = delete; /**< No default constructor allowed, @private. */
    ~Solver() override = default; /**< Destructor, @private. */

    Solver(const Solver &pOther) = delete; /**< No copy constructor allowed, @private. */
    Solver(Solver &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    Solver &operator=(const Solver &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Solver &operator=(Solver &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref Solver object.
     *
     * Factory method to create a @ref Solver object which name or KiSAO id is given:
     *
     * ```
     * auto cvodeSolver = libOpenCOR::Solver::create("CVODE");
     * auto anotherCvodeSolver = libOpenCOR::Solver::create("KISAO:0000019");
     * ```
     *
     * @param pNameOrKisaoId The name of the solver or its KiSAO id.
     *
     * @return A smart pointer to a @ref Solver object.
     */

    static SolverPtr create(const std::string &pNameOrKisaoId);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit Solver(Impl *pPimpl); /**< Constructor @private. */
};

} // namespace libOpenCOR

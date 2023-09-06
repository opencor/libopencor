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
     * @brief The type of a Solver.
     *
     * The type of a solver, i.e. whether it is an ODE or an NLA solver.
     */

    enum class Type
    {
        ODE, /**< The solver is for ODE systems. */
        NLA /**< The solver is for NLA systems. */
    };

    /**
     * @brief The method used by a Solver.
     *
     * The method used by a solver, i.e. whether it uses the CVODE, forward Euler, fourth-order Runge-Kutta, Heun,
     * KINSOL, or second-order Runge-Kutta method.
     */

    enum class Method
    {
        CVODE, /**< The solver uses the CVODE method. */
        FORWARD_EULER, /**< The solver uses the forward Euler method. */
        FOURTH_ORDER_RUNGE_KUTTA, /**< The solver uses the fourth-order Runge-Kutta method. */
        HEUN, /**< The solver uses the Heun method. */
        KINSOL, /**< The solver uses the KINSOL method. */
        SECOND_ORDER_RUNGE_KUTTA /**< The solver uses the second-order Runge-Kutta method. */
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
     * Factory method to create a @ref Solver object that uses the given @ref Method "":
     *
     * ```
     * auto cvodeSolver = libOpenCOR::Solver::create(libOpenCOR::Solver::CVODE);
     * ```
     *
     * @param pMethod The @ref Method used by the solver.
     *
     * @return A smart pointer to a @ref Solver object.
     */

    static SolverPtr create(Method pMethod);

    /**
     * @brief Get the type of this solver.
     *
     * Return the type of this solver.
     *
     * @return The type, as a @ref Type, of this solver.
     */

    Type type() const;

    /**
     * @brief Get the name of this solver.
     *
     * Return the name of this solver.
     *
     * @return The name, as a @c std::string, of this solver.
     */

    std::string name() const;

    /**
     * @brief Get the properties of this solver.
     *
     * Return the properties of this solver.
     *
     * @return The properties, as a @c std::vector of a @c std::string, of this solver.
     */

    std::vector<std::string> properties() const;

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit Solver(Impl *pPimpl); /**< Constructor @private. */
};

} // namespace libOpenCOR

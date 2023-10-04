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

#include <map>
#include <string>
#include <vector>

namespace libOpenCOR {

/**
 * @brief The Solver class.
 *
 * The Solver class is used to get various information about the solvers that are available, as well as to @ref create a
 * solver.
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
     * Factory method to create a @ref Solver object which (KiSAO) id or name is given:
     *
     * ```
     * auto anotherCvodeSolver = libOpenCOR::Solver::create("KISAO:0000019");
     * auto cvodeSolver = libOpenCOR::Solver::create("CVODE");
     * ```
     *
     * The following <a href="https://en.wikipedia.org/wiki/Ordinary_differential_equation">ODE</a> solvers can be created:
     *  - <a href="http://identifiers.org/biomodels.kisao:KISAO_0000019">`KISAO:0000019`</a> | <a href="https://computing.llnl.gov/projects/sundials/cvode">`CVODE`</a>;
     *  - <a href="http://identifiers.org/biomodels.kisao:KISAO_0000030">`KISAO:0000030`</a> | <a href="https://en.wikipedia.org/wiki/Euler_method">`Forward Euler`</a>;
     *  - <a href="http://identifiers.org/biomodels.kisao:KISAO_0000032">`KISAO:0000032`</a> | <a href="https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods">`Fourth-order Runge-Kutta`</a>;
     *  - <a href="http://identifiers.org/biomodels.kisao:KISAO_0000301">`KISAO:0000301`</a> | <a href="https://en.wikipedia.org/wiki/Heun's_method">`Heun`</a>; and
     *  - <a href="http://identifiers.org/biomodels.kisao:KISAO_0000381">`KISAO:0000381`</a> | <a href="https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods">`Second-order Runge-Kutta`</a>.
     *
     * @param pIdOrName The (KiSAO) id or name, as a @c std::string, of the solver.
     *
     * @return A smart pointer to a @ref Solver object.
     */

    static SolverPtr create(const std::string &pIdOrName);

    /**
     * @brief Get various information about the solvers that are available.
     *
     * Get various information about the solvers that are available.
     *
     * @return The information, as a @c std::vector of @ref SolverInfo, about the solvers that are available.
     */

    static std::vector<SolverInfoPtr> solversInfo();

    /**
     * @brief Return whether this solver is valid.
     *
     * Return whether this solver is valid.
     *
     * @return @c true if this solver is valid, @c false otherwise.
     */

    bool isValid() const;

    /**
     * @brief Get some information about this solver.
     *
     * Return some information about this solver.
     *
     * @return Some information, as a @ref SolverInfo, about this solver.
     */

    virtual SolverInfoPtr info() const = 0;

    /**
     * @brief Get the value of a property of this solver.
     *
     * Get the value of a property, which (KiSAO) id or name is given, of this solver.
     *
     * @param pIdOrName The (KiSAO) id or name, as a @c std::string, of a property.
     *
     * @return The value, as a @c std::string, of the property of this solver.
     */

    std::string property(const std::string &pIdOrName);

    /**
     * @brief Set the value of a property of this solver.
     *
     * Set the value of a property, which (KiSAO) ID or name is given, of this solver. If the name or KiSAO ID is not
     * supported by this solver then nothing is done.
     *
     * @param pIdOrName The (KiSAO) id or name, as a @c std::string, of a property of this solver.
     * @param pValue The value, as a @c std::string, of a property of this solver.
     */

    void setProperty(const std::string &pIdOrName, const std::string &pValue);

    /**
     * @brief Get the properties of this solver.
     *
     * Get the properties of this solver.
     *
     * @return The properties, as a @c std::map of @c std::string, of this solver.
     */

    std::map<std::string, std::string> properties() const;

    /**
     * @brief Set the properties of this solver.
     *
     * Set the properties of this solver. If the (KiSAO) ID or name of a property is not supported by this solver then
     * nothing is done. If @p pProperties has an entry for both the (KiSAO) ID and name of a property then the second
     * entry is used.
     *
     * @param pProperties The properties, as a @c std::map of @c std::string, of this solver.
     */

    void setProperties(const std::map<std::string, std::string> &pProperties);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit Solver(Impl *pPimpl); /**< Constructor, @private. */
};

} // namespace libOpenCOR

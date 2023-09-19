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
 * The Solver class is used to get various information about the solvers that are available, as well as to create a
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
     * @brief Create a @ref SolverOde object.
     *
     * Factory method to create a @ref SolverOde object which name or KiSAO id is given:
     *
     * ```
     * auto cvodeSolver = libOpenCOR::Solver::createOde("CVODE");
     * auto anotherCvodeSolver = libOpenCOR::Solver::createOde("KISAO:0000019");
     * ```
     *
     * @param pNameOrKisaoId The name of the ODE solver or its KiSAO id.
     *
     * @return A smart pointer to a @ref SolverOde object.
     */

    static SolverOdePtr createOde(const std::string &pNameOrKisaoId);

    /**
     * @brief Get various information about the solvers that are available.
     *
     * Get various information about the solvers that are available.
     *
     * @return The information, as a @c std::vector of @ref SolverInfo, about the solvers that are available.
     */

    static std::vector<SolverInfoPtr> solversInfo();

    /**
     * @brief Return whether the solver is valid.
     *
     * Return whether the solver is valid.
     *
     * @return @c true if the solver is valid, @c false otherwise.
     */

    bool isValid() const;

    /**
     * @brief Get the value of a property of this solver.
     *
     * Get the value of a property, which name or KiSAO id is given, of this solver.
     *
     * @param pNameOrKisaoId The name or KiSAO id, as a @c std::string, of a property.
     *
     * @return The value, as a @c std::string, of the property.
     */

    std::string property(const std::string &pNameOrKisaoId);

    /**
     * @brief Set the value of a property of this solver.
     *
     * Set the value of a property, which name or KiSAO ID is given, of this solver. If the name or KiSAO ID is not
     * supported by the solver then nothing is done.
     *
     * @param pNameOrKisaoId The name or KiSAO id, as a @c std::string, of a property.
     * @param pValue The value, as a @c std::string, of a property.
     */

    void setProperty(const std::string &pNameOrKisaoId, const std::string &pValue);

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
     * Set the properties of this solver. If the name or KiSAO ID of a property is not supported by the solver then
     * nothing is done for that property.
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

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

#include <vector>

namespace libOpenCOR {

/**
 * @brief The SolverInfo class.
 *
 * The SolverInfo class is used to access various information about a solver.
 */

class LIBOPENCOR_EXPORT SolverInfo
{
    friend class Solver;

public:
    using HiddenPropertiesFunction = std::vector<std::string> (*)(const std::map<std::string, std::string> &pProperties); /**< Signature of the method that returns the properties of the solver that should be hidden. */

    /**
     * Constructors, destructor, and assignment operators.
     */

#ifdef COVERAGE_ENABLED
    ~SolverInfo() = default; /**< Destructor, @private. */
#else
    ~SolverInfo(); /**< Destructor, @private. */
#endif

    SolverInfo(const SolverInfo &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverInfo(SolverInfo &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverInfo &operator=(const SolverInfo &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverInfo &operator=(SolverInfo &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the type of the solver.
     *
     * Return the type of the solver.
     *
     * @return The type, as a @ref Solver::Type, of the solver.
     */

    Solver::Type type() const;

    /**
     * @brief Get the (KiSAO) id of the solver.
     *
     * Return the (KiSAO) id of the solver.
     *
     * @return The (KiSAO) id, as a @c std::string, of the solver.
     */

    std::string id() const;

    /**
     * @brief Get the name of the solver.
     *
     * Return the name of the solver.
     *
     * @return The name, as a @c std::string, of the solver.
     */

    std::string name() const;

    /**
     * @brief Get the properties of the solver.
     *
     * Return the properties of the solver.
     *
     * @return The properties, as a @c std::vector of @ref SolverProperty, of the solver.
     */

    std::vector<SolverPropertyPtr> properties() const;

    /**
     * @brief Get the properties of the solver that should be hidden.
     *
     * Get, for the given properties, the properties of the solver that should be hidden. The key of an entry in
     * @p pProperties can be either a (KiSAO) id or a name. If @p pProperties has an entry for both the (KiSAO) id and
     * name of a property then the (KiSAO) id entry is used.
     *
     * @param pProperties The properties, as a @c std::map of @c std::string, against which to determine the properties
     * of the solver that should be hidden.
     *
     * @return The properties, as a @c std::vector of @c std::string, of the solver that should be hidden.
     */

    std::vector<std::string> hiddenProperties(const std::map<std::string, std::string> &pProperties) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *mPimpl;

    explicit SolverInfo(Solver::Type pType, const std::string &pId, const std::string &pName,
                        const std::vector<SolverPropertyPtr> &pProperties,
                        HiddenPropertiesFunction pHiddenProperties); /**< Constructor, @private. */
};

} // namespace libOpenCOR

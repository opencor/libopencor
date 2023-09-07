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
 * @brief The SolverInfo class.
 *
 * The SolverInfo class is used to access various information about a solver.
 */

class LIBOPENCOR_EXPORT SolverInfo
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverInfo(); /**< Destructor, @private. */

    SolverInfo(const SolverInfo &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverInfo(SolverInfo &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverInfo &operator=(const SolverInfo &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverInfo &operator=(SolverInfo &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the type of the solver.
     *
     * Return the type of the solver.
     *
     * @return The type, as a @ref Type, of the solver.
     */

    Solver::Type type() const;

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

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *mPimpl;

    explicit SolverInfo(Solver::Type pType, const std::string &pName,
                        const std::vector<SolverPropertyPtr> &pProperties); /**< Constructor @private. */
};

} // namespace libOpenCOR

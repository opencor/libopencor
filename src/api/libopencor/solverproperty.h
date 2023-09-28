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

#include "libopencor/export.h"

namespace libOpenCOR {

/**
 * @brief The SolverInfo class.
 *
 * The SolverInfo class is used to access various information about a solver.
 */

class LIBOPENCOR_EXPORT SolverProperty
{
    friend class Solver;

public:
    /**
     * @brief The type of the @ref SolverProperty.
     *
     * The type of the solver property, i.e. whether it is a boolean, an integer, an integer greater than zero, an
     * integer greater or equal to zero, a double, a double greater than zero, a double greater or equal to zero, or a
     * list.
     */

    enum class Type
    {
        Boolean, /**< The solver property is a boolean. */
        Integer, /**< The solver property is an integer. */
        IntegerGt0, /**< The solver property is an integer greater than zero. */
        IntegerGe0, /**< The solver property is an integer greater or equal to zero. */
        Double, /**< The solver property is a double. */
        DoubleGt0, /**< The solver property is a double greater than zero. */
        DoubleGe0, /**< The solver property is a double greater or equal to zero. */
        List /**< The solver property is a list. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    SolverProperty() = delete; /**< Constructor, @private. */
#ifdef COVERAGE_ENABLED
    ~SolverProperty() = default; /**< Destructor, @private. */
#else
    ~SolverProperty(); /**< Destructor, @private. */
#endif

    SolverProperty(const SolverProperty &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverProperty(SolverProperty &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverProperty &operator=(const SolverProperty &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverProperty &operator=(SolverProperty &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the type of this property.
     *
     * Return the type of this property.
     *
     * @return The type, as a @ref Type, of this property.
     */

    Type type() const;

    /**
     * @brief Get the (KiSAO) id of this property.
     *
     * Return the (KiSAO) id of this property.
     *
     * @return The (KiSAO) id, as a @c std::string, of this property.
     */

    std::string id() const;

    /**
     * @brief Get the name of this property.
     *
     * Return the name of this property.
     *
     * @return The name, as a @c std::string, of this property.
     */

    std::string name() const;

    /**
     * @brief Get the list of values this property can take.
     *
     * Return the name of the list of values this property can take.
     *
     * @return The list of values, as a @c std::vector of @c std::string, this property can take.
     */

    std::vector<std::string> listValues() const;

    /**
     * @brief Get the default value of this property.
     *
     * Return the default value of this property.
     *
     * @return The default value, as a @c std::string, of this property.
     */

    std::string defaultValue() const;

    /**
     * @brief Get whether this property has the same unit as the variable of integration.
     *
     * Return whether this property has the same unit as the variable of integration.
     *
     * @return Whether, as a @c bool, this property has the same unit as the variable of integration.
     */

    bool hasVoiUnit() const;

private:
    class Impl;

    Impl *mPimpl;

    explicit SolverProperty(Type pType, const std::string &pId, const std::string &pName,
                            const std::vector<std::string> &pListValues, const std::string &pDefaultValue,
                            bool pHasVoiUnit);
};

} // namespace libOpenCOR

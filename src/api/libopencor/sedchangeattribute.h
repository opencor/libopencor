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

#include "libopencor/sedchange.h"

namespace libOpenCOR {

/**
 * @brief The SedChangeAttribute class.
 *
 * The SedChangeAttribute class is used to describe a change attribute in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedChangeAttribute: public SedChange
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedChangeAttribute() override; /**< Destructor, @private. */

    SedChangeAttribute(const SedChangeAttribute &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedChangeAttribute(SedChangeAttribute &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedChangeAttribute &operator=(const SedChangeAttribute &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedChangeAttribute &operator=(SedChangeAttribute &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedChangeAttribute object.
     *
     * Factory method to create a @ref SedChangeAttribute object:
     *
     * ```
     * auto simulation = libOpenCOR::SedChangeAttribute::create(component, variable, newValue);
     * ```
     *
     * @param pComponent The component, as a @c std::string, where the target is located.
     * @param pVariable The variable, as a @c std::string, corresponding to the target.
     * @param pNewValue The new value, as a @c std::string, for the target.
     *
     * @return A smart pointer to a @ref SedChangeAttribute object.
     */

    static SedChangeAttributePtr create(const std::string &pComponent, const std::string &pVariable,
                                        const std::string &pNewValue);

    /**
     * @brief Get the component.
     *
     * Get the component.
     *
     * @return The component as a @c std::string.
     */

    std::string component() const;

    /**
     * @brief Set the component.
     *
     * Set the component.
     *
     * @param pComponent The component as a @c std::string.
     */

    void setComponent(const std::string &pComponent);

    /**
     * @brief Get the variable.
     *
     * Get the variable.
     *
     * @return The variable as a @c std::string.
     */

    std::string variable() const;

    /**
     * @brief Set the variable.
     *
     * Set the variable.
     *
     * @param pVariable The variable as a @c std::string.
     */

    void setVariable(const std::string &pVariable);

    /**
     * @brief Get the new value.
     *
     * Get the new value.
     *
     * @return The new value as a @c std::string.
     */

    std::string newValue() const;

    /**
     * @brief Set the new value.
     *
     * Set the new value.
     *
     * @param pNewValue The new value as a @c std::string.
     */

    void setNewValue(const std::string &pNewValue);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedChangeAttribute(const std::string &pComponent, const std::string &pVariable,
                                const std::string &pNewValue); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

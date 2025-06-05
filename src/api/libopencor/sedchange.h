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
 * @brief The SedChange class.
 *
 * The SedChange class is used to describe a change in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedChange: public SedBase
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedChange() override = default; /**< Destructor, @private. */

    SedChange(const SedChange &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedChange(SedChange &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedChange &operator=(const SedChange &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedChange &operator=(SedChange &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the target.
     *
     * Get the target.
     *
     * @return The target as a @c std::string.
     */

    std::string target() const;

    /**
     * @brief Set the target.
     *
     * Set the target.
     *
     * @param pTarget The target as a @c std::string.
     */

    void setTarget(const std::string &pTarget);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedChange(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

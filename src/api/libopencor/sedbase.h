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
 * @brief The SedBase class.
 *
 * The SedBase class is the base class for all Sed* classes.
 */

class LIBOPENCOR_EXPORT SedBase: public Logger
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    SedBase() = delete; /**< No default constructor allowed, @private. */

    SedBase(const SedBase &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedBase(SedBase &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedBase &operator=(const SedBase &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedBase &operator=(SedBase &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the id.
     *
     * Get the id.
     *
     * @return The id as a @c std::string.
     */

    std::string id() const;

    /**
     * @brief Set the id.
     *
     * Set the id.
     *
     * @param pId The id as a @c std::string.
     */

    void setId(const std::string &pId);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedBase(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

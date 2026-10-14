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

#include <memory>

namespace libOpenCOR {

/**
 * @brief The SedBase class.
 *
 * The SedBase class is the base class for all Sed* classes.
 */

class LIBOPENCOR_EXPORT SedBase: public Logger
{
    friend class SedDocument;

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
     * @brief Return the id.
     *
     * Return the id.
     *
     * @return The id as a @c std::string.
     */

    const std::string &id() const;

    /**
     * @brief Return whether the given id is a valid SED-ML identifier.
     *
     * Return whether the given id is a valid SED-ML identifier, i.e. it starts with a letter or an underscore, followed
     * by any number of letters, digits, and underscores.
     *
     * @param pId The id as a @c std::string.
     *
     * @return @c true if the given id is a valid SED-ML identifier, @c false otherwise.
     */

    static bool isValidId(const std::string &pId);

    /**
     * @brief Return whether the given id is a duplicate.
     *
     * Return whether the given id is already used by another object of the simulation experiment description for which
     * this object was created. The id of this object is not a duplicate unless this object was removed from its
     * simulation experiment description and another object was then given its id.
     *
     * @param pId The id as a @c std::string.
     *
     * @return @c true if the given id is a duplicate, @c false otherwise.
     */

    bool isDuplicateId(const std::string &pId) const;

    /**
     * @brief Set the id.
     *
     * Set the id, if it is a valid SED-ML identifier (see @ref isValidId) and if it is not a duplicate (see
     * @ref isDuplicateId).
     *
     * @param pId The id as a @c std::string.
     *
     * @return @c true if the id was set, @c false otherwise (in which case the id is left unchanged).
     */

    bool setId(const std::string &pId);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedBase(std::unique_ptr<Impl> pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

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

#include <string>

namespace libOpenCOR {

/**
 * @brief The Issue class.
 *
 * The Issue class is used to represent an error, a warning, or a message that has been generated as a result of using
 * libOpenCOR.
 */

class LIBOPENCOR_EXPORT Issue
{
    friend class Logger;

public:
    /**
     * @brief The type of an issue.
     *
     * The type of an issue, i.e. whether it is an error, a warning, or a message.
     */

    enum class Type
    {
        ERROR, /**< The issue is an error. */
        WARNING, /**< The issue is a warning. */
        MESSAGE /**< The issue is a message. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    Issue() = delete; /**< No default constructor allowed, @private. */
    ~Issue(); /**< Destructor, @private. */

    Issue(const Issue &pOther) = delete; /**< No copy constructor allowed, @private. */
    Issue(Issue &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    Issue &operator=(const Issue &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Issue &operator=(Issue &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the type of this issue.
     *
     * Return the type of this issue.
     *
     * @return The @ref Type of this issue.
     */

    Type type() const;

    /**
     * @brief Get the description of this issue.
     *
     * Return the @c std::string description of this issue.
     *
     * @return The @c std::string description of this issue.
     */

    std::string description() const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *mPimpl;

    explicit Issue(const std::string &pDescription, Type pType); /**< Constructor, @private. */
};

} // namespace libOpenCOR

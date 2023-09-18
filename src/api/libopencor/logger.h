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
#include "libopencor/types.h"

#include <vector>

namespace libOpenCOR {

/**
 * @brief The Logger class.
 *
 * The Logger class is used to log issues that have been generated as a result of using libOpenCOR.
 */

class LIBOPENCOR_EXPORT Logger
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    Logger() = delete; /**< No default constructor allowed, @private. */
    virtual ~Logger() = default; /**< Destructor, @private. */

    Logger(const Logger &pOther) = delete; /**< No copy constructor allowed, @private. */
    Logger(Logger &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    Logger &operator=(const Logger &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Logger &operator=(Logger &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Return the issues.
     *
     * Return the issues.
     *
     * @return The issues as a @c std::vector of references to @ref Issue objects.
     */

    std::vector<IssuePtr> issues() const;

    /**
     * @brief Return the errors.
     *
     * Return the errors.
     *
     * @return The errors as a @c std::vector of references to @ref Issue objects of type @ref Issue::Type::ERROR.
     */

    std::vector<IssuePtr> errors() const;

    /**
     * @brief Return the warnings.
     *
     * Return the warnings.
     *
     * @return The warnings as a @c std::vector of references to @ref Issue objects of type @ref Issue::Type::WARNING.
     */

    std::vector<IssuePtr> warnings() const;

    /**
     * @brief Return the messages.
     *
     * Return the messages.
     *
     * @return The messages as a @c std::vector of references to @ref Issue objects of type @ref Issue::Type::MESSAGE.
     */

    std::vector<IssuePtr> messages() const;

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit Logger(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

private:
    Impl *mPimpl;
};

} // namespace libOpenCOR

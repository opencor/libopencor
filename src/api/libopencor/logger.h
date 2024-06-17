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
     * @brief Return whether there are some issues.
     *
     * Return whether there are some issues.
     *
     * @return @c true if there are some issues, @c false otherwise.
     */

    bool hasIssues() const;

    /**
     * @brief Return the issues.
     *
     * Return the issues.
     *
     * @return The issues, as an @ref IssuePtrs.
     */

    IssuePtrs issues() const;

    /**
     * @brief Return whether there are some errors.
     *
     * Return whether there are some errors.
     *
     * @return @c true if there are some errors, @c false otherwise.
     */

    bool hasErrors() const;

    /**
     * @brief Return the errors.
     *
     * Return the errors.
     *
     * @return The errors, as a @ref IssuePtrs of type @ref Issue::Type::ERROR.
     */

    IssuePtrs errors() const;

    /**
     * @brief Return whether there are some warnings.
     *
     * Return whether there are some warnings.
     *
     * @return @c true if there are some warnings, @c false otherwise.
     */

    bool hasWarnings() const;

    /**
     * @brief Return the warnings.
     *
     * Return the warnings.
     *
     * @return The warnings, as a @ref IssuePtrs of type @ref Issue::Type::WARNING.
     */

    IssuePtrs warnings() const;

    /**
     * @brief Return whether there are some messages.
     *
     * Return whether there are some messages.
     *
     * @return @c true if there are some messages, @c false otherwise.
     */

    bool hasMessages() const;

    /**
     * @brief Return the messages.
     *
     * Return the messages.
     *
     * @return The messages, as a @ref IssuePtrs of type @ref Issue::Type::MESSAGE.
     */

    IssuePtrs messages() const;

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit Logger(Impl *pPimpl); /**< Constructor, @private. */

private:
    Impl *mPimpl; /**< The private implementation, @private. */
};

} // namespace libOpenCOR

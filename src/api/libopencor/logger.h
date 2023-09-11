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
     * @brief Return whether there are issues.
     *
     * Return whether there are issues.
     *
     * @return @c true if there are issues, @c false otherwise.
     */

    bool hasIssues() const;

    /**
     * @brief Get the number of issues.
     *
     * Return the number of issues.
     *
     * @return The number of issues.
     */

    size_t issueCount() const;

    /**
     * @brief Get the issue at the given @p pIndex.
     *
     * Return the issue at the given @p pIndex. If @p pIndex is not valid then @c nullptr is returned. The valid range
     * for @p pIndex is [0, \#issues).
     *
     * @param pIndex The index of the issue to return.
     *
     * @return A reference to the issue at the given @p pIndex or @c nullptr if @p pIndex is not valid.
     */

    IssuePtr issue(size_t pIndex) const;

    /**
     * @brief Return whether there are errors.
     *
     * Return whether there are errors.
     *
     * @return @c true if there are errors, @c false otherwise.
     */

    bool hasErrors() const;

    /**
     * @brief Get the number of errors.
     *
     * Return the number of issues of error type.
     *
     * @return The number of errors.
     */

    size_t errorCount() const;

    /**
     * @brief Get the error at the given @p pIndex.
     *
     * Return the issue, of error type, at the given @p pIndex. If @p pIndex is not valid then @c nullptr is returned.
     * The valid range for @p pIndex is [0, \#errors).
     *
     * @param pIndex The index of the error to return.
     *
     * @return A reference to the error at the given @p pIndex or @c nullptr if @p pIndex is not valid.
     */

    IssuePtr error(size_t pIndex) const;

    /**
     * @brief Return whether there are warnings.
     *
     * Return whether there are warnings.
     *
     * @return @c true if there are warnings, @c false otherwise.
     */

    /*---GRY---
        bool hasWarnings() const;
    */

    /**
     * @brief Get the number of warnings.
     *
     * Return the number of issues of warning type.
     *
     * @return The number of warnings.
     */

    /*---GRY---
        size_t warningCount() const;
    */

    /**
     * @brief Get the warning at the given @p pIndex.
     *
     * Return the issue, of warning type, at the given @p pIndex. If @p pIndex is not valid then @c nullptr is returned.
     * The valid range for @p pIndex is [0, \#warnings).
     *
     * @param pIndex The index of the warning to return.
     *
     * @return A reference to the warning at the given @p pIndex or @c nullptr if @p pIndex is not valid.
     */

    /*---GRY---
        IssuePtr warning(size_t pIndex) const;
    */

    /**
     * @brief Return whether there are messages.
     *
     * Return whether there are messages.
     *
     * @return @c true if there are messages, @c false otherwise.
     */

    /*---GRY---
        bool hasMessages() const;
    */

    /**
     * @brief Get the number of messages.
     *
     * Return the number of issues of message type.
     *
     * @return The number of messages.
     */

    /*---GRY---
        size_t messageCount() const;
    */

    /**
     * @brief Get the message at the given @p pIndex.
     *
     * Return the issue, of message type, at the given @p pIndex. If @p pIndex is not valid then @c nullptr is returned.
     * The valid range for @p pIndex is [0, \#messages).
     *
     * @param pIndex The index of the message to return.
     *
     * @return A reference to the message at the given @p pIndex or @c nullptr if @p pIndex is not valid.
     */

    /*---GRY---
        IssuePtr message(size_t pIndex) const;
    */

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit Logger(Impl *pPimpl); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

private:
    Impl *mPimpl;
};

} // namespace libOpenCOR

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

#include <stddef.h>

namespace libOpenCOR {

/**
 * @brief The Logger class.
 *
 * The Logger class is a base class for all the classes that need to log some
 * issues.
 */

class LIBOPENCOR_EXPORT Logger
{
public:
    /**
     * Constructors, destructor and assignment operators.
     */

    ~Logger() = default; /**< Default destructor, @private. */

    Logger(const Logger &pOther) = delete; /**< No copy constructor allowed, @private. */
    Logger(Logger &&) noexcept = delete; /**< No move constructor allowed, @private. */

    Logger &operator=(const Logger &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Logger &operator=(Logger &&) noexcept = delete; /**< No move assignment operator allowed, @private. */

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
     * Return the issue at the given @p pIndex. If the @p pIndex is not valid
     * then a @c nullptr is returned. The valid range for @p pIndex is
     * [0, \#issues).
     *
     * @param pIndex The index of the issue to return.
     *
     * @return A reference to the issue at the given index on success,
     * @c nullptr otherwise.
     */

    IssuePtr issue(size_t pIndex) const;

protected:
    struct Impl; /**< Forward declaration of the implementation class, @private. */

    explicit Logger(Impl *pPimpl); /**< Constructor, @private. */

    Impl *pimpl(); /**< Implementation getter, @private. */
    const Impl *pimpl() const; /**< Constant implementation getter, @private. */

private:
    Impl *mPimpl;
};

} // namespace libOpenCOR

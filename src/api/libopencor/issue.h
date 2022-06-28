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

#include <memory>

namespace libOpenCOR {

/**
 * @brief The Issue class.
 *
 * The Issue class is for representing an issue with a particular aspect of
 * libOpenCOR.
 */

class LIBOPENCOR_EXPORT Issue
{
public:
    /**
     * @brief The type of issue.
     *
     * An issue can be of one of the following types:
     *  - MESSAGE;
     *  - WARNING; or
     *  - ERROR.
     */

    enum class Type
    {
        MESSAGE,
        WARNING,
        ERROR
    };

    /**
     * Constructors, destructor and assignment operators.
     */

    Issue() = delete; /**< No default constructor allowed, @private. */
    ~Issue() = default; /**< Default destructor, @private. */

    Issue(const Issue &pOther) = delete; /**< No copy constructor allowed, @private. */
    Issue(Issue &&) noexcept = delete; /**< No move constructor allowed, @private. */

    Issue &operator=(const Issue &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Issue &operator=(Issue &&) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the type of this @c Issue.
     *
     * Return the type of this @c Issue.
     *
     * @return The type as a @c Issue.
     */

    Type type() const;

private:
    explicit Issue(Type pType); /**< Constructor @private*/

    struct Impl; /**< Forward declaration of the implementation class, @private. */

    std::unique_ptr<Impl> mPimpl;
};

} // namespace libOpenCOR

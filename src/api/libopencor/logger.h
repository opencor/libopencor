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
    ~Logger() = default; /**< Destructor, @private. */

    Logger(const Logger &pOther) = delete; /**< No copy constructor allowed, @private. */
    Logger(Logger &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    Logger &operator=(const Logger &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Logger &operator=(Logger &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit Logger(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

private:
    Impl *mPimpl;
};

} // namespace libOpenCOR

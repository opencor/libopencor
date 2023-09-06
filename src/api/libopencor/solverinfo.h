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
 * @brief The SolverInfo class.
 *
 * The SolverInfo class is used to access various information about a solver.
 */

class LIBOPENCOR_EXPORT SolverInfo
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverInfo(); /**< Destructor, @private. */

    SolverInfo(const SolverInfo &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverInfo(SolverInfo &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverInfo &operator=(const SolverInfo &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverInfo &operator=(SolverInfo &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *mPimpl;

    explicit SolverInfo(); /**< Constructor @private. */
};

} // namespace libOpenCOR

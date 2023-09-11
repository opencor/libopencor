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

#include "libopencor/solver.h"

namespace libOpenCOR {

/**
 * @brief The SolverUnknown class.
 *
 * The SolverUnknown class is used for a solver that has been requested but is unknown.
 */

class LIBOPENCOR_EXPORT SolverUnknown: public Solver
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverUnknown() override; /**< Destructor, @private. */

    SolverUnknown(const SolverUnknown &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverUnknown(SolverUnknown &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverUnknown &operator=(const SolverUnknown &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverUnknown &operator=(SolverUnknown &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SolverUnknown(); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

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

#include "solverode.h"

namespace libOpenCOR {

/**
 * @brief The SolverForwardEuler class.
 *
 * The SolverForwardEuler class implements the Foward Euler method, a first-order numerical technique for solving a
 * system of ODEs.
 */

class LIBOPENCOR_EXPORT SolverForwardEuler: public SolverOde
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverForwardEuler() override; /**< Destructor, @private. */

    SolverForwardEuler(const SolverForwardEuler &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverForwardEuler(SolverForwardEuler &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverForwardEuler &operator=(const SolverForwardEuler &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverForwardEuler &operator=(SolverForwardEuler &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverForwardEuler(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

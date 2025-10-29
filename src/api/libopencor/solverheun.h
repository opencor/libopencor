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

#include "libopencor/solverodefixedstep.h"

namespace libOpenCOR {

/**
 * @brief The SolverHeun class.
 *
 * The SolverHeun class is used to represent the <a href="https://en.wikipedia.org/wiki/Heun's_method">Heun</a> solver.
 */

class LIBOPENCOR_EXPORT SolverHeun: public SolverOdeFixedStep
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverHeun() override; /**< Destructor, @private. */

    SolverHeun(const SolverHeun &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverHeun(SolverHeun &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverHeun &operator=(const SolverHeun &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverHeun &operator=(SolverHeun &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SolverHeun object.
     *
     * Factory method to create a @ref SolverHeun object:
     *
     * ```
     * auto solver {libOpenCOR::SolverHeun::create()};
     * ```
     *
     * @return A smart pointer to a @ref SolverHeun object.
     */

    static SolverHeunPtr create();

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SolverHeun(); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

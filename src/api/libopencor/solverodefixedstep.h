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

#include "libopencor/solverode.h"

namespace libOpenCOR {

/**
 * @brief The SolverCvode class.
 *
 * The SolverCvode class is used to represent the forward Euler solver.
 */

class LIBOPENCOR_EXPORT SolverOdeFixedStep: public SolverOde
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    SolverOdeFixedStep(const SolverOdeFixedStep &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverOdeFixedStep(SolverOdeFixedStep &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverOdeFixedStep &operator=(const SolverOdeFixedStep &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverOdeFixedStep &operator=(SolverOdeFixedStep &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the step used by the solver.
     *
     * Return the step used by the solver.
     *
     * @return The step used by the solver.
     */

    double step() const;

    /**
     * @brief Set the step to be used by the solver.
     *
     * Set the step to be used by the solver.
     *
     * @param pStep The step to be used by the solver.
     */

    void setStep(double pStep);

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverOdeFixedStep(Impl *pPimpl); /**< Constructor, @private. */
};

} // namespace libOpenCOR

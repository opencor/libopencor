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
 * @brief The SolverHeun class.
 *
 * The SolverHeun class is used to represent the Heun solver.
 */

class SolverHeun: public SolverOde
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverHeun() override; /**< Destructor, @private. */

    SolverHeun(const SolverHeun &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverHeun(SolverHeun &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverHeun &operator=(const SolverHeun &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverHeun &operator=(SolverHeun &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    SolverInfoPtr info() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;

    bool solve(double &pVoi, double pVoiEnd) const override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverHeun(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

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

#include "libopencor/solvernla.h"

namespace libOpenCOR {

/**
 * @brief The SolverKinsol class.
 *
 * The SolverKinsol class is used to represent the KINSOL solver.
 */

class SolverKinsol: public SolverNla
{
    friend class Solver;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverKinsol() override; /**< Destructor, @private. */

    SolverKinsol(const SolverKinsol &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverKinsol(SolverKinsol &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverKinsol &operator=(const SolverKinsol &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverKinsol &operator=(SolverKinsol &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    SolverInfoPtr info() const override;

    Solver::Type type() const override;
    std::string id() const override;
    std::string name() const override;

    bool solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData) override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverKinsol(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

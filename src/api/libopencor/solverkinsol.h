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

#include "solvernla.h"

namespace libOpenCOR {

/**
 * @brief The SolverKinsol class.
 *
 * The SolverKinsol class is used to represent the <a href="https://computing.llnl.gov/projects/sundials/kinsol">KINSOL</a> solver.
 */

class LIBOPENCOR_EXPORT SolverKinsol: public SolverNla
{
public:
    /**
     * @brief The linear solver.
     *
     * The linear solver, i.e. dense, banded, GMRES, BiCGStab, or TFQMR.
     */

    enum class LinearSolver
    {
        DENSE, /**< A dense linear solver. */
        BANDED, /**< A banded linear solver. */
        GMRES, /**< A GMRES linear solver. */
        BICGSTAB, /**< A BiCGStab linear solver. */
        TFQMR /**< The TFQMR linear solver. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverKinsol() override; /**< Destructor, @private. */

    SolverKinsol(const SolverKinsol &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverKinsol(SolverKinsol &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverKinsol &operator=(const SolverKinsol &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverKinsol &operator=(SolverKinsol &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SolverKinsol object.
     *
     * Factory method to create a @ref SolverKinsol object:
     *
     * ```
     * auto solver = libOpenCOR::SolverKinsol::create();
     * ```
     *
     * @return A smart pointer to a @ref SolverKinsol object.
     */

    static SolverKinsolPtr create();

    std::string id() const override;
    std::string name() const override;

    /**
     * @brief Get the maximum number of iterations used by the solver.
     *
     * Return the maximum number of iterations used by the solver.
     *
     * @return The maximum number of iterations used by the solver.
     */

    int maximumNumberOfIterations() const;

    /**
     * @brief Set the maximum number of iterations to be used by the solver.
     *
     * Set the maximum number of iterations to be used by the solver.
     *
     * @param pMaximumNumberOfIterations The maximum number of iterations to be used by the solver.
     */

    void setMaximumNumberOfIterations(int pMaximumNumberOfIterations);

    /**
     * @brief Get the linear solver used by the solver.
     *
     * Return the linear solver used by the solver.
     *
     * @return The linear solver used by the solver.
     */

    LinearSolver linearSolver() const;

    /**
     * @brief Set the linear solver to be used by the solver.
     *
     * Set the linear solver to be used by the solver.
     *
     * @param pLinearSolver The linear solver to be used by the solver.
     */

    void setLinearSolver(LinearSolver pLinearSolver);

    /**
     * @brief Get the upper half-bandwidth used by the solver.
     *
     * Return the upper half-bandwidth used by the solver.
     *
     * @return The upper half-bandwidth used by the solver.
     */

    int upperHalfBandwidth() const;

    /**
     * @brief Set the upper half-bandwidth to be used by the solver.
     *
     * Set the upper half-bandwidth to be used by the solver.
     *
     * @param pUpperHalfBandwidth The upper half-bandwidth to be used by the solver.
     */

    void setUpperHalfBandwidth(int pUpperHalfBandwidth);

    /**
     * @brief Get the lower half-bandwidth used by the solver.
     *
     * Return the lower half-bandwidth used by the solver.
     *
     * @return The lower half-bandwidth used by the solver.
     */

    int lowerHalfBandwidth() const;

    /**
     * @brief Set the lower half-bandwidth to be used by the solver.
     *
     * Set the lower half-bandwidth to be used by the solver.
     *
     * @param pLowerHalfBandwidth The lower half-bandwidth to be used by the solver.
     */

    void setLowerHalfBandwidth(int pLowerHalfBandwidth);

    bool solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData) override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverKinsol(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

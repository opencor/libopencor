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
     * auto solver {libOpenCOR::SolverKinsol::create()};
     * ```
     *
     * @return A smart pointer to a @ref SolverKinsol object.
     */

    static SolverKinsolPtr create();

    /**
     * @brief Return the maximum number of iterations.
     *
     * Return the maximum number of iterations.
     *
     * @return The maximum number of iterations.
     */

    int maximumNumberOfIterations() const;

    /**
     * @brief Set the maximum number of iterations.
     *
     * Set the maximum number of iterations.
     *
     * @param pMaximumNumberOfIterations The maximum number of iterations.
     */

    void setMaximumNumberOfIterations(int pMaximumNumberOfIterations);

    /**
     * @brief Return the linear solver.
     *
     * Return the linear solver.
     *
     * @return The linear solver.
     */

    LinearSolver linearSolver() const;

    /**
     * @brief Set the linear solver.
     *
     * Set the linear solver.
     *
     * @param pLinearSolver The linear solver.
     */

    void setLinearSolver(LinearSolver pLinearSolver);

    /**
     * @brief Return the upper half-bandwidth.
     *
     * Return the upper half-bandwidth.
     *
     * @return The upper half-bandwidth.
     */

    int upperHalfBandwidth() const;

    /**
     * @brief Set the upper half-bandwidth.
     *
     * Set the upper half-bandwidth.
     *
     * @param pUpperHalfBandwidth The upper half-bandwidth.
     */

    void setUpperHalfBandwidth(int pUpperHalfBandwidth);

    /**
     * @brief Return the lower half-bandwidth.
     *
     * Return the lower half-bandwidth.
     *
     * @return The lower half-bandwidth.
     */

    int lowerHalfBandwidth() const;

    /**
     * @brief Set the lower half-bandwidth.
     *
     * Set the lower half-bandwidth.
     *
     * @param pLowerHalfBandwidth The lower half-bandwidth.
     */

    void setLowerHalfBandwidth(int pLowerHalfBandwidth);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SolverKinsol(); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

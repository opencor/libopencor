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
 * The SolverCvode class is used to represent the <a href="https://computing.llnl.gov/projects/sundials/cvode">CVODE</a> solver.
 */

class LIBOPENCOR_EXPORT SolverCvode: public SolverOde
{
public:
    /**
     * @brief The integration method.
     *
     * The integration method, i.e. Adams-Moulton or BDF.
     */

    enum class IntegrationMethod
    {
        ADAMS_MOULTON, /**< The Adams-Moulton integration method. */
        BDF /**< The BDF integration method. */
    };

    /**
     * @brief The iteration type.
     *
     * The iteration type, i.e. functional or Newton.
     */

    enum class IterationType
    {
        FUNCTIONAL, /**< A functional iteration type. */
        NEWTON /**< A Newton iteration type. */
    };

    /**
     * @brief The linear solver.
     *
     * The linear solver, i.e. dense, banded, diagonal, GMRES, BiCGStab, or TFQMR.
     */

    enum class LinearSolver
    {
        DENSE, /**< A dense linear solver. */
        BANDED, /**< A banded linear solver. */
        DIAGONAL, /**< A diagonal linear solver. */
        GMRES, /**< A GMRES linear solver. */
        BICGSTAB, /**< A BiCGStab linear solver. */
        TFQMR /**< The TFQMR linear solver. */
    };

    /**
     * @brief The preconditioner.
     *
     * The preconditioner, i.e. no or banded.
     */

    enum class Preconditioner
    {
        NO, /**< No preconditioner. */
        BANDED /**< A banded preconditioner. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SolverCvode() override; /**< Destructor, @private. */

    SolverCvode(const SolverCvode &pOther) = delete; /**< No copy constructor allowed, @private. */
    SolverCvode(SolverCvode &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SolverCvode &operator=(const SolverCvode &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SolverCvode &operator=(SolverCvode &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SolverCvode object.
     *
     * Factory method to create a @ref SolverCvode object:
     *
     * ```
     * auto solver {libOpenCOR::SolverCvode::create()};
     * ```
     *
     * @return A smart pointer to a @ref SolverCvode object.
     */

    static SolverCvodePtr create();

    /**
     * @brief Return the maximum step.
     *
     * Return the maximum step.
     *
     * @return The maximum step.
     */

    double maximumStep() const;

    /**
     * @brief Set the maximum step.
     *
     * Set the maximum step.
     *
     * @param pMaximumStep The maximum step.
     */

    void setMaximumStep(double pMaximumStep);

    /**
     * @brief Return the maximum number of steps.
     *
     * Return the maximum number of steps.
     *
     * @return The maximum number of steps.
     */

    int maximumNumberOfSteps() const;

    /**
     * @brief Set the maximum number of steps.
     *
     * Set the maximum number of steps.
     *
     * @param pMaximumNumberOfSteps The maximum number of steps.
     */

    void setMaximumNumberOfSteps(int pMaximumNumberOfSteps);

    /**
     * @brief Return the integration method.
     *
     * Return the integration method.
     *
     * @return The integration method.
     */

    IntegrationMethod integrationMethod() const;

    /**
     * @brief Set the integration method.
     *
     * Set the integration method.
     *
     * @param pIntegrationMethod The integration method.
     */

    void setIntegrationMethod(IntegrationMethod pIntegrationMethod);

    /**
     * @brief Return the iteration type.
     *
     * Return the iteration type.
     *
     * @return The iteration type.
     */

    IterationType iterationType() const;

    /**
     * @brief Set the iteration type.
     *
     * Set the iteration type.
     *
     * @param pIterationType The iteration type.
     */

    void setIterationType(IterationType pIterationType);

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
     * @brief Return the preconditioner.
     *
     * Return the preconditioner.
     *
     * @return The preconditioner.
     */

    Preconditioner preconditioner() const;

    /**
     * @brief Set the preconditioner.
     *
     * Set the preconditioner.
     *
     * @param pPreconditioner The preconditioner.
     */

    void setPreconditioner(Preconditioner pPreconditioner);

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

    /**
     * @brief Return the relative tolerance.
     *
     * Return the relative tolerance.
     *
     * @return The relative tolerance.
     */

    double relativeTolerance() const;

    /**
     * @brief Set the relative tolerance.
     *
     * Set the relative tolerance.
     *
     * @param pRelativeTolerance The relative tolerance.
     */

    void setRelativeTolerance(double pRelativeTolerance);

    /**
     * @brief Return the absolute tolerance.
     *
     * Return the absolute tolerance.
     *
     * @return The absolute tolerance.
     */

    double absoluteTolerance() const;

    /**
     * @brief Set the absolute tolerance.
     *
     * Set the absolute tolerance.
     *
     * @param pAbsoluteTolerance The absolute tolerance.
     */

    void setAbsoluteTolerance(double pAbsoluteTolerance);

    /**
     * @brief Return whether the solution should be interpolated or not.
     *
     * Return whether the solution should be interpolated or not.
     *
     * @return Whether the solution should be interpolated or not.
     */

    bool interpolateSolution() const;

    /**
     * @brief Set whether the solution should be interpolated.
     *
     * Set whether the solution should be interpolated.
     *
     * @param pInterpolateSolution Whether the solution should be interpolated.
     */

    void setInterpolateSolution(bool pInterpolateSolution);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SolverCvode(); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

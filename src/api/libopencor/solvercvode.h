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
     * auto solver = libOpenCOR::SolverCvode::create();
     * ```
     *
     * @return A smart pointer to a @ref SolverCvode object.
     */

    static SolverCvodePtr create();

    /**
     * @brief Get the maximum step used by the solver.
     *
     * Return the maximum step used by the solver.
     *
     * @return The maximum step used by the solver.
     */

    double maximumStep() const;

    /**
     * @brief Set the maximum step to be used by the solver.
     *
     * Set the maximum step to be used by the solver.
     *
     * @param pMaximumStep The maximum step to be used by the solver.
     */

    void setMaximumStep(double pMaximumStep);

    /**
     * @brief Get the maximum number of steps used by the solver.
     *
     * Return the maximum number of steps used by the solver.
     *
     * @return The maximum number of steps used by the solver.
     */

    int maximumNumberOfSteps() const;

    /**
     * @brief Set the maximum number of steps to be used by the solver.
     *
     * Set the maximum number of steps to be used by the solver.
     *
     * @param pMaximumNumberOfSteps The maximum number of steps to be used by the solver.
     */

    void setMaximumNumberOfSteps(int pMaximumNumberOfSteps);

    /**
     * @brief Get the integration method used by the solver.
     *
     * Return the integration method used by the solver.
     *
     * @return The integration method used by the solver.
     */

    IntegrationMethod integrationMethod() const;

    /**
     * @brief Set the integration method to be used by the solver.
     *
     * Set the integration method to be used by the solver.
     *
     * @param pIntegrationMethod The integration method to be used by the solver.
     */

    void setIntegrationMethod(IntegrationMethod pIntegrationMethod);

    /**
     * @brief Get the iteration type used by the solver.
     *
     * Return the iteration type used by the solver.
     *
     * @return The iteration type used by the solver.
     */

    IterationType iterationType() const;

    /**
     * @brief Set the iteration type to be used by the solver.
     *
     * Set the iteration type to be used by the solver.
     *
     * @param pIterationType The iteration type to be used by the solver.
     */

    void setIterationType(IterationType pIterationType);

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
     * @brief Get the preconditioner used by the solver.
     *
     * Return the preconditioner used by the solver.
     *
     * @return The preconditioner used by the solver.
     */

    Preconditioner preconditioner() const;

    /**
     * @brief Set the preconditioner to be used by the solver.
     *
     * Set the preconditioner to be used by the solver.
     *
     * @param pPreconditioner The preconditioner to be used by the solver.
     */

    void setPreconditioner(Preconditioner pPreconditioner);

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

    /**
     * @brief Get the relative tolerance used by the solver.
     *
     * Return the relative tolerance used by the solver.
     *
     * @return The relative tolerance used by the solver.
     */

    double relativeTolerance() const;

    /**
     * @brief Set the relative tolerance to be used by the solver.
     *
     * Set the relative tolerance to be used by the solver.
     *
     * @param pRelativeTolerance The relative tolerance to be used by the solver.
     */

    void setRelativeTolerance(double pRelativeTolerance);

    /**
     * @brief Get the absolute tolerance used by the solver.
     *
     * Return the absolute tolerance used by the solver.
     *
     * @return The absolute tolerance used by the solver.
     */

    double absoluteTolerance() const;

    /**
     * @brief Set the absolute tolerance to be used by the solver.
     *
     * Set the absolute tolerance to be used by the solver.
     *
     * @param pAbsoluteTolerance The absolute tolerance to be used by the solver.
     */

    void setAbsoluteTolerance(double pAbsoluteTolerance);

    /**
     * @brief Get whether the solution should be interpolated or not.
     *
     * Return whether the solution should be interpolated or not.
     *
     * @return Whether the solution should be interpolated or not.
     */

    bool interpolateSolution() const;

    /**
     * @brief Set whether the solution should be interpolated or not.
     *
     * Set whether the solution should be interpolated or not.
     *
     * @param pInterpolateSolution Whether the solution should be interpolated or not.
     */

    void setInterpolateSolution(bool pInterpolateSolution);

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    ComputeRates pComputeRates) override;
    bool reinitialise(double pVoi) override;

    bool solve(double &pVoi, double pVoiEnd) override;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */

    explicit SolverCvode(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

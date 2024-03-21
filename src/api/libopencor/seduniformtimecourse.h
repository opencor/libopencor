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

#include "libopencor/sedsimulation.h"

namespace libOpenCOR {

/**
 * @brief The SedUniformTimeCourse class.
 *
 * The SedUniformTimeCourse class is used to describe a uniform time course simulation in the context of a simulation
 * experiment description.
 */

class LIBOPENCOR_EXPORT SedUniformTimeCourse: public SedSimulation
{
    friend class SedInstanceTask;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedUniformTimeCourse() override; /**< Destructor, @private. */

    SedUniformTimeCourse(const SedUniformTimeCourse &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedUniformTimeCourse(SedUniformTimeCourse &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedUniformTimeCourse &operator=(const SedUniformTimeCourse &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedUniformTimeCourse &operator=(SedUniformTimeCourse &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedUniformTimeCourse object.
     *
     * Factory method to create a @ref SedUniformTimeCourse object:
     *
     * ```
     * auto sed = libOpenCOR::SedDocument::create();
     * auto simulation = libOpenCOR::SedUniformTimeCourse::create(sed);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedUniformTimeCourse object is to
     * belong.
     *
     * @return A smart pointer to a @ref SedUniformTimeCourse object.
     */

    static SedUniformTimeCoursePtr create(const SedDocumentPtr &pDocument);

    /**
     * @brief Get the initial time used by this simulation.
     *
     * Return the initial time used by this simulation.
     *
     * @return The initial time used by this simulation.
     *
     * @warning ---GRY--- The initial time of a simulation is currently ignored. We currently assume that the initial
     * time is always the same as the output start time.
     */

    double initialTime() const;

    /**
     * @brief Set the initial time to be used by this simulation.
     *
     * Set the initial time to be used by this simulation.
     *
     * @param pInitialTime The initial time to be used by this simulation.
     *
     * @warning ---GRY--- The initial time of a simulation is currently ignored. We currently assume that the initial
     * time is always the same as the output start time.
     */

    void setInitialTime(double pInitialTime);

    /**
     * @brief Get the output start time used by this simulation.
     *
     * Return the output start time used by this simulation.
     *
     * @return The output start time used by this simulation.
     */

    double outputStartTime() const;

    /**
     * @brief Set the output start time to be used by this simulation.
     *
     * Set the output start time to be used by this simulation.
     *
     * @param pOutputStartTime The output start time to be used by this simulation.
     */

    void setOutputStartTime(double pOutputStartTime);

    /**
     * @brief Get the output end time used by this simulation.
     *
     * Return the output end time used by this simulation.
     *
     * @return The output end time used by this simulation.
     */

    double outputEndTime() const;

    /**
     * @brief Set the output end time to be used by this simulation.
     *
     * Set the output end time to be used by this simulation.
     *
     * @param pOutputEndTime The output end time to be used by this simulation.
     */

    void setOutputEndTime(double pOutputEndTime);

    /**
     * @brief Get the number of steps used by this simulation.
     *
     * Return the number of steps used by this simulation.
     *
     * @return The number of steps used by this simulation.
     */

    int numberOfSteps() const;

    /**
     * @brief Set the number of steps to be used by this simulation.
     *
     * Set the number of steps to be used by this simulation.
     *
     * @param pNumberOfSteps The number of steps to be used by this simulation.
     */

    void setNumberOfSteps(int pNumberOfSteps);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedUniformTimeCourse(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

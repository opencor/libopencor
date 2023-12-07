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
 * @brief The SedSimulationUniformTimeCourse class.
 *
 * The SedSimulationUniformTimeCourse class is used to describe a uniform time course simulation in the context of a
 * simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedSimulationUniformTimeCourse: public SedSimulation
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedSimulationUniformTimeCourse() override; /**< Destructor, @private. */

    SedSimulationUniformTimeCourse(const SedSimulationUniformTimeCourse &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedSimulationUniformTimeCourse(SedSimulationUniformTimeCourse &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedSimulationUniformTimeCourse &operator=(const SedSimulationUniformTimeCourse &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedSimulationUniformTimeCourse &operator=(SedSimulationUniformTimeCourse &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedSimulationUniformTimeCourse object.
     *
     * Factory method to create a @ref SedSimulationUniformTimeCourse object:
     *
     * ```
     * auto sedDocument = libOpenCOR::SedDocument::create();
     * auto sedSimulationUniformTimeCourse = libOpenCOR::SedSimulationUniformTimeCourse::create(sedDocument);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedSimulationUniformTimeCourse object is to
     * belong.
     *
     * @return A smart pointer to a @ref SedSimulationUniformTimeCourse object.
     */

    static SedSimulationUniformTimeCoursePtr create(const SedDocumentPtr &pDocument);

    /**
     * @brief Get the initial time used by the simulation.
     *
     * Return the initial time used by the simulation.
     *
     * @return The initial time used by the simulation.
     *
     * @warning ---GRY--- The initial time of a simulation is currently ignored. We currently assume that the initial
     * time is always the same as the output start time.
     */

    double initialTime() const;

    /**
     * @brief Set the initial time to be used by the simulation.
     *
     * Set the initial time to be used by the simulation.
     *
     * @param pInitialTime The initial time to be used by the simulation.
     *
     * @warning ---GRY--- The initial time of a simulation is currently ignored. We currently assume that the initial
     * time is always the same as the output start time.
     */

    void setInitialTime(double pInitialTime);

    /**
     * @brief Get the output start time used by the simulation.
     *
     * Return the output start time used by the simulation.
     *
     * @return The output start time used by the simulation.
     */

    double outputStartTime() const;

    /**
     * @brief Set the output start time to be used by the simulation.
     *
     * Set the output start time to be used by the simulation.
     *
     * @param pOutputStartTime The output start time to be used by the simulation.
     */

    void setOutputStartTime(double pOutputStartTime);

    /**
     * @brief Get the output end time used by the simulation.
     *
     * Return the output end time used by the simulation.
     *
     * @return The output end time used by the simulation.
     */

    double outputEndTime() const;

    /**
     * @brief Set the output end time to be used by the simulation.
     *
     * Set the output end time to be used by the simulation.
     *
     * @param pOutputEndTime The output end time to be used by the simulation.
     */

    void setOutputEndTime(double pOutputEndTime);

    /**
     * @brief Get the number of steps used by the simulation.
     *
     * Return the number of steps used by the simulation.
     *
     * @return The number of steps used by the simulation.
     */

    int numberOfSteps() const;

    /**
     * @brief Set the number of steps to be used by the simulation.
     *
     * Set the number of steps to be used by the simulation.
     *
     * @param pNumberOfSteps The number of steps to be used by the simulation.
     */

    void setNumberOfSteps(int pNumberOfSteps);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedSimulationUniformTimeCourse(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

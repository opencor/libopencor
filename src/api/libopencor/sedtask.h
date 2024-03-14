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

#include "libopencor/sedabstracttask.h"

namespace libOpenCOR {

/**
 * @brief The SedTask class.
 *
 * The SedTask class is used to describe a task in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedTask: public SedAbstractTask
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedTask() override; /**< Destructor, @private. */

    SedTask(const SedTask &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedTask(SedTask &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedTask &operator=(const SedTask &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedTask &operator=(SedTask &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedTask object.
     *
     * Factory method to create a @ref SedTask object:
     *
     * ```
     * auto sed = libOpenCOR::SedDocument::create();
     * auto task = libOpenCOR::SedTask::create(sed);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedTask object is to belong.
     * @param pModel The model to be used by this task.
     * @param pSimulation The simulation to be used by this task.
     *
     * @return A smart pointer to a @ref SedTask object.
     */

    static SedTaskPtr create(const SedDocumentPtr &pDocument, const SedModelPtr &pModel,
                             const SedSimulationPtr &pSimulation);

    /**
     * @brief Get the model used by this task.
     *
     * Return the model used by this task.
     *
     * @return The model, as a @ref SedModelPtr, used by this task.
     */

    SedModelPtr model() const;

    /**
     * @brief Set the model to be used by this task.
     *
     * Set the model to be used by this task.
     *
     * @param pModel The model to be used by this task.
     */

    void setModel(const SedModelPtr &pModel);

    /**
     * @brief Get the simulation used by this task.
     *
     * Return the simulation used by this task.
     *
     * @return The simulation, as a @ref SedSimulationPtr, used by this task.
     */

    SedSimulationPtr simulation() const;

    /**
     * @brief Set the simulation to be used by this task.
     *
     * Set the simulation to be used by this task.
     *
     * @param pSimulation The simulation to be used by this task.
     */

    void setSimulation(const SedSimulationPtr &pSimulation);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedTask(const SedDocumentPtr &pDocument, const SedModelPtr &pModel, const SedSimulationPtr &pSimulation); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

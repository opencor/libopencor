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

#include "libopencor/logger.h"

namespace libOpenCOR {

/**
 * @brief The SedDocument class.
 *
 * The SedDocument class is used to describe a simulation experiment that adheres to the SED-ML specification.
 */

class LIBOPENCOR_EXPORT SedDocument: public Logger
    , public std::enable_shared_from_this<SedDocument>
{
    friend class SedAbstractTask;
    friend class SedInstanceTask;
    friend class SedModel;
    friend class SedSimulation;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedDocument() override; /**< Destructor, @private. */

    SedDocument(const SedDocument &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedDocument(SedDocument &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedDocument &operator=(const SedDocument &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedDocument &operator=(SedDocument &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedDocument object.
     *
     * Factory method to create a @ref SedDocument object:
     *
     * ```
     * auto document = libOpenCOR::SedDocument::create();
     * ```
     *
     * @param pFile The @ref File, if any, used to initialise this @ref SedDocument object.
     *
     * @return A smart pointer to a @ref SedDocument object.
     */

    static SedDocumentPtr create(const FilePtr &pFile = {});

#ifdef __EMSCRIPTEN__
    static SedDocumentPtr defaultCreate();
#endif

    /**
     * @brief Get the serialised version of this simulation experiment description.
     *
     * Return the @c std::string serialised version of this simulation experiment description.
     *
     * @return The @c std::string serialised version of this simulation experiment description.
     */

    std::string serialise() const;

    /**
     * @brief Get the serialised version of this simulation experiment description.
     *
     * Return the @c std::string serialised version of this simulation experiment description.
     *
     * @param pBasePath The @c std::string base path to use to determine the relative path, if possible, of a model
     * source.
     *
     * @return The @c std::string serialised version of this simulation experiment description.
     */

    std::string serialise(const std::string &pBasePath) const;

    /**
     * @brief Return whether there are some models.
     *
     * Return whether there are some models.
     *
     * @return @c true if there are some models, @c false otherwise.
     */

    bool hasModels() const;

    /**
     * @brief Return the number of models.
     *
     * Return the number of models.
     *
     * @return The number of models.
     */

    size_t modelCount() const;

    /**
     * @brief Return the models.
     *
     * Return the models.
     *
     * @return The models, as a @ref SedModelPtrs.
     */

    SedModelPtrs models() const;

    /**
     * @brief Return the model at the given index.
     *
     * Return the model at the given index.
     *
     * @param pIndex The index of the model to return.
     *
     * @return The model as a @ref SedModelPtr, if the index is valid, @c nullptr otherwise.
     */

    SedModelPtr model(size_t pIndex) const;

    /**
     * @brief Add the model to this simulation experiment description.
     *
     * Add the model to this simulation experiment description.
     *
     * @param pModel The @ref SedModel object to be added.
     *
     * @return @c true if the model was added, @c false otherwise.
     */

    bool addModel(const SedModelPtr &pModel);

    /**
     * @brief Remove the model from this simulation experiment description.
     *
     * Remove the model from this simulation experiment description.
     *
     * @param pModel The @ref SedModel object to be removed.
     *
     * @return @c true if the model was removed, @c false otherwise.
     */

    bool removeModel(const SedModelPtr &pModel);

    /**
     * @brief Remove all models from this simulation experiment description.
     *
     * Remove all models from this simulation experiment description.
     *
     * @return @c true if all models were removed, @c false otherwise.
     */
    bool removeAllModels();

    /**
     * @brief Return whether there are some simulations.
     *
     * Return whether there are some simulations.
     *
     * @return @c true if there are some simulations, @c false otherwise.
     */

    bool hasSimulations() const;

    /**
     * @brief Return the number of simulations.
     *
     * Return the number of simulations.
     *
     * @return The number of simulations.
     */

    size_t simulationCount() const;

    /**
     * @brief Return the simulations.
     *
     * Return the simulations.
     *
     * @return The simulations, as a @ref SedSimulationPtrs.
     */

    SedSimulationPtrs simulations() const;

    /**
     * @brief Return the simulation at the given index.
     *
     * Return the simulation at the given index.
     *
     * @param pIndex The index of the simulation to return.
     *
     * @return The simulation as a @ref SedSimulationPtr, if the index is valid, @c nullptr otherwise.
     */

    SedSimulationPtr simulation(size_t pIndex) const;

    /**
     * @brief Add the simulation to this simulation experiment description.
     *
     * Add the simulation to this simulation experiment description.
     *
     * @param pSimulation The @ref SedSimulation object to be added.
     *
     * @return @c true if the simulation was added, @c false otherwise.
     */

    bool addSimulation(const SedSimulationPtr &pSimulation);

    /**
     * @brief Remove the simulation from this simulation experiment description.
     *
     * Remove the simulation from this simulation experiment description.
     *
     * @param pSimulation The @ref SedSimulation object to be removed.
     *
     * @return @c true if the simulation was removed, @c false otherwise.
     */

    bool removeSimulation(const SedSimulationPtr &pSimulation);

    /**
     * @brief Remove all simulations from this simulation experiment description.
     *
     * Remove all simulations from this simulation experiment description.
     *
     * @return @c true if all simulations were removed, @c false otherwise.
     */
    bool removeAllSimulations();

    /**
     * @brief Return whether there are some tasks.
     *
     * Return whether there are some tasks.
     *
     * @return @c true if there are some tasks, @c false otherwise.
     */

    bool hasTasks() const;

    /**
     * @brief Return the number of tasks.
     *
     * Return the number of tasks.
     *
     * @return The number of tasks.
     */

    size_t taskCount() const;

    /**
     * @brief Return the tasks.
     *
     * Return the tasks.
     *
     * @return The tasks, as a @ref SedAbstractTaskPtrs.
     */

    SedAbstractTaskPtrs tasks() const;

    /**
     * @brief Return the task at the given index.
     *
     * Return the task at the given index.
     *
     * @param pIndex The index of the task to return.
     *
     * @return The task as a @ref SedAbstractTaskPtr, if the index is valid, @c nullptr otherwise.
     */

    SedAbstractTaskPtr task(size_t pIndex) const;

    /**
     * @brief Add the task to this simulation experiment description.
     *
     * Add the task to this simulation experiment description.
     *
     * @param pTask The @ref SedAbstractTask object to be added.
     *
     * @return @c true if the task was added, @c false otherwise.
     */

    bool addTask(const SedAbstractTaskPtr &pTask);

    /**
     * @brief Remove the task from this simulation experiment description.
     *
     * Remove the task from this simulation experiment description.
     *
     * @param pTask The @ref SedAbstractTask object to be removed.
     *
     * @return @c true if the task was removed, @c false otherwise.
     */

    bool removeTask(const SedAbstractTaskPtr &pTask);

    /**
     * @brief Remove all tasks from this simulation experiment description.
     *
     * Remove all tasks from this simulation experiment description.
     *
     * @return @c true if all tasks were removed, @c false otherwise.
     */
    bool removeAllTasks();

    /**
     * @brief Create an instance of this simulation experiment description.
     *
     * Create an instance of this simulation experiment description.
     *
     * @param pCompiled Whether the instance of this simulation experiment description is to be compiled (as opposed to
     * being interpreted).
     *
     * @return A smart pointer to a @ref SedInstance object.
     */

#ifdef __EMSCRIPTEN__
    SedInstancePtr instantiate();
#else
    SedInstancePtr instantiate(bool pCompiled = true);
#endif

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedDocument(); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

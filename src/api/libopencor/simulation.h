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
 * @brief The Simulation class.
 *
 * The Simulation class is used to represent a simulation associated with a CellML file, a SED-ML file, or a COMBINE
 * archive.
 */

class LIBOPENCOR_EXPORT Simulation: public Logger
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    Simulation() = delete; /**< No default constructor allowed, @private. */
    ~Simulation(); /**< Destructor, @private. */

    Simulation(const Simulation &pOther) = delete; /**< No copy constructor allowed, @private. */
    Simulation(Simulation &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    Simulation &operator=(const Simulation &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    Simulation &operator=(Simulation &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref Simulation object.
     *
     * Factory method to create a @ref Simulation object:
     *
     * ```
     * auto file = libOpenCOR::File::create(...);
     * auto simulation = libOpenCOR::Simulation::create(file);
     * ```
     *
     * @param pFile The @ref File for which we want to create a simulation.
     *
     * @return A smart pointer to a @ref Simulation object.
     */

    static SimulationPtr create(const FilePtr &pFile);

    /**
     * @brief Run the simulation.
     *
     * Run the simulation.
     */

    void run();

    /**
     * @brief Pause the simulation.
     *
     * Pause the simulation.
     */

    void pause();

    /**
     * @brief Resume the simulation.
     *
     * Resume the simulation.
     */

    void resume();

    /**
     * @brief Stop the simulation.
     *
     * Stop the simulation.
     */

    void stop();

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit Simulation(const FilePtr &pFile); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

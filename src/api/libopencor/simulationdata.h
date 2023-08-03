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
 * @brief The SimulationData class.
 *
 * The SimulationData class contains the various data that are used to run a simulation.
 */

class SimulationData: public Logger
{
    friend class Simulation;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SimulationData(); /**< Destructor, @private. */

    SimulationData(const SimulationData &pOther) = delete; /**< No copy constructor allowed, @private. */
    SimulationData(SimulationData &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SimulationData &operator=(const SimulationData &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SimulationData &operator=(SimulationData &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Return whether the settings are valid.
     *
     * Return whether the settings are valid.
     *
     * @return @c true if the settings are valid, @c false otherwise.
     */

    bool isValid() const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SimulationData(); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

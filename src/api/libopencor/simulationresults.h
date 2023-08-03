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
 * @brief The SimulationResults class.
 *
 * The SimulationResults class is used to keep track of a simulation's results.
 */

class SimulationResults: public Logger
{
    friend class Simulation;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SimulationResults(); /**< Destructor, @private. */

    SimulationResults(const SimulationResults &pOther) = delete; /**< No copy constructor allowed, @private. */
    SimulationResults(SimulationResults &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SimulationResults &operator=(const SimulationResults &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SimulationResults &operator=(SimulationResults &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SimulationResults(); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

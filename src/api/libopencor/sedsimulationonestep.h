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
 * @brief The SedSimulationOneStep class.
 *
 * The SedSimulationOneStep class is used to describe a one step simulation in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedSimulationOneStep: public SedSimulation
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedSimulationOneStep() override; /**< Destructor, @private. */

    SedSimulationOneStep(const SedSimulationOneStep &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedSimulationOneStep(SedSimulationOneStep &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedSimulationOneStep &operator=(const SedSimulationOneStep &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedSimulationOneStep &operator=(SedSimulationOneStep &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedSimulationOneStep object.
     *
     * Factory method to create a @ref SedSimulationOneStep object:
     *
     * ```
     * auto sedDocument = libOpenCOR::SedDocument::create();
     * auto sedSimulationOneStep = libOpenCOR::SedSimulationOneStep::create(sedDocument);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedSimulationOneStep object belongs.
     *
     * @return A smart pointer to a @ref SedSimulationOneStep object.
     */

    static SedSimulationOneStepPtr create(const SedDocumentPtr &pDocument);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedSimulationOneStep(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

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
 * @brief The SedOneStep class.
 *
 * The SedOneStep class is used to describe a one step simulation in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedOneStep: public SedSimulation
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedOneStep() override; /**< Destructor, @private. */

    SedOneStep(const SedOneStep &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedOneStep(SedOneStep &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedOneStep &operator=(const SedOneStep &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedOneStep &operator=(SedOneStep &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedOneStep object.
     *
     * Factory method to create a @ref SedOneStep object:
     *
     * ```
     * auto document {libOpenCOR::SedDocument::create()};
     * auto simulation {libOpenCOR::SedOneStep::create(document)};
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedOneStep object is to belong.
     *
     * @return A smart pointer to a @ref SedOneStep object.
     */

    static SedOneStepPtr create(const SedDocumentPtr &pDocument);

    /**
     * @brief Get the step used by this simulation.
     *
     * Return the step used by this simulation.
     *
     * @return The step used by this simulation.
     */

    double step() const;

    /**
     * @brief Set the step to be used by this simulation.
     *
     * Set the step to be used by this simulation.
     *
     * @param pStep The step to be used by this simulation.
     */

    void setStep(double pStep);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedOneStep(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

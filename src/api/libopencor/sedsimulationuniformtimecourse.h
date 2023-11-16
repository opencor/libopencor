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
     * @param pDocument The @ref SedDocument object to which the @ref SedSimulationUniformTimeCourse object belongs.
     *
     * @return A smart pointer to a @ref SedSimulationUniformTimeCourse object.
     */

    static SedSimulationUniformTimeCoursePtr create(const SedDocumentPtr &pDocument);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedSimulationUniformTimeCourse(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

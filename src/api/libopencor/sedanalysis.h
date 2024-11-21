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
 * @brief The SedAnalysis class.
 *
 * The SedAnalysis class is used to describe an analysis simulation in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedAnalysis: public SedSimulation
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedAnalysis() override; /**< Destructor, @private. */

    SedAnalysis(const SedAnalysis &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedAnalysis(SedAnalysis &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedAnalysis &operator=(const SedAnalysis &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedAnalysis &operator=(SedAnalysis &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedAnalysis object.
     *
     * Factory method to create a @ref SedAnalysis object:
     *
     * ```
     * auto document = libOpenCOR::SedDocument::create();
     * auto simulation = libOpenCOR::SedAnalysis::create(document);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedAnalysis object is to belong.
     *
     * @return A smart pointer to a @ref SedAnalysis object.
     */

    static SedAnalysisPtr create(const SedDocumentPtr &pDocument);

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedAnalysis(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

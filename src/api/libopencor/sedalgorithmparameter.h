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

#include "libopencor/sedbase.h"

namespace libOpenCOR {

/**
 * @brief The SedAlgorithmParameter class.
 *
 * The SedAlgorithmParameter class is used to describe an algorithm parameter in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedAlgorithmParameter: public SedBase
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    //---GRY---
    // ~SedAlgorithmParameter() override; /**< Destructor, @private. */

    SedAlgorithmParameter(const SedAlgorithmParameter &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedAlgorithmParameter(SedAlgorithmParameter &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedAlgorithmParameter &operator=(const SedAlgorithmParameter &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedAlgorithmParameter &operator=(SedAlgorithmParameter &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    //---GRY---
    // explicit SedAlgorithmParameter(); /**< Constructor @private. */

    // Impl *pimpl(); /**< Private implementation pointer, @private. */
    // const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

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
 * @brief The SedDataDescription class.
 *
 * The SedDataDescription class is used to describe some data description in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedDataDescription: public SedBase
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    //---GRY---
    // ~SedDataDescription() override; /**< Destructor, @private. */

    SedDataDescription(const SedDataDescription &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedDataDescription(SedDataDescription &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedDataDescription &operator=(const SedDataDescription &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedDataDescription &operator=(SedDataDescription &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    //---GRY---
    // explicit SedDataDescription(); /**< Constructor @private. */

    // Impl *pimpl(); /**< Private implementation pointer, @private. */
    // const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

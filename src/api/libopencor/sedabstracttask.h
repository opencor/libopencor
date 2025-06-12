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
 * @brief The SedAbstractTask class.
 *
 * The SedAbstractTask class is used to describe an abstract task in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedAbstractTask: public SedBase
{
    friend class SedDocument;
    friend class SedInstance;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    SedAbstractTask(const SedAbstractTask &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedAbstractTask(SedAbstractTask &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedAbstractTask &operator=(const SedAbstractTask &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedAbstractTask &operator=(SedAbstractTask &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

protected:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedAbstractTask(Impl *pPimpl); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

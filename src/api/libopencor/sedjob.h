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
 * @brief The SedJob class.
 *
 * The SedJob class is used to describe a run in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedJob: public Logger
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedJob() override; /**< Destructor, @private. */

    SedJob(const SedJob &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedJob(SedJob &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedJob &operator=(const SedJob &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedJob &operator=(SedJob &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedJob(); /**< Constructor @private. */
};

} // namespace libOpenCOR

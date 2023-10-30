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
 * @brief The SedUniformTimeCourse class.
 *
 * The SedUniformTimeCourse class is used to describe a uniform time course simulation in the context of a simulation
 * experiment description.
 */

class LIBOPENCOR_EXPORT SedUniformTimeCourse: public SedSimulation
{
public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    //---GRY---
    // ~SedUniformTimeCourse() override; /**< Destructor, @private. */

    SedUniformTimeCourse(const SedUniformTimeCourse &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedUniformTimeCourse(SedUniformTimeCourse &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedUniformTimeCourse &operator=(const SedUniformTimeCourse &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedUniformTimeCourse &operator=(SedUniformTimeCourse &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    //---GRY---
    // explicit SedUniformTimeCourse(); /**< Constructor @private. */

    // Impl *pimpl(); /**< Private implementation pointer, @private. */
    // const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

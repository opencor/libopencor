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
 * @brief The SedInstance class.
 *
 * The SedInstance class is used to describe an instance of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedInstance: public Logger
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedInstance() override; /**< Destructor, @private. */

    SedInstance(const SedInstance &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedInstance(SedInstance &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedInstance &operator=(const SedInstance &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedInstance &operator=(SedInstance &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Run all the tasks associated with this instance.
     *
     * Run all the tasks associated with this instance.
     *
     * @return The elapsed time in milliseconds.
     */

    double run();

    /**
     * @brief Return whether there are some tasks.
     *
     * Return whether there are some tasks.
     *
     * @return @c true if there are some tasks, @c false otherwise.
     */

    bool hasTasks() const;

    /**
     * @brief Return the number of tasks.
     *
     * Return the number of tasks.
     *
     * @return The number of tasks.
     */

    size_t taskCount() const;

    /**
     * @brief Return the tasks.
     *
     * Return the tasks.
     *
     * @return The tasks, as a @ref SedInstanceTaskPtrs.
     */

    SedInstanceTaskPtrs tasks() const;

    /**
     * @brief Return the task at the given index.
     *
     * Return the task at the given index.
     *
     * @param pIndex The index of the task to return.
     *
     * @return The task as a @ref SedInstanceTaskPtr, if the index is valid, @c nullptr otherwise.
     */

    SedInstanceTaskPtr task(size_t pIndex) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedInstance(const SedDocumentPtr &pDocument, bool pCompiled); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

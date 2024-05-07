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
 * @brief The SedInstanceTask class.
 *
 * The SedInstanceTask class is used to describe an instance of a task in the context of a simulation experiment
 * description.
 */

class LIBOPENCOR_EXPORT SedInstanceTask: public Logger
{
    friend class SedInstance;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedInstanceTask() override; /**< Destructor, @private. */

    SedInstanceTask(const SedInstanceTask &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedInstanceTask(SedInstanceTask &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedInstanceTask &operator=(const SedInstanceTask &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedInstanceTask &operator=(SedInstanceTask &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Get the values of the variable of integration.
     *
     * Get the values of the variable of integration.
     *
     * @return The values of the variable of integration.
     */

    Doubles voi() const;

    /**
     * @brief Get the name of the variable of integration.
     *
     * Get the name of the variable of integration.
     *
     * @return The name of the variable of integration.
     */

    std::string voiName() const;

    /**
     * @brief Get the number of states.
     *
     * Get the number of states.
     *
     * @return The number of states.
     */

    size_t stateCount() const;

    /**
     * @brief Get the values of the state at the given index.
     *
     * Get the values of the state at the given index.
     *
     * @param pIndex The index of the state.
     *
     * @return The values of the state.
     */

    Doubles state(size_t pIndex) const;

    /**
     * @brief Get the name of the state at the given index.
     *
     * Get the name of the state at the given index.
     *
     * @param pIndex The index of the state.
     *
     * @return The name of the state.
     */

    std::string stateName(size_t pIndex) const;

    /**
     * @brief Get the number of rates.
     *
     * Get the number of rates.
     *
     * @return The number of rates.
     */

    size_t rateCount() const;

    /**
     * @brief Get the values of the rate at the given index.
     *
     * Get the values of the rate at the given index.
     *
     * @param pIndex The index of the rate.
     *
     * @return The values of the rate.
     */

    Doubles rate(size_t pIndex) const;

    /**
     * @brief Get the name of the rate at the given index.
     *
     * Get the name of the rate at the given index.
     *
     * @param pIndex The index of the rate.
     *
     * @return The name of the rate.
     */

    std::string rateName(size_t pIndex) const;

    /**
     * @brief Get the number of variables.
     *
     * Get the number of variables.
     *
     * @return The number of variables.
     */

    size_t variableCount() const;

    /**
     * @brief Get the values of the variable at the given index.
     *
     * Get the values of the variable at the given index.
     *
     * @param pIndex The index of the variable.
     *
     * @return The values of the variable.
     */

    Doubles variable(size_t pIndex) const;

    /**
     * @brief Get the name of the variable at the given index.
     *
     * Get the name of the variable at the given index.
     *
     * @param pIndex The index of the variable.
     *
     * @return The name of the variable.
     */

    std::string variableName(size_t pIndex) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedInstanceTask(const SedAbstractTaskPtr &pTask, bool pCompiled); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

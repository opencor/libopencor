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
    , public std::enable_shared_from_this<SedInstanceTask>
{
    friend class SedChangeAttribute;
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

#ifdef __EMSCRIPTEN__
    emscripten::val voiAsArray() const;
#endif

    /**
     * @brief Get the name of the variable of integration.
     *
     * Get the name of the variable of integration.
     *
     * @return The name of the variable of integration.
     */

    std::string voiName() const;

    /**
     * @brief Get the unit of the variable of integration.
     *
     * Get the unit of the variable of integration.
     *
     * @return The unit of the variable of integration.
     */

    std::string voiUnit() const;

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
     * @return The values of the state, as a @ref Doubles, if the index is valid, an empty vector otherwise.
     */

    Doubles state(size_t pIndex) const;

#ifdef __EMSCRIPTEN__
    emscripten::val stateAsArray(size_t pIndex) const;
#endif

    /**
     * @brief Get the name of the state.
     *
     * Get the name of the state at the given index.
     *
     * @param pIndex The index of the state.
     *
     * @return The name of the state, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string stateName(size_t pIndex) const;

    /**
     * @brief Get the unit of the state.
     *
     * Get the unit of the state at the given index.
     *
     * @param pIndex The index of the state.
     *
     * @return The unit of the state, as a @c std::string, if the index is valid, an empty vector otherwise.
     */

    std::string stateUnit(size_t pIndex) const;

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
     * @return The values of the rate, as a @ref Doubles, if the index is valid, an empty vector otherwise.
     */

    Doubles rate(size_t pIndex) const;

#ifdef __EMSCRIPTEN__
    emscripten::val rateAsArray(size_t pIndex) const;
#endif

    /**
     * @brief Get the name of the rate.
     *
     * Get the name of the rate at the given index.
     *
     * @param pIndex The index of the rate.
     *
     * @return The name of the rate, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string rateName(size_t pIndex) const;

    /**
     * @brief Get the unit of the rate.
     *
     * Get the unit of the rate at the given index.
     *
     * @param pIndex The index of the rate.
     *
     * @return The unit of the rate, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string rateUnit(size_t pIndex) const;

    /**
     * @brief Get the number of constants.
     *
     * Get the number of constants.
     *
     * @return The number of constants.
     */

    size_t constantCount() const;

    /**
     * @brief Get the values of the constant at the given index.
     *
     * Get the values of the constant at the given index.
     *
     * @param pIndex The index of the constant.
     *
     * @return The values of the constant, as a @ref Doubles, if the index is valid, an empty vector otherwise.
     */

    Doubles constant(size_t pIndex) const;

#ifdef __EMSCRIPTEN__
    emscripten::val constantAsArray(size_t pIndex) const;
#endif

    /**
     * @brief Get the name of the constant.
     *
     * Get the name of the constant at the given index.
     *
     * @param pIndex The index of the constant.
     *
     * @return The name of the constant, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string constantName(size_t pIndex) const;

    /**
     * @brief Get the unit of the constant.
     *
     * Get the unit of the constant at the given index.
     *
     * @param pIndex The index of the constant.
     *
     * @return The unit of the constant, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string constantUnit(size_t pIndex) const;

    /**
     * @brief Get the number of computed constants.
     *
     * Get the number of computed constants.
     *
     * @return The number of computed constants.
     */

    size_t computedConstantCount() const;

    /**
     * @brief Get the values of the computed constant at the given index.
     *
     * Get the values of the computed constant at the given index.
     *
     * @param pIndex The index of the computed constant.
     *
     * @return The values of the computed constant, as a @ref Doubles, if the index is valid, an empty vector otherwise.
     */

    Doubles computedConstant(size_t pIndex) const;

#ifdef __EMSCRIPTEN__
    emscripten::val computedConstantAsArray(size_t pIndex) const;
#endif

    /**
     * @brief Get the name of the computed constant.
     *
     * Get the name of the computed constant at the given index.
     *
     * @param pIndex The index of the computed constant.
     *
     * @return The name of the computed constant, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string computedConstantName(size_t pIndex) const;

    /**
     * @brief Get the unit of the computed constant.
     *
     * Get the unit of the computed constant at the given index.
     *
     * @param pIndex The index of the computed constant.
     *
     * @return The unit of the computed constant, as a @c std::string, if the index is valid, an empty string otherwise.
     */

    std::string computedConstantUnit(size_t pIndex) const;

    /**
     * @brief Get the number of algebraic variables.
     *
     * Get the number of algebraic variables.
     *
     * @return The number of algebraic variables.
     */

    size_t algebraicVariableCount() const;

    /**
     * @brief Get the values of the algebraic variable at the given index.
     *
     * Get the values of the algebraic variable at the given index.
     *
     * @param pIndex The index of the algebraic variable.
     *
     * @return The values of the algebraic variable, as a @c Doubles, if the index is valid, an empty vector otherwise.
     */

    Doubles algebraicVariable(size_t pIndex) const;

#ifdef __EMSCRIPTEN__
    emscripten::val algebraicVariableAsArray(size_t pIndex) const;
#endif

    /**
     * @brief Get the name of the algebraic variable.
     *
     * Get the name of the algebraic variable at the given index.
     *
     * @param pIndex The index of the algebraic variable.
     *
     * @return The name of the algebraic variable, as a @c std::string, if the index is valid, an empty string
     * otherwise.
     */

    std::string algebraicVariableName(size_t pIndex) const;

    /**
     * @brief Get the unit of the algebraic variable.
     *
     * Get the unit of the algebraic variable at the given index.
     *
     * @param pIndex The index of the algebraic variable.
     *
     * @return The unit of the algebraic variable, as a @c std::string, if the index is valid, an empty string
     * otherwise.
     */

    std::string algebraicVariableUnit(size_t pIndex) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedInstanceTask(const SedAbstractTaskPtr &pTask); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

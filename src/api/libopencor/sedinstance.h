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
     */

    void run();

    //---GRY--- THE BELOW METHODS ARE ONLY HERE SO THAT WE CAN QUICKLY DEMONSTRATE HOW THINGS CAN WORK FROM JavaScript.

    Doubles voi() const; /**< Get the values of the variable of integration. */
    Doubles state(size_t pIndex) const; /**< Get the values of the state. */
    Doubles rate(size_t pIndex) const; /**< Get the values of the rate. */
    Doubles variable(size_t pIndex) const; /**< Get the values of the variable. */

    std::string voiName() const; /**< Get the name of the variable of integration. */
    std::string stateName(size_t pIndex) const; /**< Get the name of the state. */
    std::string rateName(size_t pIndex) const; /**< Get the name of the rate. */
    std::string variableName(size_t pIndex) const; /**< Get the name of the variable. */

    size_t stateCount() const; /**< Get the size of the state. */
    size_t variableCount() const; /**< Get the size of the variable. */

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedInstance(const SedDocumentPtr &pDocument, bool pCompiled); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

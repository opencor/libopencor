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

#include "libopencor/export.h"

namespace libOpenCOR {

class SolverProperty;
using SolverPropertyPtr = std::shared_ptr<SolverProperty>;

using SolverPropertyValue = std::variant<bool, int, double, std::string>;

class SolverProperty
{
public:
    enum class Type
    {
        Boolean,
        Integer,
        IntegerGt0,
        IntegerGe0,
        Double,
        DoubleGe0,
        DoubleGt0,
        List
    };

    SolverProperty() = delete;
    ~SolverProperty();

    SolverProperty(const SolverProperty &pOther) = delete;
    SolverProperty(SolverProperty &&pOther) noexcept = delete;

    SolverProperty &operator=(const SolverProperty &pRhs) = delete;
    SolverProperty &operator=(SolverProperty &&pRhs) noexcept = delete;

    static SolverPropertyPtr create(Type pType, const std::string &pName, const std::vector<std::string> &pListValues,
                                    const SolverPropertyValue &pDefaultValue, bool pHasVoiValue);

    Type type() const;
    std::string name() const;
    SolverPropertyValue value() const;
    std::vector<std::string> listValues() const;
    SolverPropertyValue defaultValue() const;
    bool hasVoiUnit() const;

private:
    class Impl;

    Impl *mPimpl;

    explicit SolverProperty(Type pType, const std::string &pName, const std::vector<std::string> &pListValues,
                            const SolverPropertyValue &pDefaultValue, bool pHasVoiValue);
};

} // namespace libOpenCOR

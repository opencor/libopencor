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

#include "logger_p.h"
#include "utils.h"

#include "libopencor/solver.h"
#include "libopencor/solverinfo.h"
#include "libopencor/solverproperty.h"

namespace libOpenCOR {

class Solver::Impl: public Logger::Impl
{
public:
    static StringSizetMap SolversIndex;
    static StringStringMap SolversId;
    static StringSolverCreateMap SolversCreate;
    static SolverInfoPtrVector SolversInfo;

    StringStringMap mProperties;

    static void registerSolvers();
    static void registerSolver(Type pType, const std::string &pId, const std::string &pName, SolverCreate pCreate,
                               const SolverPropertyPtrVector &pProperties,
                               SolverInfo::HiddenProperties pHiddenProperties);
    static SolverPropertyPtr createProperty(SolverProperty::Type pType, const std::string &pId,
                                            const std::string &pName, const StringVector &pListValues,
                                            const std::string &pDefaultValue, bool pHasVoiUnit);
    static std::string valueFromProperties(const std::string &pId, const std::string &pName,
                                           const StringStringMap &pProperties);

    virtual ~Impl() = default;

    virtual StringStringMap propertiesId() const = 0;

    std::string id(const std::string &pIdOrName) const;

    std::string property(const std::string &pIdOrName);
    void setProperty(const std::string &pIdOrName, const std::string &pValue);

    StringStringMap properties() const;
    void setProperties(const StringStringMap &pProperties);
};

} // namespace libOpenCOR

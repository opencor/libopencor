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

#include "libopencor/solver.h"
#include "libopencor/solverproperty.h"

namespace libOpenCOR {

using SolverOdeCreate = SolverOdePtr (*)();

class Solver::Impl: public Logger::Impl
{
public:
    static std::map<std::string, std::string> SolversKisaoId;
    static std::map<std::string, SolverOdeCreate> SolversCreateOde;
    static std::vector<SolverInfoPtr> SolversInfo;

    bool mIsValid = false;
    std::map<std::string, std::string> mProperties;

    static void registerSolverOde(Type pType, const std::string &pName, const std::string &pKisaoId, SolverOdeCreate pCreateOde,
                                  const std::vector<SolverPropertyPtr> &pProperties);
    static SolverPropertyPtr createProperty(SolverProperty::Type pType, const std::string &pName,
                                            const std::string &pKisaoId, const std::vector<std::string> &pListValues,
                                            const std::string &pDefaultValue, bool pHasVoiValue);

    virtual ~Impl() = default;

    std::string kisaoId(const std::string &pNameOrKisaoId) const;

    std::string property(const std::string &pNameOrKisaoId);
    void setProperty(const std::string &pNameOrKisaoId, const std::string &pValue);

    std::map<std::string, std::string> properties() const;
    void setProperties(const std::map<std::string, std::string> &pProperties);

    virtual std::map<std::string, std::string> propertiesKisaoId() const = 0;
};

} // namespace libOpenCOR

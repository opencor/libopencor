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

#include "libopencor/solverinfo.h"

#include <string>
#include <vector>

namespace libOpenCOR {

class SolverInfo::Impl
{
public:
    Solver::Type mType = Solver::Type::ODE;
    std::string mName;
    std::vector<SolverPropertyPtr> mProperties;

    static SolverInfoPtr create(Solver::Type pType, const std::string &pName, const std::vector<SolverPropertyPtr> &pProperties);

    explicit Impl(Solver::Type pType, const std::string &pName, const std::vector<SolverPropertyPtr> &pProperties);
};

} // namespace libOpenCOR

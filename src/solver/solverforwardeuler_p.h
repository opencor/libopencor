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

#include "solver_p.h"
#include "solverforwardeuler.h"

namespace libOpenCOR {

class SolverForwardEuler::Impl: public Solver::Impl
{
public:
    static const Solver::Type Type = Solver::Type::ODE;
    static const std::string Name;
    static const std::string KisaoId;

    static const std::string StepName;
    static const std::string StepKisaoId;
    static constexpr double StepDefaultValue = 1.0;

    static SolverPtr create();
    static std::vector<SolverPropertyPtr> propertiesInfo();

    explicit Impl();

    std::map<std::string, std::string> propertiesKisaoId() const override;
};

} // namespace libOpenCOR

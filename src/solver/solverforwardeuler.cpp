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

#include "solverforwardeuler_p.h"

namespace libOpenCOR {

const std::string SolverForwardEuler::Impl::Name = "Forward Euler"; // NOLINT
const std::string SolverForwardEuler::Impl::KisaoId = "KISAO:0000030"; // NOLINT

const std::string SolverForwardEuler::Impl::StepName = "Step"; // NOLINT
const std::string SolverForwardEuler::Impl::StepKisaoId = "KISAO:0000483"; // NOLINT

SolverPtr SolverForwardEuler::Impl::create()
{
    return std::shared_ptr<SolverForwardEuler> {new SolverForwardEuler {}};
}

std::vector<SolverPropertyPtr> SolverForwardEuler::Impl::propertiesInfo()
{
    return {
        Solver::Impl::createProperty(SolverProperty::Type::DoubleGt0, StepName, StepKisaoId,
                                     {},
                                     std::to_string(StepDefaultValue),
                                     true),
    };
}

SolverForwardEuler::Impl::Impl()
    : SolverOde::Impl()
{
    mIsValid = true;

    mProperties[StepKisaoId] = std::to_string(StepDefaultValue);
}

std::map<std::string, std::string> SolverForwardEuler::Impl::propertiesKisaoId() const
{
    static const std::map<std::string, std::string> PropertiesKisaoId = {
        {StepName, StepKisaoId},
    };

    return PropertiesKisaoId;
}

SolverForwardEuler::SolverForwardEuler()
    : SolverOde(new Impl())
{
}

SolverForwardEuler::~SolverForwardEuler()
{
    delete pimpl();
}

SolverForwardEuler::Impl *SolverForwardEuler::pimpl()
{
    return static_cast<Impl *>(SolverOde::pimpl());
}

/*---GRY---
const SolverForwardEuler::Impl *SolverForwardEuler::pimpl() const
{
    return static_cast<const Impl *>(SolverOde::pimpl());
}
*/

} // namespace libOpenCOR

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

#include "solver_p.h"
#include "solverforwardeuler_p.h"
#include "solverinfo_p.h"
#include "solverunknown_p.h"

namespace libOpenCOR {

std::map<std::string, std::string> Solver::Impl::SolversKisaoId; // NOLINT
std::map<std::string, SolverCreate> Solver::Impl::SolversCreate; // NOLINT
std::vector<SolverInfoPtr> Solver::Impl::SolversInfo; // NOLINT

void Solver::Impl::registerSolver(Type pType, const std::string &pName, const std::string &pKisaoId,
                                  SolverCreate pCreate, const std::vector<SolverPropertyPtr> &pProperties)
{
#ifndef COVERAGE_ENABLED
    if (auto iter = Solver::Impl::SolversCreate.find(pKisaoId); iter == Solver::Impl::SolversCreate.end()) {
#endif
        Solver::Impl::SolversKisaoId[pName] = pKisaoId;
        Solver::Impl::SolversCreate[pKisaoId] = pCreate;
        Solver::Impl::SolversInfo.push_back(SolverInfo::Impl::create(pType, pName, pKisaoId, pProperties));
#ifndef COVERAGE_ENABLED
    }
#endif
}

SolverPropertyPtr Solver::Impl::createProperty(SolverProperty::Type pType, const std::string &pName,
                                               const std::string &pKisaoId, const std::vector<std::string> &pListValues,
                                               const std::string &pDefaultValue, bool pHasVoiUnit)
{
    return SolverPropertyPtr {new SolverProperty(pType, pName, pKisaoId, pListValues, pDefaultValue, pHasVoiUnit)};
}

std::map<std::string, std::string> Solver::Impl::propertiesKisaoId() const
{
    return {};
}

std::string Solver::Impl::kisaoId(const std::string &pNameOrKisaoId) const
{
    auto kisaoIds = propertiesKisaoId();
    auto kisaoIdIter = kisaoIds.find(pNameOrKisaoId);

    return (kisaoIdIter != kisaoIds.end()) ? kisaoIdIter->second : pNameOrKisaoId;
}

std::string Solver::Impl::property(const std::string &pNameOrKisaoId)
{
    return mProperties[kisaoId(pNameOrKisaoId)];
}

void Solver::Impl::setProperty(const std::string &pNameOrKisaoId, const std::string &pValue)
{
    auto kisaoId = this->kisaoId(pNameOrKisaoId);

    if (mProperties.find(kisaoId) != mProperties.end()) {
        mProperties[kisaoId] = pValue;
    }
}

std::map<std::string, std::string> Solver::Impl::properties() const
{
    return mProperties;
}

void Solver::Impl::setProperties(const std::map<std::string, std::string> &pProperties)
{
    mProperties = pProperties;
}

Solver::Solver(Impl *pPimpl)
    : Logger(pPimpl)
{
}

Solver::Impl *Solver::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const Solver::Impl *Solver::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

SolverPtr Solver::create(const std::string &pNameOrKisaoId)
{
    auto kisaoIdIter = Solver::Impl::SolversKisaoId.find(pNameOrKisaoId);
    auto kisaoId = (kisaoIdIter != Solver::Impl::SolversKisaoId.end()) ? kisaoIdIter->second : pNameOrKisaoId;

    if (auto iter = Solver::Impl::SolversCreate.find(kisaoId); iter != Solver::Impl::SolversCreate.end()) {
        return iter->second();
    }

    return SolverUnknown::Impl::create();
}

std::vector<SolverInfoPtr> Solver::solversInfo()
{
    static auto initialised = false;

    if (!initialised) {
        initialised = true;

        Solver::Impl::registerSolver(SolverForwardEuler::Impl::TYPE,
                                     SolverForwardEuler::Impl::NAME,
                                     SolverForwardEuler::Impl::KISAO_ID,
                                     SolverForwardEuler::Impl::create,
                                     SolverForwardEuler::Impl::propertiesInfo());
    }

    return Solver::Impl::SolversInfo;
}

bool Solver::isValid() const
{
    return pimpl()->mIsValid;
}

std::string Solver::property(const std::string &pNameOrKisaoId)
{
    return pimpl()->property(pNameOrKisaoId);
}

void Solver::setProperty(const std::string &pNameOrKisaoId, const std::string &pValue)
{
    pimpl()->setProperty(pNameOrKisaoId, pValue);
}

std::map<std::string, std::string> Solver::properties() const
{
    return pimpl()->properties();
}

void Solver::setProperties(const std::map<std::string, std::string> &pProperties)
{
    pimpl()->setProperties(pProperties);
}

} // namespace libOpenCOR

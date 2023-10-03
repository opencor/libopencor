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
#include "solvercvode_p.h"
#include "solverforwardeuler_p.h"
#include "solverfourthorderrungekutta_p.h"
#include "solverheun_p.h"
#include "solverinfo_p.h"
#include "solversecondorderrungekutta_p.h"
#include "solverunknown_p.h"

namespace libOpenCOR {

std::map<std::string, std::string> Solver::Impl::SolversId; // NOLINT
std::map<std::string, SolverCreate> Solver::Impl::SolversCreate; // NOLINT
std::vector<SolverInfoPtr> Solver::Impl::SolversInfo; // NOLINT

void Solver::Impl::registerSolver(Type pType, const std::string &pId, const std::string &pName,
                                  SolverCreate pCreate, const std::vector<SolverPropertyPtr> &pProperties,
                                  SolverInfo::HiddenPropertiesFunction pHiddenProperties)
{
#ifndef COVERAGE_ENABLED
    if (auto iter = Solver::Impl::SolversCreate.find(pId); iter == Solver::Impl::SolversCreate.end()) {
#endif
        Solver::Impl::SolversId[pName] = pId;
        Solver::Impl::SolversCreate[pId] = pCreate;
        Solver::Impl::SolversInfo.push_back(SolverInfo::Impl::create(pType, pId, pName, pProperties, pHiddenProperties));
#ifndef COVERAGE_ENABLED
    }
#endif
}

SolverPropertyPtr Solver::Impl::createProperty(SolverProperty::Type pType, const std::string &pId,
                                               const std::string &pName, const std::vector<std::string> &pListValues,
                                               const std::string &pDefaultValue, bool pHasVoiUnit)
{
    return SolverPropertyPtr {new SolverProperty(pType, pId, pName, pListValues, pDefaultValue, pHasVoiUnit)};
}

std::map<std::string, std::string> Solver::Impl::propertiesId() const
{
    return {};
}

std::string Solver::Impl::id(const std::string &pIdOrName) const
{
    auto ids = propertiesId();
    auto idIter = ids.find(pIdOrName);

    return (idIter != ids.end()) ? idIter->second : pIdOrName;
}

std::string Solver::Impl::property(const std::string &pIdOrName)
{
    auto id = this->id(pIdOrName);

    if (mProperties.find(id) != mProperties.end()) {
        return mProperties[id];
    }

    return {};
}

void Solver::Impl::setProperty(const std::string &pIdOrName, const std::string &pValue)
{
    auto id = this->id(pIdOrName);

    if (mProperties.find(id) != mProperties.end()) {
        mProperties[id] = pValue;
    }
}

std::map<std::string, std::string> Solver::Impl::properties() const
{
    return mProperties;
}

void Solver::Impl::setProperties(const std::map<std::string, std::string> &pProperties)
{
    for (const auto &property : pProperties) {
        setProperty(property.first, property.second);
    }
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

SolverPtr Solver::create(const std::string &pIdOrName)
{
    auto idIter = Solver::Impl::SolversId.find(pIdOrName);
    auto id = (idIter != Solver::Impl::SolversId.end()) ? idIter->second : pIdOrName;

    if (auto iter = Solver::Impl::SolversCreate.find(id); iter != Solver::Impl::SolversCreate.end()) {
        return iter->second();
    }

    return SolverUnknown::Impl::create();
}

std::vector<SolverInfoPtr> Solver::solversInfo()
{
    static auto initialised = false;

    if (!initialised) {
        initialised = true;

        Solver::Impl::registerSolver(SolverCvode::Impl::TYPE,
                                     SolverCvode::Impl::ID,
                                     SolverCvode::Impl::NAME,
                                     SolverCvode::Impl::create,
                                     SolverCvode::Impl::propertiesInfo(),
                                     SolverCvode::Impl::hiddenProperties);

        Solver::Impl::registerSolver(SolverForwardEuler::Impl::TYPE,
                                     SolverForwardEuler::Impl::ID,
                                     SolverForwardEuler::Impl::NAME,
                                     SolverForwardEuler::Impl::create,
                                     SolverForwardEuler::Impl::propertiesInfo(),
                                     SolverForwardEuler::Impl::hiddenProperties);

        Solver::Impl::registerSolver(SolverFourthOrderRungeKutta::Impl::TYPE,
                                     SolverFourthOrderRungeKutta::Impl::ID,
                                     SolverFourthOrderRungeKutta::Impl::NAME,
                                     SolverFourthOrderRungeKutta::Impl::create,
                                     SolverFourthOrderRungeKutta::Impl::propertiesInfo(),
                                     SolverFourthOrderRungeKutta::Impl::hiddenProperties);

        Solver::Impl::registerSolver(SolverHeun::Impl::TYPE,
                                     SolverHeun::Impl::ID,
                                     SolverHeun::Impl::NAME,
                                     SolverHeun::Impl::create,
                                     SolverHeun::Impl::propertiesInfo(),
                                     SolverHeun::Impl::hiddenProperties);

        Solver::Impl::registerSolver(SolverSecondOrderRungeKutta::Impl::TYPE,
                                     SolverSecondOrderRungeKutta::Impl::ID,
                                     SolverSecondOrderRungeKutta::Impl::NAME,
                                     SolverSecondOrderRungeKutta::Impl::create,
                                     SolverSecondOrderRungeKutta::Impl::propertiesInfo(),
                                     SolverSecondOrderRungeKutta::Impl::hiddenProperties);
    }

    return Solver::Impl::SolversInfo;
}

bool Solver::isValid() const
{
    return pimpl()->mIsValid;
}

std::string Solver::property(const std::string &pIdOrName)
{
    return pimpl()->property(pIdOrName);
}

void Solver::setProperty(const std::string &pIdOrName, const std::string &pValue)
{
    pimpl()->setProperty(pIdOrName, pValue);
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

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

#include "sedchangeattribute_p.h"
#include "sedinstancetask_p.h"

#include "utils.h"

#include <memory>

namespace libOpenCOR {

SedChangeAttribute::Impl::Impl(const std::string &pComponentName, const std::string &pVariableName,
                               const std::string &pNewValue)
    : mComponentName(pComponentName)
    , mVariableName(pVariableName)
    , mNewValue(pNewValue)
{
    updateTarget();
}

void SedChangeAttribute::Impl::setTarget(const std::string &pTarget)
{
    mTarget = pTarget;
}

void SedChangeAttribute::Impl::updateTarget()
{
    setTarget("/cellml:model/cellml:component[@name='" + mComponentName + "']/cellml:variable[@name='" + mVariableName + "']");
}

const std::string &SedChangeAttribute::Impl::componentName() const
{
    return mComponentName;
}

void SedChangeAttribute::Impl::setComponentName(const std::string &pComponentName)
{
    mComponentName = pComponentName;

    updateTarget();
}

const std::string &SedChangeAttribute::Impl::variableName() const
{
    return mVariableName;
}

void SedChangeAttribute::Impl::setVariableName(const std::string &pVariableName)
{
    mVariableName = pVariableName;

    updateTarget();
}

const std::string &SedChangeAttribute::Impl::newValue() const
{
    return mNewValue;
}

void SedChangeAttribute::Impl::setNewValue(const std::string &pNewValue)
{
    mNewValue = pNewValue;
}

void SedChangeAttribute::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node {xmlNewNode(nullptr, toConstXmlCharPtr("changeAttribute"))};

    SedChange::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("newValue"), toConstXmlCharPtr(mNewValue));

    xmlAddChild(pNode, node);
}

void SedChangeAttribute::Impl::apply(SedInstanceTask::Impl *pInstanceTaskPimpl,
                                     const libcellml::AnalyserModelPtr &pAnalyserModel) const
{
    // Note: we report our warnings to the instance task rather than to ourselves. Indeed, we are owned by a model,
    //       which may be shared by several instance tasks, and our warnings would otherwise accumulate every time we
    //       get applied.

    auto addInstanceTaskWarning = [pInstanceTaskPimpl](const std::string &pWarning) {
        pInstanceTaskPimpl->addIssue(Issue::Type::WARNING, pWarning, "Change attribute");
    };
    auto addCannotChangeWarning = [&addInstanceTaskWarning](const std::string &pVariableName,
                                                            const std::string &pComponentName,
                                                            const char *pVariableType) {
        std::string warning;

        warning.reserve(pVariableName.size() + pComponentName.size() + 120); // NOLINT

        warning += "The ";
        warning += pVariableType;
        warning += " '";
        warning += pVariableName;
        warning += "' in component '";
        warning += pComponentName;
        warning += "' cannot be changed. Only state variables and constants can be changed.";

        addInstanceTaskWarning(warning);
    };

    auto changeName {name(mComponentName, mVariableName)};
    const auto stateCount = pInstanceTaskPimpl->stateCount();
    const auto constantCount = pInstanceTaskPimpl->constantCount();
    const auto computedConstantCount = pInstanceTaskPimpl->computedConstantCount();
    const auto algebraicVariableCount = pInstanceTaskPimpl->algebraicVariableCount();

    if (pInstanceTaskPimpl->voiName() == changeName) {
        auto voiVariable {pAnalyserModel->voi()->variable()};
        auto voiComponent {owningComponent(voiVariable)};

        addCannotChangeWarning(voiVariable->name(), voiComponent->name(), "variable of integration");

        return;
    }

    auto isParameterSet {false};

    for (size_t i {0}; i < stateCount; ++i) {
        if (pInstanceTaskPimpl->stateName(i) == changeName) {
            pInstanceTaskPimpl->mChanges.emplace_back(pInstanceTaskPimpl->mStates + i, toDouble(newValue())); // NOLINT

            isParameterSet = true;

            break;
        }
    }

    if (!isParameterSet) {
        for (size_t i {0}; i < constantCount; ++i) {
            if (pInstanceTaskPimpl->constantName(i) == changeName) {
                pInstanceTaskPimpl->mChanges.emplace_back(pInstanceTaskPimpl->mConstants + i, toDouble(newValue())); // NOLINT

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        const auto &computedConstants = pAnalyserModel->computedConstants();

        for (size_t i {0}; i < computedConstantCount; ++i) {
            if (pInstanceTaskPimpl->computedConstantName(i) == changeName) {
                auto computedConstantVariable {computedConstants[i]->variable()};
                auto computedConstantComponent {owningComponent(computedConstantVariable)};

                addCannotChangeWarning(computedConstantVariable->name(), computedConstantComponent->name(), "computed constant");

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        const auto &algebraicVariables = pAnalyserModel->algebraicVariables();

        for (size_t i {0}; i < algebraicVariableCount; ++i) {
            if (pInstanceTaskPimpl->algebraicVariableName(i) == changeName) {
                auto algebraicVariable {algebraicVariables[i]->variable()};
                auto algebraicComponent {owningComponent(algebraicVariable)};

                addCannotChangeWarning(algebraicVariable->name(), algebraicComponent->name(), "algebraic variable");

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        std::string warning;

        warning.reserve(mVariableName.size() + mComponentName.size() + 72); // NOLINT

        warning += "The variable '";
        warning += mVariableName;
        warning += "' in component '";
        warning += mComponentName;
        warning += "' could not be found and therefore could not be changed.";

        addInstanceTaskWarning(warning);
    }
}

SedChangeAttribute::SedChangeAttribute(const std::string &pComponentName, const std::string &pVariableName,
                                       const std::string &pNewValue)
    : SedChange(std::make_unique<Impl>(pComponentName, pVariableName, pNewValue))
{
}

SedChangeAttribute::~SedChangeAttribute() = default;

SedChangeAttribute::Impl *SedChangeAttribute::pimpl()
{
    return static_cast<Impl *>(SedChange::pimpl());
}

const SedChangeAttribute::Impl *SedChangeAttribute::pimpl() const
{
    return static_cast<const Impl *>(SedChange::pimpl());
}

SedChangeAttributePtr SedChangeAttribute::create(const std::string &pComponent, const std::string &pVariable,
                                                 const std::string &pNewValue)
{
    if (!isCellmlIdentifier(pComponent) || !isCellmlIdentifier(pVariable) || !isDouble(pNewValue)) {
        return nullptr;
    }

    return SedChangeAttributePtr(new SedChangeAttribute(pComponent, pVariable, pNewValue));
}

const std::string &SedChangeAttribute::componentName() const
{
    return pimpl()->componentName();
}

bool SedChangeAttribute::setComponentName(const std::string &pComponentName)
{
    if (!isCellmlIdentifier(pComponentName)) {
        return false;
    }

    pimpl()->setComponentName(pComponentName);

    return true;
}

const std::string &SedChangeAttribute::variableName() const
{
    return pimpl()->variableName();
}

bool SedChangeAttribute::setVariableName(const std::string &pVariableName)
{
    if (!isCellmlIdentifier(pVariableName)) {
        return false;
    }

    pimpl()->setVariableName(pVariableName);

    return true;
}

const std::string &SedChangeAttribute::newValue() const
{
    return pimpl()->newValue();
}

bool SedChangeAttribute::setNewValue(const std::string &pNewValue)
{
    if (!isDouble(pNewValue)) {
        return false;
    }

    pimpl()->setNewValue(pNewValue);

    return true;
}

} // namespace libOpenCOR

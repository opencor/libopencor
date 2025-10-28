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

std::string SedChangeAttribute::Impl::componentName() const
{
    return mComponentName;
}

void SedChangeAttribute::Impl::setComponentName(const std::string &pComponentName)
{
    mComponentName = pComponentName;

    updateTarget();
}

std::string SedChangeAttribute::Impl::variableName() const
{
    return mVariableName;
}

void SedChangeAttribute::Impl::setVariableName(const std::string &pVariableName)
{
    mVariableName = pVariableName;

    updateTarget();
}

std::string SedChangeAttribute::Impl::newValue() const
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

void SedChangeAttribute::Impl::apply(const SedInstanceTaskPtr &pInstanceTask,
                                     const libcellml::AnalyserModelPtr &pAnalyserModel)
{
    auto *instanceTaskPimpl {pInstanceTask->pimpl()};
    auto changeName {name(mComponentName, mVariableName)};

    if (instanceTaskPimpl->voiName() == changeName) {
        auto voiVariable {pAnalyserModel->voi()->variable()};
        auto voiComponent {owningComponent(voiVariable)};

        addWarning(std::string("The variable of integration '").append(voiVariable->name()).append("' in component '").append(voiComponent->name()).append("'cannot be changed. Only state variables and constants can be changed."));

        return;
    }

    auto isParameterSet {false};

    for (size_t i {0}; i < pAnalyserModel->stateCount(); ++i) {
        if (instanceTaskPimpl->stateName(i) == changeName) {
            instanceTaskPimpl->mStates[i] = toDouble(newValue()); // NOLINT

            isParameterSet = true;

            break;
        }
    }

    if (!isParameterSet) {
        for (size_t i {0}; i < pAnalyserModel->constantCount(); ++i) {
            if (instanceTaskPimpl->constantName(i) == changeName) {
                instanceTaskPimpl->mConstants[i] = toDouble(newValue()); // NOLINT

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        for (size_t i {0}; i < pAnalyserModel->computedConstantCount(); ++i) {
            if (instanceTaskPimpl->computedConstantName(i) == changeName) {
                auto computedConstantVariable {pAnalyserModel->computedConstants()[i]->variable()};
                auto computedConstantComponent {owningComponent(computedConstantVariable)};

                addWarning(std::string("The computed constant '").append(computedConstantVariable->name()).append("' in component '").append(computedConstantComponent->name()).append("' cannot be changed. Only state variables and constants can be changed."));

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        for (size_t i {0}; i < pAnalyserModel->algebraicCount(); ++i) {
            if (instanceTaskPimpl->algebraicName(i) == changeName) {
                auto algebraicVariable {pAnalyserModel->algebraic()[i]->variable()};
                auto algebraicComponent {owningComponent(algebraicVariable)};

                addWarning(std::string("The algebraic variable '").append(algebraicVariable->name()).append("' in component '").append(algebraicComponent->name()).append("' cannot be changed. Only state variables and constants can be changed."));

                isParameterSet = true;

                break;
            }
        }
    }

    if (!isParameterSet) {
        addWarning(std::string("The variable '").append(mComponentName).append("' in component '").append(mVariableName).append("'could not be found and therefore could not be changed."));
    }
}

SedChangeAttribute::SedChangeAttribute(const std::string &pComponentName, const std::string &pVariableName,
                                       const std::string &pNewValue)
    : SedChange(new Impl(pComponentName, pVariableName, pNewValue))
{
}

SedChangeAttribute::~SedChangeAttribute()
{
    delete pimpl();
}

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
    return SedChangeAttributePtr(new SedChangeAttribute(pComponent, pVariable, pNewValue));
}

std::string SedChangeAttribute::componentName() const
{
    return pimpl()->componentName();
}

void SedChangeAttribute::setComponentName(const std::string &pComponentName)
{
    pimpl()->setComponentName(pComponentName);
}

std::string SedChangeAttribute::variableName() const
{
    return pimpl()->variableName();
}

void SedChangeAttribute::setVariableName(const std::string &pVariableName)
{
    pimpl()->setVariableName(pVariableName);
}

std::string SedChangeAttribute::newValue() const
{
    return pimpl()->newValue();
}

void SedChangeAttribute::setNewValue(const std::string &pNewValue)
{
    pimpl()->setNewValue(pNewValue);
}

} // namespace libOpenCOR

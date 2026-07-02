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

#include <algorithm>
#include <vector>

namespace libOpenCOR {

Solver::Impl::Impl(const std::string &pId, const std::string &pName)
    : mId(pId)
    , mName(pName)
{
}

const std::string &Solver::Impl::id() const
{
    return mId;
}

const std::string &Solver::Impl::name() const
{
    return mName;
}

void Solver::Impl::serialise(xmlNodePtr pNode, bool pNlaAlgorithm) const
{
    // Solver information.

    auto *algorithmNode {xmlNewNode(nullptr, toConstXmlCharPtr(pNlaAlgorithm ? "nlaAlgorithm" : "algorithm"))};

    if (pNlaAlgorithm) {
        xmlSetNs(algorithmNode, xmlNewNs(algorithmNode, toConstXmlCharPtr(LIBOPENCOR_NAMESPACE), nullptr));
    }

    xmlNewProp(algorithmNode, toConstXmlCharPtr("kisaoID"), toConstXmlCharPtr(mId));

    xmlAddChild(pNode, algorithmNode);

    // Solver properties information.

    auto *propertiesNode {xmlNewNode(nullptr, toConstXmlCharPtr("listOfAlgorithmParameters"))};

    xmlAddChild(algorithmNode, propertiesNode);

    auto props {properties()};
    std::vector<std::pair<std::string, std::string>> sortedProps(props.begin(), props.end());

    std::ranges::sort(sortedProps.begin(), sortedProps.end());

    for (const auto &[key, value] : sortedProps) {
        auto *propertyNode {xmlNewNode(nullptr, toConstXmlCharPtr("algorithmParameter"))};

        xmlNewProp(propertyNode, toConstXmlCharPtr("kisaoID"), toConstXmlCharPtr(key));
        xmlNewProp(propertyNode, toConstXmlCharPtr("value"), toConstXmlCharPtr(value));

        xmlAddChild(propertiesNode, propertyNode);
    }
}

Solver::Solver(Impl *pPimpl)
    : Logger(pPimpl)
{
}

Solver::Impl *Solver::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const Solver::Impl *Solver::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

const std::string &Solver::id() const
{
    return pimpl()->id();
}

const std::string &Solver::name() const
{
    return pimpl()->name();
}

} // namespace libOpenCOR

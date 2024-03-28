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

namespace libOpenCOR {

Solver::Impl::Impl(const std::string &pId, const std::string &pName)
    : mId(pId)
    , mName(pName)
{
}

void Solver::Impl::serialise(xmlNodePtr pNode, bool pNlaAlgorithm) const
{
    // Solver information.

    auto *algorithmNode = xmlNewNode(nullptr, toConstXmlCharPtr(pNlaAlgorithm ? "nlaAlgorithm" : "algorithm"));

    if (pNlaAlgorithm) {
        xmlSetNs(algorithmNode, xmlNewNs(algorithmNode, toConstXmlCharPtr(LIBOPENCOR_NAMESPACE), nullptr));
    }

    xmlNewProp(algorithmNode, toConstXmlCharPtr("kisaoID"), toConstXmlCharPtr(mId));

    xmlAddChild(pNode, algorithmNode);

    // Solver properties information.

    auto *propertiesNode = xmlNewNode(nullptr, toConstXmlCharPtr("listOfAlgorithmParameters"));

    xmlAddChild(algorithmNode, propertiesNode);

    for (auto const &property : properties()) {
        auto *propertyNode = xmlNewNode(nullptr, toConstXmlCharPtr("algorithmParameter"));

        xmlNewProp(propertyNode, toConstXmlCharPtr("kisaoID"), toConstXmlCharPtr(property.first));
        xmlNewProp(propertyNode, toConstXmlCharPtr("value"), toConstXmlCharPtr(property.second));

        xmlAddChild(propertiesNode, propertyNode);
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

std::string Solver::id() const
{
    return pimpl()->mId;
}

std::string Solver::name() const
{
    return pimpl()->mName;
}

} // namespace libOpenCOR

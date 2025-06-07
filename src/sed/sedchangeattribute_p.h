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

#include "sedchange_p.h"

#include "libopencor/sedchangeattribute.h"

namespace libOpenCOR {

class SedChangeAttribute::Impl: public SedChange::Impl
{
public:
    std::string mComponent;
    std::string mVariable;
    std::string mNewValue;

    explicit Impl(const std::string &pComponent, const std::string &pVariable, const std::string &pNewValue);

    void setTarget(const std::string &pTarget) override;
    void updateTarget();

    std::string component() const;
    void setComponent(const std::string &pComponent);

    std::string variable() const;
    void setVariable(const std::string &pVariable);

    std::string newValue() const;
    void setNewValue(const std::string &pNewValue);

    void serialise(xmlNodePtr pNode) const override;
};

} // namespace libOpenCOR

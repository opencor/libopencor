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

#include "logger_p.h"

#include "libopencor/sedbase.h"

#include <libxml/tree.h>

namespace libOpenCOR {

class SedBase::Impl: public Logger::Impl
{
public:
    std::string mId;
    std::string mName;
    std::string mMetaId;

    virtual ~Impl() = default;

    static const xmlChar *constXmlCharPtr(const std::string &pString);

    virtual void serialise(xmlNodePtr pNode, const std::string &pBasePath) const = 0;
};

} // namespace libOpenCOR
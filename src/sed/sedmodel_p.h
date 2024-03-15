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

#include "sedbase_p.h"

#include "libopencor/sedmodel.h"

namespace libOpenCOR {

class SedModel::Impl: public SedBase::Impl
{
public:
    FilePtr mFile;

    explicit Impl(const SedDocumentPtr &pDocument, const FilePtr &pFile);

    bool isValid();

#ifdef BUILDING_USING_CLANG
    // Prevent Clang from complaining about SedModel::Impl::serialise() hiding SedBase::Impl::serialise().

    using SedBase::Impl::serialise;
#endif

    void serialise(xmlNodePtr pNode, const std::string &pBasePath) const;
};

} // namespace libOpenCOR

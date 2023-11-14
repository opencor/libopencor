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

#include "libopencor/seddocument.h"

#include <unordered_set>

namespace libOpenCOR {

class SedDocument::Impl: public SedBase::Impl
{
public:
    std::string mXmlns = "http://sed-ml.org/sed-ml/level1/version4";
    size_t mLevel = 1;
    size_t mVersion = 4;

    std::unordered_set<std::string> mIds;

    std::vector<SedDataDescriptionPtr> mDataDescriptions;
    std::vector<SedModelPtr> mModels;
    std::vector<SedSimulationPtr> mSimulations;
    std::vector<SedAbstractTaskPtr> mTasks;
    std::vector<SedDataGeneratorPtr> mDataGenerators;
    std::vector<SedOutputPtr> mOutputs;
    std::vector<SedStylePtr> mStyles;

    std::string uniqueId(const std::string &pPrefix);

    void initialise(const FilePtr &pFile, const SedDocumentPtr &pOwner);
    void initialiseWithCellmlFile(const FilePtr &pFile, const SedDocumentPtr &pOwner);

#ifdef BUILDING_USING_CLANG
    // Prevent Clang from complaining about SedDocument::Impl::serialise() hiding SedBase::Impl::serialise().

    using SedBase::Impl::serialise;
#endif

    void serialise(xmlNodePtr pNode) const override;

    std::string serialise(const std::string &pBasePath) const;
};

} // namespace libOpenCOR

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

class SedDocument::Impl: public Logger::Impl
{
public:
    std::string mXmlns = "http://sed-ml.org/sed-ml/level1/version4";
    size_t mLevel = 1;
    size_t mVersion = 4;

    std::unordered_set<std::string> mIds;

    SedDataDescriptionPtrVector mDataDescriptions;
    SedModelPtrVector mModels;
    SedSimulationPtrVector mSimulations;
    SedAbstractTaskPtrVector mTasks;
    SedDataGeneratorPtrVector mDataGenerators;
    SedOutputPtrVector mOutputs;
    SedStylePtrVector mStyles;

    std::string uniqueId(const std::string &pPrefix);

    void initialise(const SedDocumentPtr &pOwner, const FilePtr &pFile);
    void initialiseFromCellmlFile(const SedDocumentPtr &pOwner, const FilePtr &pFile);

    void serialise(xmlNodePtr pNode) const;

    std::string serialise(const std::string &pBasePath = {}) const;

    bool hasModels() const;
    bool addModel(const SedModelPtr &pModel);
    bool removeModel(const SedModelPtr &pModel);

    bool hasSimulations() const;
    bool addSimulation(const SedSimulationPtr &pSimulation);
    bool removeSimulation(const SedSimulationPtr &pSimulation);

    bool hasTasks() const;
    bool addTask(const SedAbstractTaskPtr &pTask);
    bool removeTask(const SedAbstractTaskPtr &pTask);

    void runTask(const SedJobPtr &pJob, const SedTaskPtr &pTask);
    SedJobPtr run();
};

} // namespace libOpenCOR

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

    SedDataDescriptionPtrs mDataDescriptions;
    SedModelPtrs mModels;
    SedSimulationPtrs mSimulations;
    SedAbstractTaskPtrs mTasks;
    SedDataGeneratorPtrs mDataGenerators;
    SedOutputPtrs mOutputs;
    SedStylePtrs mStyles;

    std::string uniqueId(const std::string &pPrefix);

    void initialise(const SedDocumentPtr &pOwner, const FilePtr &pFile);
    static void initialiseFromCellmlFile(const SedDocumentPtr &pOwner, const FilePtr &pFile);
    void initialiseFromSedmlFile(const SedDocumentPtr &pOwner, const FilePtr &pFile);
    void initialiseFromCombineArchive(const SedDocumentPtr &pOwner, const FilePtr &pFile);

    void serialise(xmlNodePtr pNode) const;

    std::string serialise(const std::string &pBasePath = {}) const;

    bool hasModels() const;
    size_t modelCount() const;
    SedModelPtrs models() const;
    SedModelPtr model(size_t pIndex) const;
    bool addModel(const SedModelPtr &pModel);
    bool removeModel(const SedModelPtr &pModel);
    bool removeAllModels();

    bool hasSimulations() const;
    size_t simulationCount() const;
    SedSimulationPtrs simulations() const;
    SedSimulationPtr simulation(size_t pIndex) const;
    bool addSimulation(const SedSimulationPtr &pSimulation);
    bool removeSimulation(const SedSimulationPtr &pSimulation);
    bool removeAllSimulations();

    bool hasTasks() const;
    size_t taskCount() const;
    SedAbstractTaskPtrs tasks() const;
    SedAbstractTaskPtr task(size_t pIndex) const;
    bool addTask(const SedAbstractTaskPtr &pTask);
    bool removeTask(const SedAbstractTaskPtr &pTask);
    bool removeAllTasks();
};

} // namespace libOpenCOR

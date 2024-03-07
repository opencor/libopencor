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

    std::vector<SedDataDescriptionPtr> mDataDescriptions;
    SedModelPtrVector mModels;
    SedSimulationPtrVector mSimulations;
    std::vector<SedAbstractTaskPtr> mTasks;
    std::vector<SedDataGeneratorPtr> mDataGenerators;
    std::vector<SedOutputPtr> mOutputs;
    std::vector<SedStylePtr> mStyles;

    double mVoi = 0.0; //---GRY--- THIS SHOULD BE MOVED TO A TASK OBJECT.
    double *mStates = nullptr; //---GRY--- THIS SHOULD BE MOVED TO A TASK OBJECT.
    double *mRates = nullptr; //---GRY--- THIS SHOULD BE MOVED TO A TASK OBJECT.
    double *mVariables = nullptr; //---GRY--- THIS SHOULD BE MOVED TO A TASK OBJECT.

    ~Impl();

    void resetArrays();

    std::string uniqueId(const std::string &pPrefix);

    void initialise(const FilePtr &pFile, const SedDocumentPtr &pOwner);
    void initialiseFromCellmlFile(const FilePtr &pFile, const SedDocumentPtr &pOwner);

    bool isValid();

    void serialise(xmlNodePtr pNode) const;

    std::string serialise(const std::string &pBasePath = {}) const;

    bool addModel(const SedModelPtr &pModel);
    bool removeModel(const SedModelPtr &pModel);

    bool addSimulation(const SedSimulationPtr &pSimulation);
    bool removeSimulation(const SedSimulationPtr &pSimulation);

    bool start();
};

} // namespace libOpenCOR
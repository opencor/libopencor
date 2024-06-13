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

#include "sedmlfile_p.h"

#include "filemanager.h"
#include "utils.h"

#include "libopencor/file.h"
#include "libopencor/sedanalysis.h"
#include "libopencor/sedonestep.h"
#include "libopencor/sedsteadystate.h"
#include "libopencor/seduniformtimecourse.h"

#include "libsedmlbegin.h"
#include <sedml/SedDocument.h>
#include <sedml/SedReader.h>
#include <sedml/SedUniformTimeCourse.h>
#include "libsedmlend.h"

#include <ranges>

namespace libOpenCOR {

SedmlFile::Impl::Impl(const FilePtr &pFile, libsedml::SedDocument *pDocument)
    : mLocation(pathToString(stringToPath(pFile->fileName()).parent_path()) + "/")
    , mDocument(pDocument)
{
}

SedmlFile::Impl::~Impl()
{
    delete mDocument;
}

std::vector<FilePtr> SedmlFile::Impl::models()
{
    // Retrieve the models.

    removeAllIssues();

    std::vector<FilePtr> res;
    auto fileManager = FileManager::instance();

    for (unsigned int i = 0; i < mDocument->getNumModels(); ++i) {
        auto source = mDocument->getModel(i)->getSource();
        auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(source);
        auto modelSource = (isLocalFile && stringToPath(fileNameOrUrl).is_relative()) ?
                               mLocation + fileNameOrUrl :
                               fileNameOrUrl;
        auto file = fileManager.file(modelSource);

        if (file != nullptr) {
            res.push_back(file);
        } else {
            addWarning("The model '" + source + "' could not be found. It has been automatically added, but it is empty.");

            res.push_back(File::create(modelSource));
        }
    }

    return res;
}

std::vector<SedSimulationPtr> SedmlFile::Impl::simulations(const SedDocumentPtr &pOwner) const
{
    // Retrieve the simulations.

    std::vector<SedSimulationPtr> res;

    for (unsigned int i = 0; i < mDocument->getNumSimulations(); ++i) {
        auto *sedSimulation = mDocument->getSimulation(i);

#ifndef CODE_COVERAGE_ENABLED
        if (sedSimulation->isSedUniformTimeCourse()) {
#endif
            auto *sedUniformTimeCourse = reinterpret_cast<libsedml::SedUniformTimeCourse *>(sedSimulation);
            auto uniformTimeCourse = SedUniformTimeCourse::create(pOwner);

            uniformTimeCourse->setInitialTime(sedUniformTimeCourse->getInitialTime());
            uniformTimeCourse->setOutputStartTime(sedUniformTimeCourse->getOutputStartTime());
            uniformTimeCourse->setOutputEndTime(sedUniformTimeCourse->getOutputEndTime());
            uniformTimeCourse->setNumberOfSteps(sedUniformTimeCourse->getNumberOfSteps());

            res.push_back(uniformTimeCourse);
#ifndef CODE_COVERAGE_ENABLED
        } else if (sedSimulation->isSedOneStep()) {
            auto *sedOneStep = reinterpret_cast<libsedml::SedOneStep *>(sedSimulation);
            auto oneStep = SedOneStep::create(pOwner);

            (void)sedOneStep;

            res.push_back(oneStep);
        } else if (sedSimulation->isSedSteadyState()) {
            auto *sedSteadyState = reinterpret_cast<libsedml::SedSteadyState *>(sedSimulation);
            auto steadyState = SedSteadyState::create(pOwner);

            (void)sedSteadyState;

            res.push_back(steadyState);
        } else { // SedAnalysis.
            auto *sedAnalysis = reinterpret_cast<libsedml::SedAnalysis *>(sedSimulation);
            auto analysis = SedAnalysis::create(pOwner);

            (void)sedAnalysis;

            res.push_back(analysis);
        }
#endif
    }

    return res;
}

SedmlFile::SedmlFile(const FilePtr &pFile, libsedml::SedDocument *pDocument)
    : Logger(new Impl {pFile, pDocument})
{
}

SedmlFile::~SedmlFile()
{
    delete pimpl();
}

SedmlFile::Impl *SedmlFile::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const SedmlFile::Impl *SedmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

SedmlFilePtr SedmlFile::create(const FilePtr &pFile)
{
    // Try to retrieve a SED-ML document.

    auto fileContents = pFile->contents();

    if (!fileContents.empty()) {
        auto *document = libsedml::readSedMLFromString(toString(fileContents).c_str());

        // A non-SED-ML file results in our SED-ML document having at least one error, the first of which being of id
        // libsedml::SedNotSchemaConformant (e.g., a CellML file, i.e. an XML file, but not a SED-ML one) or
        // XMLContentEmpty (e.g., a COMBINE archive, i.e. not an XML file). So, we use these facts to determine whether
        // our current SED-ML document is indeed a SED-ML file.

        if ((document->getNumErrors() == 0)
            || ((document->getError(0)->getErrorId() != libsedml::SedNotSchemaConformant)
                && (document->getError(0)->getErrorId() != libsbml::XMLContentEmpty))) {
            return SedmlFilePtr {new SedmlFile {pFile, document}};
        }

        delete document;
    }

    return nullptr;
}

std::vector<FilePtr> SedmlFile::models()
{
    return pimpl()->models();
}

std::vector<SedSimulationPtr> SedmlFile::simulations(const SedDocumentPtr &pOwner) const
{
    return pimpl()->simulations(pOwner);
}

} // namespace libOpenCOR

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

#include "solvernla_p.h"
#include "solverode_p.h"

#include "utils.h"

#include "libopencor/file.h"
#include "libopencor/filemanager.h"
#include "libopencor/sedanalysis.h"
#include "libopencor/seddocument.h"
#include "libopencor/sedmodel.h"
#include "libopencor/sedonestep.h"
#include "libopencor/sedsteadystate.h"
#include "libopencor/sedtask.h"
#include "libopencor/seduniformtimecourse.h"
#include "libopencor/solvercvode.h"
#include "libopencor/solverforwardeuler.h"
#include "libopencor/solverfourthorderrungekutta.h"
#include "libopencor/solverheun.h"
#include "libopencor/solverkinsol.h"
#include "libopencor/solversecondorderrungekutta.h"

#include "libsedmlbegin.h"
#include <sedml/SedDocument.h>
#include <sedml/SedReader.h>
#include <sedml/SedOneStep.h>
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

void SedmlFile::Impl::populateDocument(const SedDocumentPtr &pDocument)
{
    // Populate the models.

    removeAllIssues();

    auto fileManager = FileManager::instance();

    for (unsigned int i = 0; i < mDocument->getNumModels(); ++i) {
        auto source = mDocument->getModel(i)->getSource();
        auto [isLocalFile, fileNameOrUrl] = retrieveFileInfo(source);
        auto modelSource = (isLocalFile && stringToPath(fileNameOrUrl).is_relative()) ?
                               mLocation + fileNameOrUrl :
                               fileNameOrUrl;
        auto file = fileManager.file(modelSource);
        SedModelPtr model;

        if (file != nullptr) {
            model = SedModel::create(pDocument, file);
        } else {
            addWarning(std::string("The model '").append(source).append("' could not be found. It has been automatically added, but it is empty."));

            model = SedModel::create(pDocument, File::create(modelSource));
        }

        model->setId(mDocument->getModel(i)->getId());

        pDocument->addModel(model);
    }

    // Populate the simulations.

    for (unsigned int i = 0; i < mDocument->getNumSimulations(); ++i) {
        auto *sedSimulation = mDocument->getSimulation(i);
        SedSimulationPtr simulation;

        if (sedSimulation->isSedUniformTimeCourse()) {
            auto *sedUniformTimeCourse = reinterpret_cast<libsedml::SedUniformTimeCourse *>(sedSimulation);
            auto uniformTimeCourse = SedUniformTimeCourse::create(pDocument);

            uniformTimeCourse->setInitialTime(sedUniformTimeCourse->getInitialTime());
            uniformTimeCourse->setOutputStartTime(sedUniformTimeCourse->getOutputStartTime());
            uniformTimeCourse->setOutputEndTime(sedUniformTimeCourse->getOutputEndTime());
            uniformTimeCourse->setNumberOfSteps(sedUniformTimeCourse->getNumberOfSteps());

            simulation = uniformTimeCourse;
        } else if (sedSimulation->isSedOneStep()) {
            auto *sedOneStep = reinterpret_cast<libsedml::SedOneStep *>(sedSimulation);
            auto oneStep = SedOneStep::create(pDocument);

            oneStep->setStep(sedOneStep->getStep());

            simulation = oneStep;
        } else if (sedSimulation->isSedSteadyState()) {
            simulation = SedSteadyState::create(pDocument);
        } else { // SedAnalysis.
            simulation = SedAnalysis::create(pDocument);
        }

        simulation->setId(sedSimulation->getId());

        pDocument->addSimulation(simulation);

        // Populate the simulation's algorithm.

        auto *sedAlgorithm = sedSimulation->getAlgorithm();
        auto kisaoId = sedAlgorithm->getKisaoID();
        SolverOdePtr odeSolver = nullptr;
        SolverNlaPtr nlaSolver = nullptr;

        if (kisaoId == "KISAO:0000019") {
            odeSolver = SolverCvode::create();
        } else if (kisaoId == "KISAO:0000030") {
            odeSolver = SolverForwardEuler::create();
        } else if (kisaoId == "KISAO:0000032") {
            odeSolver = SolverFourthOrderRungeKutta::create();
        } else if (kisaoId == "KISAO:0000301") {
            odeSolver = SolverHeun::create();
        } else if (kisaoId == "KISAO:0000282") {
            nlaSolver = SolverKinsol::create();
        } else if (kisaoId == "KISAO:0000381") {
            odeSolver = SolverSecondOrderRungeKutta::create();
        } else {
            addWarning(std::string("The solver '").append(kisaoId).append("' is not recognised. The CVODE solver will be used instead."));

            odeSolver = SolverCvode::create();
        }

        if (odeSolver != nullptr) {
            odeSolver->pimpl()->populate(sedAlgorithm);

            addIssues(odeSolver);

            simulation->setOdeSolver(odeSolver);
        }

        if (nlaSolver != nullptr) {
            nlaSolver->pimpl()->populate(sedAlgorithm);

            addIssues(nlaSolver);

            simulation->setNlaSolver(nlaSolver);
        }
    }

    // Populate the tasks.
    //---GRY--- FOR NOW, WE CREATE ONE TASK IF WE HAVE ONE MODEL AND ONE SIMULATION. THIS IS SO THAT WE CAN MEET OUR
    //          ISAN DEADLINE.

#ifdef CODE_COVERAGE_ENABLED
    if (mDocument->getNumModels() == 1) {
#else
    if ((mDocument->getNumModels() == 1) && (mDocument->getNumSimulations() == 1)) {
#endif
        pDocument->addTask(SedTask::create(pDocument, pDocument->models()[0], pDocument->simulations()[0]));
    }
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

/*---GRY---
const SedmlFile::Impl *SedmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}
*/

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

void SedmlFile::populateDocument(const SedDocumentPtr &pDocument)
{
    pimpl()->populateDocument(pDocument);
}

} // namespace libOpenCOR

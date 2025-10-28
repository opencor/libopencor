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

#include "cellmlfile_p.h"
#include "file_p.h"

#include "utils.h"

#include "libopencor/seddocument.h"
#include "libopencor/sedmodel.h"
#include "libopencor/sedsteadystate.h"
#include "libopencor/sedtask.h"
#include "libopencor/seduniformtimecourse.h"

namespace libOpenCOR {

CellmlFile::Impl::Impl(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : mFile(pFile)
    , mModel(pModel)
{
    // Resolve imports and flatten the model, if needed and possible.

    if (mModel->hasUnresolvedImports()) {
        auto importer {libcellml::Importer::create(pStrict)};

        if (importer->resolveImports(mModel, pathToString(stringToPath(pFile->path()).parent_path()))) {
            mModel = importer->flattenModel(mModel);
        } else {
            addIssues(importer);
        }
    }

    // Analyse the model.
    // Note: we do this even if there are some errors (as a result of resolving imports). This is so that we can
    //       retrieve the analyser's issues if needed (e.g., when wanting to retrieve a runtime) and so that we don't
    //       have to test mAnalyserModel for nullity everywhere.

    mAnalyser->analyseModel(mModel);

    mAnalyserModel = mAnalyser->model();

    if (mAnalyser->errorCount() != 0) {
        addIssues(mAnalyser);
    }
}

void CellmlFile::Impl::populateDocument(const SedDocumentPtr &pDocument) const
{
    // Add a model.

    auto model {SedModel::create(pDocument, mFile.lock())};

    pDocument->addModel(model);

    // Add a uniform time course simulation in the case of an ODE/DAE model while a steady state simulation in the case
    // of an algebraic or NLA model.

    SedSimulationPtr simulation;
    auto type {mAnalyserModel->type()};

    if ((type == libcellml::AnalyserModel::Type::ODE)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation = SedUniformTimeCourse::create(pDocument);
    } else {
        simulation = SedSteadyState::create(pDocument);
    }

    pDocument->addSimulation(simulation);

    // Add the required solver(s) depending on the type of our model.

    if ((type == libcellml::AnalyserModel::Type::ODE)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation->setOdeSolver(SolverCvode::create());
    }

    if ((type == libcellml::AnalyserModel::Type::NLA)
        || (type == libcellml::AnalyserModel::Type::DAE)) {
        simulation->setNlaSolver(SolverKinsol::create());
    }

    // Add a task.

    pDocument->addTask(SedTask::create(pDocument, model, simulation));
}

libcellml::AnalyserModel::Type CellmlFile::Impl::type() const
{
    return mAnalyserModel->type();
}

libcellml::AnalyserPtr CellmlFile::Impl::analyser() const
{
    return mAnalyser;
}

libcellml::AnalyserModelPtr CellmlFile::Impl::analyserModel() const
{
    return mAnalyserModel;
}

CellmlFileRuntimePtr CellmlFile::Impl::runtime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    return CellmlFileRuntime::create(pCellmlFile, pNlaSolver);
}

CellmlFile::CellmlFile(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict)
    : Logger(new Impl {pFile, pModel, pStrict})
{
}

CellmlFile::~CellmlFile()
{
    delete pimpl();
}

CellmlFile::Impl *CellmlFile::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const CellmlFile::Impl *CellmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

CellmlFilePtr CellmlFile::create(const FilePtr &pFile)
{
    // Check whether the file is a CellML file and if so then return its CellmlFile object.

    if (pFile->pimpl()->type() == File::Type::CELLML_FILE) {
        return pFile->pimpl()->mCellmlFile;
    }

    // Check whether the type of the file is already known and if so then return since there is no need to parse the
    // file contents again.

    if (pFile->pimpl()->mTypeChecked) {
        return {};
    }

    // Try to parse the file contents as a CellML file, be it a CellML 1.x or a CellML 2.0 file.

    auto parser {libcellml::Parser::create(false)};
    auto model {parser->parseModel(toString(pFile->contents()))};

    if (parser->errorCount() == 0) {
        return CellmlFilePtr {new CellmlFile {pFile, model, false}};
    }

    return {};
}

void CellmlFile::populateDocument(const SedDocumentPtr &pDocument)
{
    pimpl()->populateDocument(pDocument);
}

libcellml::AnalyserModel::Type CellmlFile::type() const
{
    return pimpl()->type();
}

libcellml::AnalyserPtr CellmlFile::analyser() const
{
    return pimpl()->analyser();
}

libcellml::AnalyserModelPtr CellmlFile::analyserModel() const
{
    return pimpl()->analyserModel();
}

CellmlFileRuntimePtr CellmlFile::runtime(const SolverNlaPtr &pNlaSolver)
{
    return CellmlFile::Impl::runtime(shared_from_this(), pNlaSolver);
}

} // namespace libOpenCOR

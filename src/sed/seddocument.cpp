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

#include "file_p.h"
#include "seddocument_p.h"
#include "sedmodel_p.h"
#include "sedsimulation_p.h"

#include "utils.h"

#include "libopencor/file.h"
#include "libopencor/sedmodel.h"
#include "libopencor/sedsimulationsteadystate.h"
#include "libopencor/sedsimulationuniformtimecourse.h"
#include "libopencor/solvercvode.h"
#include "libopencor/solverkinsol.h"

#include <algorithm>
#include <sstream>

#include <libxml/tree.h>

//---GRY--- THE BELOW HEADER SHOULD BE REMOVED ONCE WE CAN RUN A SIMULATION THROUGH A TASK OBJECT.
#include "sedsimulationuniformtimecourse_p.h"

#define LIBOPENCOR_DEBUGGING

#ifdef LIBOPENCOR_DEBUGGING
namespace {

void outputDifferentialModelHeader(size_t pStatesCount, size_t pVariablesCount)
{
    printf("voi"); // NOLINT
    for (size_t i = 0; i < pStatesCount; ++i) {
        printf(",state_%d", static_cast<int>(i)); // NOLINT
    }
    for (size_t i = 0; i < pVariablesCount; ++i) {
        printf(",variable_%d", static_cast<int>(i)); // NOLINT
    }
    printf("\n"); // NOLINT
}

void outputDifferentialModel(double voi, double *pStates, double *pRates, double *pVariables, size_t pStatesCount, size_t pVariablesCount, bool pCsv)
{
    if (pCsv) {
        printf("%g", voi); // NOLINT
        for (size_t i = 0; i < pStatesCount; ++i) {
            printf(",%g", pStates[i]); // NOLINT
        }
        for (size_t i = 0; i < pVariablesCount; ++i) {
            printf(",%g", pVariables[i]); // NOLINT
        }
        printf("\n"); // NOLINT
    } else {
        printf("---------[%g]---------[BEGIN]\n", voi); // NOLINT
        for (size_t i = 0; i < pStatesCount; ++i) {
            printf(">>> STATES[%d]: %g\n", static_cast<int>(i), pStates[i]); // NOLINT
        }
        for (size_t i = 0; i < pStatesCount; ++i) {
            printf(">>> RATES[%d]: %g\n", static_cast<int>(i), pRates[i]); // NOLINT
        }
        for (size_t i = 0; i < pVariablesCount; ++i) {
            printf(">>> VARIABLES[%d]: %g\n", static_cast<int>(i), pVariables[i]); // NOLINT
        }
        printf("---------[%g]---------[END]\n", voi); // NOLINT
    }

    fflush(stdout); // NOLINT
}

void outputNonDifferentialModel(double *pVariables, size_t pVariablesCount)
{
    printf("---------------------------------------[BEGIN]\n"); // NOLINT
    for (size_t i = 0; i < pVariablesCount; ++i) {
        printf(">>> VARIABLES[%d]: %g\n", static_cast<int>(i), pVariables[i]); // NOLINT
    }
    printf("---------------------------------------[END]\n"); // NOLINT
    fflush(stdout); // NOLINT
}
#endif

} // namespace

namespace libOpenCOR {

SedDocument::Impl::~Impl()
{
    resetArrays();
}

void SedDocument::Impl::resetArrays()
{
    delete[] mStates;
    delete[] mRates;
    delete[] mVariables;

    mStates = nullptr;
    mRates = nullptr;
    mVariables = nullptr;
}

std::string SedDocument::Impl::uniqueId(const std::string &pPrefix)
{
    size_t counter = 0;
    std::stringstream stream;

    stream << pPrefix << ++counter;

    auto res = stream.str();

#ifndef CODE_COVERAGE_ENABLED //---GRY--- SHOULD BE REMOVED AT SOME POINT.
    while (mIds.contains(res)) {
        stream.str({});

        stream << pPrefix << ++counter;

        res = stream.str();
    }
#endif

    mIds.insert(res);

    return res;
}

void SedDocument::Impl::initialise(const FilePtr &pFile, const SedDocumentPtr &pOwner)
{
    // Check whether we were given a file.

    if (pFile == nullptr) {
        return;
    }

    // Make sure that the given file is supported.

    switch (pFile->type()) {
    case File::Type::UNKNOWN_FILE:
        addError("A simulation experiment description cannot be created using an unknown file.");

        break;
    case File::Type::CELLML_FILE:
        initialiseFromCellmlFile(pFile, pOwner);

        break;
    case File::Type::SEDML_FILE:
        addError("A simulation experiment description cannot (currently) be created using a SED-ML file.");

        break;
#ifdef __EMSCRIPTEN__
    default: // File::Type::COMBINE_ARCHIVE.
        addError("A simulation experiment description cannot (currently) be created using a COMBINE archive.");

        break;
#else
    case File::Type::COMBINE_ARCHIVE:
        addError("A simulation experiment description cannot (currently) be created using a COMBINE archive.");

        break;
    default: // File::Type::IRRETRIEVABLE_FILE.
        addError("A simulation experiment description cannot be created using an irretrievable file.");

        break;
#endif
    }
}

void SedDocument::Impl::initialiseFromCellmlFile(const FilePtr &pFile, const SedDocumentPtr &pOwner)
{
    // Add a model for the given CellML file.

    addModel(SedModel::create(pFile, pOwner));

    // Add a uniform time course simulation in the case of an ODE/DAE model while a steady state simulation in the case
    // of an algebraic or NLA model.

    SedSimulationPtr simulation;
    auto cellmlFileType = pFile->pimpl()->mCellmlFile->type();

    if ((cellmlFileType == libcellml::AnalyserModel::Type::ODE)
        || cellmlFileType == libcellml::AnalyserModel::Type::DAE) {
        simulation = SedSimulationUniformTimeCourse::create(pOwner);
    } else {
        simulation = SedSimulationSteadyState::create(pOwner);
    }

    addSimulation(simulation);

    // Add the required solver(s) depending on the type of our model.

    if (cellmlFileType == libcellml::AnalyserModel::Type::ODE) {
        simulation->setOdeSolver(SolverCvode::create());
    } else if (cellmlFileType == libcellml::AnalyserModel::Type::NLA) {
        simulation->setNlaSolver(SolverKinsol::create());
    } else if (cellmlFileType == libcellml::AnalyserModel::Type::DAE) {
        simulation->setOdeSolver(SolverCvode::create());
        simulation->setNlaSolver(SolverKinsol::create());
    }
}

bool SedDocument::Impl::isValid()
{
    // Make sure that we have one model and one simulation.

    if (mModels.size() != 1) {
        addError("A simulation experiment description must (currently) have exactly one model.");
    }

    if (mSimulations.size() != 1) {
        addError("A simulation experiment description must (currently) have exactly one simulation.");
    }

    // Make sure that all our models are valid.

    for (const auto &model : mModels) {
        if (!model->pimpl()->isValid()) {
            addIssues(model);
        }
    }

    // Simulations are always valid, so we don't need to check them, but for coverage reasons we do.

#ifdef CODE_COVERAGE_ENABLED
    for (const auto &simulation : mSimulations) {
        simulation->pimpl()->isValid();
    }
#endif

    //---GRY--- WE DON'T (CURRENTLY) SUPPORT ANY OF THE OTHER SED-ML CONCEPTS, HENCE WE CHECK A FEW MORE THINGS HERE.

    // Make sure that the simulation has the expected solver(s).

    if (mErrors.empty()) {
        auto model = mModels[0];
        auto simulation = mSimulations[0];
        auto cellmlFileType = model->pimpl()->mFile->pimpl()->mCellmlFile->type();

        if ((cellmlFileType == libcellml::AnalyserModel::Type::ODE)
            && (simulation->odeSolver() == nullptr)) {
            addError("The simulation is to be linked to an ODE model and must therefore specify an ODE solver.");
        } else if ((cellmlFileType == libcellml::AnalyserModel::Type::NLA)
                   && (simulation->nlaSolver() == nullptr)) {
            addError("The simulation is to be linked to an NLA model and must therefore specify an NLA solver.");
        } else if (cellmlFileType == libcellml::AnalyserModel::Type::DAE) {
            if (simulation->odeSolver() == nullptr) {
                addError("The simulation is to be linked to a DAE model and must therefore specify an ODE solver.");
            }

            if (simulation->nlaSolver() == nullptr) {
                addError("The simulation is to be linked to a DAE model and must therefore specify an NLA solver.");
            }
        }
    }

    return mErrors.empty();
}

void SedDocument::Impl::serialise(xmlNodePtr pNode) const
{
    xmlNewProp(pNode, toConstXmlCharPtr("xmlns"), toConstXmlCharPtr(mXmlns));
    xmlNewProp(pNode, toConstXmlCharPtr("level"), toConstXmlCharPtr(std::to_string(mLevel)));
    xmlNewProp(pNode, toConstXmlCharPtr("version"), toConstXmlCharPtr(std::to_string(mVersion)));
}

std::string SedDocument::Impl::serialise(const std::string &pBasePath) const
{
    // Serialise our SED-ML document using libxml2.

    auto *doc = xmlNewDoc(toConstXmlCharPtr("1.0"));
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("sed"));

    serialise(node);

    xmlDocSetRootElement(doc, node);

    // Add the data descriptions, if any, to our SED-ML document.

    /*---GRY---
    if (!mDataDescriptions.empty()) {
        auto *sedListOfDataDescriptions = xmlNewNode(nullptr, toConstXmlCharPtr("listOfDataDescriptions"));

        xmlAddChild(node, sedListOfDataDescriptions);

        for (const auto &dataDescription : mDataDescriptions) {
            (void)dataDescription;
        }
    }
    */

    // Add the models, if any, to our SED-ML document.

    if (!mModels.empty()) {
        auto *sedListOfModels = xmlNewNode(nullptr, toConstXmlCharPtr("listOfModels"));

        xmlAddChild(node, sedListOfModels);

        for (const auto &model : mModels) {
            model->pimpl()->serialise(sedListOfModels, pBasePath);
        }
    }

    // Add the simulations, if any, to our SED-ML document.

    if (!mSimulations.empty()) {
        auto *sedListOfSimulations = xmlNewNode(nullptr, toConstXmlCharPtr("listOfSimulations"));

        for (const auto &simulation : mSimulations) {
            simulation->pimpl()->serialise(sedListOfSimulations);
        }

        xmlAddChild(node, sedListOfSimulations);
    }

    // Add the tasks, if any, to our SED-ML document.

    /*---GRY---
    if (!mTasks.empty()) {
        auto *sedListOfTasks = xmlNewNode(nullptr, toConstXmlCharPtr("listOfTasks"));

        xmlAddChild(node, sedListOfTasks);

        for (const auto &task : mTasks) {
            (void)task;
        }
    }
    */

    // Add the data generators, if any, to our SED-ML document.

    /*---GRY---
    if (!mDataGenerators.empty()) {
        auto *sedListOfDataGenerators = xmlNewNode(nullptr, toConstXmlCharPtr("listOfDataGenerators"));

        xmlAddChild(node, sedListOfDataGenerators);

        for (const auto &dataGenerator : mDataGenerators) {
            (void)dataGenerator;
        }
    }
    */

    // Add the outputs, if any, to our SED-ML document.

    /*---GRY---
    if (!mOutputs.empty()) {
        auto *sedListOfOutputs = xmlNewNode(nullptr, toConstXmlCharPtr("listOfOutputs"));

        for (const auto &output : mOutputs) {
            (void)output;
        }

        xmlAddChild(node, sedListOfOutputs);
    }
    */

    // Add the styles, if any, to our SED-ML document.

    /*---GRY---
    if (!mStyles.empty()) {
        auto *sedListOfStyles = xmlNewNode(nullptr, toConstXmlCharPtr("listOfStyles"));

        xmlAddChild(node, sedListOfStyles);

        for (const auto &style : mStyles) {
            (void)style;
        }
    }
    */

    // Convert our SED-ML document to a string and return it.

    xmlChar *buffer = nullptr;
    int size = 0;

    xmlDocDumpFormatMemoryEnc(doc, &buffer, &size, "UTF-8", 1);

    std::stringstream res;

    res << buffer;

    xmlFree(buffer);
    xmlFreeDoc(doc);

    return res.str();
}

bool SedDocument::Impl::addModel(const SedModelPtr &pModel)
{
    if (pModel == nullptr) {
        return false;
    }

    auto model = std::find_if(mModels.cbegin(), mModels.cend(), [&](const auto &s) {
        return s == pModel;
    });

    if (model != mModels.end()) {
        return false;
    }

    mModels.push_back(pModel);

    return true;
}

bool SedDocument::Impl::removeModel(const SedModelPtr &pModel)
{
    auto model = std::find_if(mModels.cbegin(), mModels.cend(), [&](const auto &s) {
        return s == pModel;
    });

    if (model != mModels.end()) {
        mIds.erase((*model)->pimpl()->mId);
        mModels.erase(model);

        return true;
    }

    return false;
}

bool SedDocument::Impl::addSimulation(const SedSimulationPtr &pSimulation)
{
    if (pSimulation == nullptr) {
        return false;
    }

    auto simulation = std::find_if(mSimulations.cbegin(), mSimulations.cend(), [&](const auto &s) {
        return s == pSimulation;
    });

    if (simulation != mSimulations.end()) {
        return false;
    }

    mSimulations.push_back(pSimulation);

    return true;
}

bool SedDocument::Impl::removeSimulation(const SedSimulationPtr &pSimulation)
{
    auto simulation = std::find_if(mSimulations.cbegin(), mSimulations.cend(), [&](const auto &s) {
        return s == pSimulation;
    });

    if (simulation != mSimulations.end()) {
        mIds.erase((*simulation)->pimpl()->mId);
        mSimulations.erase(simulation);

        return true;
    }

    return false;
}

bool SedDocument::Impl::start()
{
    removeAllIssues();

    // Make sure that we are valid.

    if (!isValid()) {
        return false;
    }

#ifndef __EMSCRIPTEN__
    // Get a runtime for the model.

    auto cellmlFile = mModels[0]->pimpl()->mFile->pimpl()->mCellmlFile;
    auto simulation = mSimulations[0];
    auto runtime = cellmlFile->runtime(simulation->nlaSolver());

#    ifndef CODE_COVERAGE_ENABLED
    if (!runtime->errors().empty()) {
        addIssues(runtime);

        return false;
    }
#    endif

    // Create our various arrays.

    resetArrays();

    auto cellmlFileType = cellmlFile->type();
    auto differentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                             || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);
    auto analyserModel = cellmlFile->analyserModel();

    if (differentialModel) {
        mStates = new double[analyserModel->stateCount()];
        mRates = new double[analyserModel->stateCount()];
        mVariables = new double[analyserModel->variableCount()];
    } else {
        mVariables = new double[analyserModel->variableCount()];
    }

    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.

    auto uniformTimeCourseSimulation = differentialModel ? dynamic_pointer_cast<SedSimulationUniformTimeCourse>(simulation) : nullptr;
    auto *uniformTimeCourseSimulationPimpl = differentialModel ? uniformTimeCourseSimulation->pimpl() : nullptr;

    if (differentialModel) {
        mVoi = uniformTimeCourseSimulationPimpl->mOutputStartTime;

        runtime->initialiseVariablesForDifferentialModel()(mStates, mRates, mVariables); // NOLINT
        runtime->computeComputedConstants()(mVariables); // NOLINT
        runtime->computeRates()(mVoi, mStates, mRates, mVariables); // NOLINT
        runtime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
    } else {
        runtime->initialiseVariablesForAlgebraicModel()(mVariables); // NOLINT
        runtime->computeComputedConstants()(mVariables); // NOLINT
        runtime->computeVariablesForAlgebraicModel()(mVariables); // NOLINT
    }

#    ifdef LIBOPENCOR_DEBUGGING
    static const auto CSV = true;

    if (differentialModel) {
        if (CSV) {
            outputDifferentialModelHeader(analyserModel->stateCount(), analyserModel->variableCount());
        }
        outputDifferentialModel(mVoi, mStates, mRates, mVariables, analyserModel->stateCount(), analyserModel->variableCount(), CSV);
    } else {
        outputNonDifferentialModel(mVariables, analyserModel->variableCount());
    }
#    endif

    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    if (differentialModel) {
        // Initialise the ODE solver.

        auto odeSolver = uniformTimeCourseSimulation->odeSolver();

        odeSolver->initialise(mVoi, analyserModel->stateCount(), mStates, mRates, mVariables, runtime->computeRates());

        // Compute the differential model.

        auto voiStart = mVoi;
        auto voiEnd = uniformTimeCourseSimulationPimpl->mOutputEndTime;
        auto voiInterval = (voiEnd - mVoi) / uniformTimeCourseSimulationPimpl->mNumberOfSteps;
        size_t voiCounter = 0;

        while (!fuzzyCompare(mVoi, voiEnd)) {
            if (!odeSolver->solve(mVoi, std::min(voiStart + static_cast<double>(++voiCounter) * voiInterval, voiEnd))) {
                addIssues(odeSolver);

                return false;
            }

            runtime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT

#    ifdef LIBOPENCOR_DEBUGGING
            outputDifferentialModel(mVoi, mStates, mRates, mVariables, analyserModel->stateCount(), analyserModel->variableCount(), CSV);
#    endif
        }
    }
#endif

    return true;
}

SedDocument::SedDocument()
    : Logger(new Impl {})
{
}

SedDocument::~SedDocument()
{
    delete pimpl();
}

SedDocument::Impl *SedDocument::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

const SedDocument::Impl *SedDocument::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}

SedDocumentPtr SedDocument::create(const FilePtr &pFile)
{
    auto res = SedDocumentPtr {new SedDocument {}};

    res->pimpl()->initialise(pFile, res);

    return res;
}

#ifdef __EMSCRIPTEN__
SedDocumentPtr SedDocument::defaultCreate()
{
    return create({});
}
#endif

std::string SedDocument::serialise() const
{
    return pimpl()->serialise();
}

std::string SedDocument::serialise(const std::string &pBasePath) const
{
    return pimpl()->serialise(pBasePath);
}

SedModelPtrVector SedDocument::models() const
{
    return pimpl()->mModels;
}

bool SedDocument::addModel(const SedModelPtr &pModel)
{
    return pimpl()->addModel(pModel);
}

bool SedDocument::removeModel(const SedModelPtr &pModel)
{
    return pimpl()->removeModel(pModel);
}

SedSimulationPtrVector SedDocument::simulations() const
{
    return pimpl()->mSimulations;
}

bool SedDocument::addSimulation(const SedSimulationPtr &pSimulation)
{
    return pimpl()->addSimulation(pSimulation);
}

bool SedDocument::removeSimulation(const SedSimulationPtr &pSimulation)
{
    return pimpl()->removeSimulation(pSimulation);
}

bool SedDocument::start()
{
    return pimpl()->start();
}

} // namespace libOpenCOR

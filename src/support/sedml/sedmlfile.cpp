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
#include "sedmlfile_p.h"
#include "solvernla_p.h"
#include "solverode_p.h"

#include "cellmlfile.h"
#include "utils.h"

#include "libopencor/filemanager.h"
#include "libopencor/sedanalysis.h"
#include "libopencor/sedchangeattribute.h"
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

#include "libxml/parser.h"
#include "libxml/tree.h"

#include "sedml/SedChangeAttribute.h"
#include "sedml/SedDocument.h"
#include "sedml/SedReader.h"
#include "sedml/SedOneStep.h"
#include "sedml/SedUniformTimeCourse.h"

#include <deque>
#include <ranges>

namespace libOpenCOR {

SedmlFile::Impl::Impl(const FilePtr &pFile, libsedml::SedDocument *pDocument)
    : mLocation(pathToString(stringToPath(pFile->url().empty() ?
                                              pFile->fileName() :
                                              pFile->url())
                                 .parent_path()))
    , mDocument(pDocument)
    , mContents(toString(pFile->contents()))
{
    if (!mLocation.empty()) {
        mLocation += "/";
    }
}

SedmlFile::Impl::~Impl()
{
    delete mDocument;
}

NlaSolverInfo SedmlFile::Impl::nlaSolver(const libsedml::SedSimulation *sedSimulation, bool hasNlaSolver) const
{
    SolverNlaPtr nlaSolver {nullptr};
    std::vector<std::string> warnings;
    auto *doc {xmlReadMemory(mContents.c_str(), static_cast<int>(mContents.size()), nullptr, nullptr, XML_PARSE_NONET)};
    auto *rootNode {xmlDocGetRootElement(doc)};
    auto hasLocalNlaAlgorithm {false};
    const auto &sedSimulationId {sedSimulation->getId()};
    std::deque<xmlNodePtr> nodes {rootNode};

    while (!nodes.empty()) {
        auto *node {nodes.front()};

        nodes.pop_front();

        auto *id {xmlGetProp(node, toConstXmlCharPtr("id"))};

        if (((xmlStrEqual(node->name, toConstXmlCharPtr("analysis")) != 0)
             || (xmlStrEqual(node->name, toConstXmlCharPtr("oneStep")) != 0)
             || (xmlStrEqual(node->name, toConstXmlCharPtr("steadyState")) != 0)
             || (xmlStrEqual(node->name, toConstXmlCharPtr("uniformTimeCourse")) != 0))
            && (id != nullptr) && (std::string(reinterpret_cast<const char *>(id)) == sedSimulationId)) {
            for (auto *child {node->children}; child != nullptr; child = child->next) {
                if ((child->type == XML_ELEMENT_NODE)
                    && (xmlStrEqual(child->name, toConstXmlCharPtr("nlaAlgorithm")) != 0)
                    && (xmlStrEqual(child->ns->href, toConstXmlCharPtr(LIBOPENCOR_NAMESPACE)) != 0)) {
                    if (hasNlaSolver) {
                        std::string warning;

                        warning.reserve(sedSimulationId.size() + 108); // NOLINT

                        warning += "The NLA solver is already set for the simulation '";
                        warning += sedSimulationId;
                        warning += "'. The one specified in the nlaAlgorithm element will be ignored.";

                        warnings.push_back(warning);

                        break;
                    }

                    if (hasLocalNlaAlgorithm) {
                        std::string warning;

                        warning.reserve(sedSimulationId.size() + 84); // NOLINT

                        warning += "Multiple nlaAlgorithm elements have been found for the simulation '";
                        warning += sedSimulationId;
                        warning += "'. The first one will be used.";

                        warnings.push_back(warning);

                        break;
                    }

                    hasLocalNlaAlgorithm = true;

                    auto *kisaoId {xmlGetProp(child, toConstXmlCharPtr("kisaoID"))};
                    const std::string kisaoIdValue((kisaoId != nullptr) ? reinterpret_cast<const char *>(kisaoId) : "");

                    if (kisaoId != nullptr) {
                        xmlFree(kisaoId);
                    }

                    if (kisaoIdValue != "KISAO:0000282") {
                        std::string warning;

                        warning.reserve(kisaoIdValue.size() + sedSimulationId.size() + 63); // NOLINT

                        warning += "The NLA solver '";
                        warning += kisaoIdValue;
                        warning += "' in simulation '";
                        warning += sedSimulationId;
                        warning += "' is not recognised. The KINSOL solver will be used instead.";

                        warnings.push_back(warning);
                    }

                    auto *algorithm {new libsedml::SedAlgorithm()};

                    for (auto *nlaAlgorithmChild {child->children}; nlaAlgorithmChild != nullptr; nlaAlgorithmChild = nlaAlgorithmChild->next) {
                        if ((nlaAlgorithmChild->type == XML_ELEMENT_NODE)
                            && (xmlStrEqual(nlaAlgorithmChild->name, toConstXmlCharPtr("listOfAlgorithmParameters")) != 0)
                            && (xmlStrEqual(nlaAlgorithmChild->ns->href, toConstXmlCharPtr(LIBOPENCOR_NAMESPACE)) != 0)) {
                            for (auto *algorithmParameterNode {nlaAlgorithmChild->children}; algorithmParameterNode != nullptr;
                                 algorithmParameterNode = algorithmParameterNode->next) {
                                if ((algorithmParameterNode->type == XML_ELEMENT_NODE)
                                    && (xmlStrEqual(algorithmParameterNode->name, toConstXmlCharPtr("algorithmParameter")) != 0)
                                    && (xmlStrEqual(algorithmParameterNode->ns->href, toConstXmlCharPtr(LIBOPENCOR_NAMESPACE)) != 0)) {
                                    auto *algorithmParameterKisaoId {xmlGetProp(algorithmParameterNode, toConstXmlCharPtr("kisaoID"))};
                                    auto *algorithmParameterValue {xmlGetProp(algorithmParameterNode, toConstXmlCharPtr("value"))};

                                    if ((algorithmParameterKisaoId != nullptr) && (algorithmParameterValue != nullptr)) {
                                        auto *algorithmParameter {algorithm->createAlgorithmParameter()};

                                        algorithmParameter->setKisaoID(reinterpret_cast<const char *>(algorithmParameterKisaoId));
                                        algorithmParameter->setValue(reinterpret_cast<const char *>(algorithmParameterValue));
                                    }

                                    if (algorithmParameterKisaoId != nullptr) {
                                        xmlFree(algorithmParameterKisaoId);
                                    }

                                    if (algorithmParameterValue != nullptr) {
                                        xmlFree(algorithmParameterValue);
                                    }
                                }
                            }
                        }
                    }

                    nlaSolver = SolverKinsol::create();

                    nlaSolver->pimpl()->populate(algorithm);

                    delete algorithm;
                }
            }

            xmlFree(id);

            nodes.clear();

            continue;
        }

        if (id != nullptr) {
            xmlFree(id);
        }

        for (auto *child {node->children}; child != nullptr; child = child->next) {
            if (child->type == XML_ELEMENT_NODE) {
                nodes.push_back(child);
            }
        }
    }

    xmlFreeDoc(doc);

    return {nlaSolver, warnings};
}

NlaSolverInfo SedmlFile::Impl::legacyNlaSolver(const libsedml::SedSimulation *sedSimulation, bool hasNlaSolver) const // NOLINT
{
    static constexpr auto OPENCOR_NAMESPACE {"http://www.opencor.ws/"};

    SolverNlaPtr nlaSolver {nullptr};
    std::vector<std::string> warnings;
    const auto *annotation {sedSimulation->getAnnotation()};

    if (annotation != nullptr) {
        auto hasLocalNlaSolver {false};
        const auto &sedSimulationId {sedSimulation->getId()};

        for (unsigned int i {0}; i < annotation->getNumChildren(); ++i) {
            const auto &nlaSolverNode {annotation->getChild(i)};

            if ((nlaSolverNode.getURI() == OPENCOR_NAMESPACE)
                && (nlaSolverNode.getName() == "nlaSolver")) {
                const auto &nlaSolverName {nlaSolverNode.getAttrValue("name")};

                if (hasNlaSolver) {
                    std::string warning;

                    warning.reserve(nlaSolverName.size() + sedSimulationId.size() + 80); // NOLINT

                    warning += "The NLA solver '";
                    warning += nlaSolverName;
                    warning += "' is specified in both the algorithm element and in the annotation of the simulation '";
                    warning += sedSimulationId;
                    warning += "'. The one specified in the algorithm element will be used.";

                    warnings.push_back(warning);

                    break;
                }

                if (hasLocalNlaSolver) {
                    std::string warning;

                    warning.reserve(sedSimulationId.size() + 80); // NOLINT

                    warning += "Multiple NLA solvers have been found in the annotation of the simulation '";
                    warning += sedSimulationId;
                    warning += "'. The first one will be used.";

                    warnings.push_back(warning);

                    break;
                }

                hasLocalNlaSolver = true;

                if (nlaSolverName != "KINSOL") {
                    std::string warning;

                    warning.reserve(nlaSolverName.size() + sedSimulationId.size() + 63); // NOLINT

                    warning += "The NLA solver '";
                    warning += nlaSolverName;
                    warning += "' in simulation '";
                    warning += sedSimulationId;
                    warning += "' is not recognised. The KINSOL solver will be used instead.";

                    warnings.push_back(warning);
                }

                auto *algorithm {new libsedml::SedAlgorithm()};

                for (unsigned int j {0}; j < nlaSolverNode.getNumChildren(); ++j) {
                    const auto &nlaSolverPropertyNode {nlaSolverNode.getChild(j)};

                    // Convert the properties to a pointer to libsedml::SedAlgorithm so that we can then call SolverKinsol::Impl::populate() on it.

                    if ((nlaSolverPropertyNode.getURI() == OPENCOR_NAMESPACE)
                        && (nlaSolverPropertyNode.getName() == "solverProperty")) {
                        const auto &id {nlaSolverPropertyNode.getAttrValue("id")};
                        const auto &value {nlaSolverPropertyNode.getAttrValue("value")};
                        std::string algorithmParameterKisaoId;
                        std::string algorithmParameterValue;

                        if ((id == "MaximumNumberOfIterations")) {
                            algorithmParameterKisaoId = "KISAO:0000486";
                            algorithmParameterValue = value;
                        } else if (id == "LinearSolver") {
                            algorithmParameterKisaoId = "KISAO:0000477";
                            algorithmParameterValue = value;
                        } else if (id == "UpperHalfBandwidth") {
                            algorithmParameterKisaoId = "KISAO:0000479";
                            algorithmParameterValue = value;
                        } else if (id == "LowerHalfBandwidth") {
                            algorithmParameterKisaoId = "KISAO:0000480";
                            algorithmParameterValue = value;
                        } else {
                            std::string warning;

                            warning.reserve(id.size() + 63); // NOLINT

                            warning += "The NLA solver property '";
                            warning += id;
                            warning += "' is not recognised. It will be ignored.";

                            warnings.push_back(warning);
                        }

                        if (!algorithmParameterKisaoId.empty()) {
                            auto *algorithmParameter {algorithm->createAlgorithmParameter()};

                            algorithmParameter->setKisaoID(algorithmParameterKisaoId);
                            algorithmParameter->setValue(algorithmParameterValue);
                        }
                    } else {
                        std::string warning;

                        warning.reserve(nlaSolverPropertyNode.getName().size() + 63); // NOLINT

                        warning += "Only NLA solver properties with name 'solverProperty' are currently supported. The element '";
                        warning += nlaSolverPropertyNode.getName();
                        warning += "' will be ignored.";

                        warnings.push_back(warning);
                    }
                }

                nlaSolver = SolverKinsol::create();

                nlaSolver->pimpl()->populate(algorithm);

                delete algorithm;
            }
        }
    }

    return {nlaSolver, warnings};
}

void SedmlFile::Impl::populateDocument(const SedDocumentPtr &pDocument)
{
    // Populate the models.

    removeAllIssues();

    auto &fileManager {FileManager::instance()};

    for (unsigned int i {0}; i < mDocument->getNumModels(); ++i) {
        auto source {mDocument->getModel(i)->getSource()};
        auto [isLocalFile, fileNameOrUrl] {retrieveFileInfo(source)};
        auto modelSource {(isLocalFile && stringToPath(fileNameOrUrl).is_relative()) ?
                              mLocation + fileNameOrUrl :
                              fileNameOrUrl};
#ifdef __EMSCRIPTEN__
        const auto &file {fileManager.fileFromFileNameOrUrl(modelSource)};
#else
        const auto &file {fileManager.file(modelSource)};
#endif
        SedModelPtr model;

        if (file != nullptr) {
            model = SedModel::create(pDocument, file);
        } else {
            std::string warning;

            warning.reserve(source.size() + 80); // NOLINT

            warning += "The model '";
            warning += source;
            warning += "' could not be found in the file manager. It has been automatically added to it.";

            addWarning(warning);

            model = SedModel::create(pDocument, File::create(modelSource));
        }

        model->setId(mDocument->getModel(i)->getId());

        for (unsigned int j {0}; j < mDocument->getModel(i)->getNumChanges(); ++j) {
            auto *change {mDocument->getModel(i)->getChange(j)};

            if (change->isSedChangeAttribute()) {
                static const std::string TARGET_START {"/cellml:model/cellml:component[@name='"};
                static const std::string TARGET_MIDDLE {"']/cellml:variable[@name='"};
                static const std::string TARGET_END {"']"};

                auto *changeAttribute {reinterpret_cast<libsedml::SedChangeAttribute *>(change)};
                auto target {changeAttribute->getTarget()};
                std::string componentName;
                std::string variableName;
                auto newValue {changeAttribute->getNewValue()};

                if (target.starts_with(TARGET_START)) {
                    target.erase(0, TARGET_START.length());

                    auto targetMiddlePos {target.find(TARGET_MIDDLE)};

                    if (targetMiddlePos != std::string::npos) {
                        componentName = target.substr(0, targetMiddlePos);

                        target.erase(0, targetMiddlePos + TARGET_MIDDLE.length());

                        auto targetEndPos {target.find(TARGET_END)};

                        if (targetEndPos != std::string::npos) {
                            variableName = target.substr(0, targetEndPos);
                        }

                        target.erase(0, targetEndPos + TARGET_END.length());

                        if (!target.empty()) {
                            componentName.clear();
                            variableName.clear();
                        }
                    }
                }

                auto canAddChange {true};

                if (componentName.empty() || variableName.empty()) {
                    canAddChange = false;

                    const std::string targetString(changeAttribute->getTarget());
                    std::string error;

                    error.reserve(targetString.size() + 111); // NOLINT

                    error += "The component and variable names could not be retrieved for the change of type 'changeAttribute' and of target '";
                    error += targetString;
                    error += "'.";

                    addError(error);
                }

                if (!isDouble(newValue)) {
                    canAddChange = false;

                    std::string error;

                    error.reserve(newValue.size() + 70); // NOLINT

                    error += "The new value '";
                    error += newValue;
                    error += "' for the change of type 'changeAttribute' is not a valid double value.";

                    addError(error);
                }

                if (canAddChange) {
                    model->addChange(SedChangeAttribute::create(componentName, variableName,
                                                                changeAttribute->getNewValue()));
                }
            } else {
                const std::string changeElementName(change->getElementName());
                std::string warning;

                warning.reserve(changeElementName.size() + 92); // NOLINT

                warning += "Only changes of type 'changeAttribute' are currently supported. The change of type '";
                warning += changeElementName;
                warning += "' has been ignored.";

                addWarning(warning);
            }
        }

        pDocument->addModel(model);
    }

    // Populate the simulations.

    for (unsigned int i {0}; i < mDocument->getNumSimulations(); ++i) {
        auto *sedSimulation {mDocument->getSimulation(i)};
        SedSimulationPtr simulation;

        if (sedSimulation->isSedUniformTimeCourse()) {
            auto *sedUniformTimeCourse {reinterpret_cast<libsedml::SedUniformTimeCourse *>(sedSimulation)};
            auto uniformTimeCourse {SedUniformTimeCourse::create(pDocument)};

            uniformTimeCourse->setInitialTime(sedUniformTimeCourse->getInitialTime());
            uniformTimeCourse->setOutputStartTime(sedUniformTimeCourse->getOutputStartTime());
            uniformTimeCourse->setOutputEndTime(sedUniformTimeCourse->getOutputEndTime());
            uniformTimeCourse->setNumberOfSteps(sedUniformTimeCourse->getNumberOfSteps());

            simulation = uniformTimeCourse;
        } else if (sedSimulation->isSedOneStep()) {
            auto *sedOneStep {reinterpret_cast<libsedml::SedOneStep *>(sedSimulation)};
            auto oneStep {SedOneStep::create(pDocument)};

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

        auto *sedAlgorithm {sedSimulation->getAlgorithm()};

        if (sedAlgorithm != nullptr) {
            const auto &kisaoId {sedAlgorithm->getKisaoID()};
            SolverOdePtr odeSolver {nullptr};
            SolverNlaPtr nlaSolver {nullptr};

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
                std::string warning;

                warning.reserve(kisaoId.size() + 63); // NOLINT

                warning += "The solver '";
                warning += kisaoId;
                warning += "' is not recognised. The CVODE solver will be used instead.";

                addWarning(warning);

                odeSolver = SolverCvode::create();
            }

            if (odeSolver != nullptr) {
                odeSolver->pimpl()->populate(sedAlgorithm);

                addIssues(odeSolver, odeSolver->name());

                simulation->setOdeSolver(odeSolver);
            }

            if (nlaSolver != nullptr) {
                nlaSolver->pimpl()->populate(sedAlgorithm);

                addIssues(nlaSolver, nlaSolver->name());

                simulation->setNlaSolver(nlaSolver);
            }

            // Populate the NLA solver, if any, using our new approach (i.e. using libOpenCOR's nlaSolver element) and
            // then using our legacy approach (i.e. using a SED-ML annotation).

            auto applyNlaSolverInfo = [this, &simulation](const auto &pNlaSolverInfo) {
                for (const auto &nlaSolverWarning : pNlaSolverInfo.warnings) {
                    addWarning(nlaSolverWarning);
                }

                if (pNlaSolverInfo.nlaSolver != nullptr) {
                    addIssues(pNlaSolverInfo.nlaSolver, pNlaSolverInfo.nlaSolver->name());

                    simulation->setNlaSolver(pNlaSolverInfo.nlaSolver);
                }
            };

            auto nlaSolverInfo {Impl::nlaSolver(sedSimulation, simulation->nlaSolver() != nullptr)};

            applyNlaSolverInfo(nlaSolverInfo);

            auto legacyNlaSolverInfo {legacyNlaSolver(sedSimulation, simulation->nlaSolver() != nullptr)};

            applyNlaSolverInfo(legacyNlaSolverInfo);
        }
    }

    // Populate the tasks.
    //---GRY--- FOR NOW, WE ONLY CREATE A TASK IF WE HAVE ONE MODEL AND ONE SIMULATION.

#ifdef CODE_COVERAGE_ENABLED
    if (mDocument->getNumModels() == 1) {
#else
    if ((mDocument->getNumModels() == 1) && (mDocument->getNumSimulations() == 1)) {
#endif
        pDocument->addTask(SedTask::create(pDocument, pDocument->model(0), pDocument->simulation(0)));
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
    return static_cast<Impl *>(Logger::mPimpl);
}

/*---GRY---
const SedmlFile::Impl *SedmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}
*/

SedmlFilePtr SedmlFile::create(const FilePtr &pFile)
{
    static const SedmlFilePtr NO_SEDML_FILE_PTR;

    // Check whether the file is a SED-ML file and if so then return its SedmlFile object.

    if (pFile->pimpl()->type() == File::Type::SEDML_FILE) {
        return pFile->pimpl()->mSedmlFile;
    }

    // Check whether the type of the file is already known and if so then return since there is no need to parse the
    // file contents again.

    if (pFile->pimpl()->mTypeChecked) {
        return NO_SEDML_FILE_PTR;
    }

    // Try to retrieve a SED-ML document.

    auto *document {libsedml::readSedMLFromString(toString(pFile->contents()).c_str())};

    // A non-SED-ML file results in our SED-ML document having at least one error. That error may be the result of a
    // malformed XML file (e.g., an HTML file is an XML-like file but not actually an XML file or a COMBINE archive
    // which is just not an XML file) or a valid XML file but not a SED-ML file (e.g., a CellML file is an XML file but
    // not a SED-ML file). So, we use these facts to determine whether our current SED-ML document is indeed a SED-ML
    // file.

    if ((document->getNumErrors() == 0)
        || ((document->getError(0)->getErrorId() > libsbml::XMLErrorCodesUpperBound)
            && (document->getError(0)->getErrorId() != libsedml::SedNotSchemaConformant))) {
        return SedmlFilePtr {new SedmlFile {pFile, document}};
    }

    delete document;

    return NO_SEDML_FILE_PTR;
}

void SedmlFile::populateDocument(const SedDocumentPtr &pDocument)
{
    pimpl()->populateDocument(pDocument);
}

} // namespace libOpenCOR

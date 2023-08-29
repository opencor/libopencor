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

#include "seddocument_p.h"
#include "sedmodel_p.h"

#include "utils.h"

#include "libopencor/file.h"
#include "libopencor/sedmodel.h"

#include <iomanip>
#include <sstream>

#include <libxml/tree.h>

namespace libOpenCOR {

std::string SedDocument::Impl::uniqueId(const std::string &pPrefix)
{
    static const auto WIDTH = 9;
    static const auto FILL = '0';

    size_t counter = 0;
    std::stringstream stream;

    stream << pPrefix << std::setw(WIDTH) << std::setfill(FILL) << ++counter;

    auto res = stream.str();

//---GRY---
#ifndef COVERAGE_ENABLED
    while (mIds.contains(res)) {
        stream.str({});

        stream << pPrefix << std::setw(WIDTH) << std::setfill(FILL) << ++counter;

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
        initialiseWithCellmlFile(pFile, pOwner);

        break;
    case File::Type::SEDML_FILE:
        addError("A simulation experiment description cannot currently be created using a SED-ML file.");

        break;
#ifdef __EMSCRIPTEN__
    default: // File::Type::COMBINE_ARCHIVE.
        addError("A simulation experiment description cannot currently be created using a COMBINE archive.");

        break;
#else
    case File::Type::COMBINE_ARCHIVE:
        addError("A simulation experiment description cannot currently be created using a COMBINE archive.");

        break;
    default: // File::Type::IRRETRIEVABLE_FILE.
        addError("A simulation experiment description cannot be created using an irretrievable file.");

        break;
#endif
    }
}

void SedDocument::Impl::initialiseWithCellmlFile(const FilePtr &pFile, const SedDocumentPtr &pOwner)
{
    // Add a model for the given CellML file.

    mModels.push_back(std::shared_ptr<SedModel> {new SedModel {pFile, pOwner}});
}

std::string SedDocument::Impl::serialise(const std::string &pBasePath) const
{
    // Serialise our SED-ML document using libxml2.

    auto *doc = xmlNewDoc(constXmlCharPtr("1.0"));
    auto *sedNode = xmlNewNode(nullptr, constXmlCharPtr("sed"));

    xmlNewProp(sedNode, constXmlCharPtr("xmlns"), constXmlCharPtr(mXmlns));
    xmlNewProp(sedNode, constXmlCharPtr("level"), constXmlCharPtr(std::to_string(mLevel)));
    xmlNewProp(sedNode, constXmlCharPtr("version"), constXmlCharPtr(std::to_string(mVersion)));

    xmlDocSetRootElement(doc, sedNode);

    // Add the data descriptions, if any, to our SED-ML document.

    /*---GRY---
    if (!mDataDescriptions.empty()) {
        auto *sedListOfDataDescriptions = xmlNewNode(nullptr, constXmlCharPtr("listOfDataDescriptions"));

        for (const auto &dataDescription : mDataDescriptions) {
            (void)dataDescription;
        }

        xmlAddChild(sedNode, sedListOfDataDescriptions);
    }
    */

    // Add the models, if any, to our SED-ML document.

    if (!mModels.empty()) {
        auto *sedListOfModels = xmlNewNode(nullptr, constXmlCharPtr("listOfModels"));

        for (const auto &model : mModels) {
            model->pimpl()->populate(sedListOfModels, pBasePath);
        }

        xmlAddChild(sedNode, sedListOfModels);
    }

    // Add the simulations, if any, to our SED-ML document.

    /*---GRY---
    if (!mSimulations.empty()) {
        auto *sedListOfSimulations = xmlNewNode(nullptr, constXmlCharPtr("listOfSimulations"));

        for (const auto &simulation : mSimulations) {
            (void)simulation;
        }

        xmlAddChild(sedNode, sedListOfSimulations);
    }
    */

    // Add the tasks, if any, to our SED-ML document.

    /*---GRY---
    if (!mTasks.empty()) {
        auto *sedListOfTasks = xmlNewNode(nullptr, constXmlCharPtr("listOfTasks"));

        for (const auto &task : mTasks) {
            (void)task;
        }

        xmlAddChild(sedNode, sedListOfTasks);
    }
    */

    // Add the data generators, if any, to our SED-ML document.

    /*---GRY---
    if (!mDataGenerators.empty()) {
        auto *sedListOfDataGenerators = xmlNewNode(nullptr, constXmlCharPtr("listOfDataGenerators"));

        for (const auto &dataGenerator : mDataGenerators) {
            (void)dataGenerator;
        }

        xmlAddChild(sedNode, sedListOfDataGenerators);
    }
    */

    // Add the outputs, if any, to our SED-ML document.

    /*---GRY---
    if (!mOutputs.empty()) {
        auto *sedListOfOutputs = xmlNewNode(nullptr, constXmlCharPtr("listOfOutputs"));

        for (const auto &output : mOutputs) {
            (void)output;
        }

        xmlAddChild(sedNode, sedListOfOutputs);
    }
    */

    // Add the styles, if any, to our SED-ML document.

    /*---GRY---
    if (!mStyles.empty()) {
        auto *sedListOfStyles = xmlNewNode(nullptr, constXmlCharPtr("listOfStyles"));

        for (const auto &style : mStyles) {
            (void)style;
        }

        xmlAddChild(sedNode, sedListOfStyles);
    }
    */

    // Add the algorithm parameters, if any, to our SED-ML document.

    /*---GRY---
    if (!mAlgorithmParameters.empty()) {
        auto *sedListOfAlgorithmParameters = xmlNewNode(nullptr, constXmlCharPtr("listOfAlgorithmParameters"));

        for (const auto &algorithmParameter : mAlgorithmParameters) {
            (void)algorithmParameter;
        }

        xmlAddChild(sedNode, sedListOfAlgorithmParameters);
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

SedDocument::SedDocument()
    : SedBase(new Impl())
{
}

SedDocument::~SedDocument()
{
    delete pimpl();
}

SedDocument::Impl *SedDocument::pimpl()
{
    return reinterpret_cast<Impl *>(SedBase::pimpl());
}

const SedDocument::Impl *SedDocument::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedBase::pimpl());
}

SedDocumentPtr SedDocument::create()
{
    return std::shared_ptr<SedDocument> {new SedDocument {}};
}

void SedDocument::initialise(const FilePtr &pFile)
{
    pimpl()->initialise(pFile, shared_from_this());
}

std::string SedDocument::serialise() const
{
    return serialise({});
}

std::string SedDocument::serialise(const std::string &pBasePath) const
{
    return pimpl()->serialise(pBasePath);
}

} // namespace libOpenCOR

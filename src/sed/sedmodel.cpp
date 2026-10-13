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
#include "sedchange_p.h"
#include "seddocument_p.h"
#include "sedmodel_p.h"

#include "utils.h"

#include <algorithm>
#include <memory>

namespace libOpenCOR {

static constexpr auto ID_PREFIX {"model"};

SedModel::Impl::Impl(const SedDocumentPtr &pDocument, const FilePtr &pFile)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
    , mFile(pFile)
{
}

const FilePtr &SedModel::Impl::file() const
{
    return mFile;
}

bool SedModel::Impl::isValid()
{
    // Reset our issues so that they are not reported again should we be validated again.

    removeAllIssues();

    // Make sure that the CellML file is valid.

    switch (mFile->pimpl()->mCellmlFile->type()) {
    case libcellml::AnalyserModel::Type::INVALID:
        addError("The CellML file is invalid.");

        break;
    case libcellml::AnalyserModel::Type::OVERCONSTRAINED:
        addError("The CellML file is overconstrained.");

        break;
    case libcellml::AnalyserModel::Type::UNDERCONSTRAINED:
        addError("The CellML file is underconstrained.");

        break;
    case libcellml::AnalyserModel::Type::UNSUITABLY_CONSTRAINED:
        addError("The CellML file is unsuitably constrained.");

        break;
    default: // libcellml::AnalyserModel::Type::UNKNOWN (we should never get this type),
             // libcellml::AnalyserModel::Type::ALGEBRAIC,
             // libcellml::AnalyserModel::Type::DAE,
             // libcellml::AnalyserModel::Type::NLA, or
             // libcellml::AnalyserModel::Type::ODE.
        break;
    }

    addIssues(mFile->pimpl()->mCellmlFile, "CellML");

    return !hasErrors();
}

bool SedModel::Impl::hasChanges() const
{
    return !mChanges.empty();
}

size_t SedModel::Impl::changeCount() const
{
    return mChanges.size();
}

const SedChangePtrs &SedModel::Impl::changes() const
{
    return mChanges;
}

const SedChangePtr &SedModel::Impl::change(size_t pIndex) const
{
    static const SedChangePtr NO_SED_CHANGE_PTR;

    if (pIndex >= mChanges.size()) {
        return NO_SED_CHANGE_PTR;
    }

    return mChanges[pIndex];
}

bool SedModel::Impl::addChange(const SedChangePtr &pChange)
{
    if (pChange == nullptr) {
        return false;
    }

    auto change {std::ranges::find_if(mChanges, [&pChange](const auto &c) {
        return c == pChange;
    })};

    if (change != mChanges.end()) {
        return false;
    }

    mChanges.push_back(pChange);

    return true;
}

bool SedModel::Impl::removeChange(const SedChangePtr &pChange)
{
    auto change {std::ranges::find_if(mChanges, [&pChange](const auto &c) {
        return c == pChange;
    })};

    if (change != mChanges.end()) {
        mChanges.erase(change);

        return true;
    }

    return false;
}

bool SedModel::Impl::removeAllChanges()
{
    if (!hasChanges()) {
        return false;
    }

    while (!mChanges.empty()) {
        removeChange(mChanges.front());
    }

    return true;
}

void SedModel::Impl::setBasePath(const std::string &pBasePath)
{
    mBasePath = pBasePath;
}

void SedModel::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node {xmlNewNode(nullptr, toConstXmlCharPtr("model"))};

    SedBase::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("language"), toConstXmlCharPtr("urn:sedml:language:cellml"));

    auto source {mBasePath.empty() ?
                     urlPath(mFile->path()) :
#ifdef BUILDING_USING_MSVC
                     forwardSlashPath(relativePath(mFile->path(), mBasePath))};
#else
                     relativePath(mFile->path(), mBasePath)};
#endif

    // A model source is a URI reference, which gets decoded when it is for a local file and which query and fragment
    // are then ignored (see retrieveFileInfo() and SedmlFile::Impl::populateDocument()). So, for a local file, we must
    // percent-encode the percent signs, question marks, and number signs in its file name (e.g., "my%20model#1.cellml"
    // must become "my%2520model%231.cellml"), so that the model source can be decoded back to the file name of the
    // model.

    if (mFile->url().empty()) {
        std::string encodedSource;

        encodedSource.reserve(source.size());

        for (const auto sourceChar : source) {
            if (sourceChar == '%') {
                encodedSource += "%25";
            } else if (sourceChar == '?') {
                encodedSource += "%3F";
            } else if (sourceChar == '#') {
                encodedSource += "%23";
            } else {
                encodedSource += sourceChar;
            }
        }

        source = std::move(encodedSource);
    }

    // A relative model source which first segment contains a colon (e.g., "my:model.cellml") would be mistaken for a URI
    // with a scheme (i.e. "my"), so we prefix it with "./" (i.e. "./my:model.cellml"), as recommended by
    // https://www.rfc-editor.org/rfc/rfc3986#section-4.2.
    // Note: a model source is only relative if we were given a base path (see relativePath()), so we don't need to worry
    //       about the drive letter of a Windows file name (e.g., "C:/some/path/model.cellml").

    if (!mBasePath.empty() && (source.find(':') < source.find('/'))) {
        source.insert(0, "./");
    }

    // The file name or URL of the model may not be valid UTF-8 (e.g., "file:///some/path/%FF.cellml" gets
    // percent-decoded to "/some/path/\xFF.cellml") or may contain characters that are not allowed in an XML document
    // (e.g., "/some/path/\x01.cellml"), but the model source must be valid since it is an XML attribute. So, we
    // percent-encode those bytes (e.g., "\xFF.cellml" becomes "%FF.cellml"), which get decoded back when the SED-ML
    // file is loaded.
    // Note #1: this must be done after percent-encoding the percent signs of a local file name, so that the percent
    //          signs that we add here don't get percent-encoded.
    // Note #2: a null character is also percent-encoded (i.e. "%00"), so that the model source doesn't get truncated
    //          (and refer to another file). It doesn't, however, get decoded back (see decodeUrl()), so the model
    //          source won't refer to the model when the SED-ML file is loaded, but such a model cannot exist on disk
    //          anyway.

    source = percentEncodeNonXmlCharacters(source);

    xmlNewProp(node, toConstXmlCharPtr("source"), toConstXmlCharPtr(source));

    xmlAddChild(pNode, node);

    // Add the changes, if any, to our SED-ML model.

    if (!mChanges.empty()) {
        auto *sedListOfChangeNodes {xmlNewNode(nullptr, toConstXmlCharPtr("listOfChanges"))};

        xmlAddChild(node, sedListOfChangeNodes);

        for (const auto &change : mChanges) {
            change->pimpl()->serialise(sedListOfChangeNodes);
        }
    }
}

SedModel::SedModel(const SedDocumentPtr &pDocument, const FilePtr &pFile)
    : SedBase(std::make_unique<Impl>(pDocument, pFile))
{
}

SedModel::~SedModel() = default;

SedModel::Impl *SedModel::pimpl()
{
    return static_cast<Impl *>(SedBase::pimpl());
}

const SedModel::Impl *SedModel::pimpl() const
{
    return static_cast<const Impl *>(SedBase::pimpl());
}

SedModelPtr SedModel::create(const SedDocumentPtr &pDocument, const FilePtr &pFile)
{
    return SedModelPtr {new SedModel {pDocument, pFile}};
}

const FilePtr &SedModel::file() const
{
    return pimpl()->file();
}

bool SedModel::hasChanges() const
{
    return pimpl()->hasChanges();
}

size_t SedModel::changeCount() const
{
    return pimpl()->changeCount();
}

const SedChangePtrs &SedModel::changes() const
{
    return pimpl()->changes();
}

const SedChangePtr &SedModel::change(size_t pIndex) const
{
    return pimpl()->change(pIndex);
}

bool SedModel::addChange(const SedChangePtr &pChange)
{
    return pimpl()->addChange(pChange);
}

bool SedModel::removeChange(const SedChangePtr &pChange)
{
    return pimpl()->removeChange(pChange);
}

bool SedModel::removeAllChanges()
{
    return pimpl()->removeAllChanges();
}

} // namespace libOpenCOR

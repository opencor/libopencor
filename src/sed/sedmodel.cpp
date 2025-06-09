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

namespace libOpenCOR {

static constexpr auto ID_PREFIX = "model";

SedModel::Impl::Impl(const SedDocumentPtr &pDocument, const FilePtr &pFile)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
    , mFile(pFile)
{
}

FilePtr SedModel::Impl::file() const
{
    return mFile;
}

bool SedModel::Impl::isValid()
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT CELLML FILES. THIS WILL CLEARLY CHANGE IN THE FUTURE.

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

    addIssues(mFile->pimpl()->mCellmlFile);

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

SedChangePtrs SedModel::Impl::changes() const
{
    return mChanges;
}

SedChangePtr SedModel::Impl::change(size_t pIndex) const
{
    if (pIndex >= mChanges.size()) {
        return {};
    }

    return mChanges[pIndex];
}

bool SedModel::Impl::addChange(const SedChangePtr &pChange)
{
    if (pChange == nullptr) {
        return false;
    }

    auto change = std::ranges::find_if(mChanges, [&](const auto &c) {
        return c == pChange;
    });

    if (change != mChanges.end()) {
        return false;
    }

    mChanges.push_back(pChange);

    return true;
}

bool SedModel::Impl::removeChange(const SedChangePtr &pChange)
{
    auto change = std::ranges::find_if(mChanges, [&](const auto &c) {
        return c == pChange;
    });

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
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("model"));

    SedBase::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("language"), toConstXmlCharPtr("urn:sedml:language:cellml"));

    auto source = mBasePath.empty() ?
                      urlPath(mFile->path()) :
#ifdef BUILDING_USING_MSVC
                      forwardSlashPath(relativePath(mFile->path(), mBasePath));
#else
                      relativePath(mFile->path(), mBasePath);
#endif

    xmlNewProp(node, toConstXmlCharPtr("source"), toConstXmlCharPtr(source));

    xmlAddChild(pNode, node);

    // Add the changes, if any, to our SED-ML model.

    if (!mChanges.empty()) {
        auto *sedListOfChangeNodes = xmlNewNode(nullptr, toConstXmlCharPtr("listOfChanges"));

        xmlAddChild(node, sedListOfChangeNodes);

        for (const auto &change : mChanges) {
            change->pimpl()->serialise(sedListOfChangeNodes);
        }
    }
}

SedModel::SedModel(const SedDocumentPtr &pDocument, const FilePtr &pFile)
    : SedBase(new Impl(pDocument, pFile))
{
}

SedModel::~SedModel()
{
    delete pimpl();
}

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

FilePtr SedModel::file() const
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

SedChangePtrs SedModel::changes() const
{
    return pimpl()->changes();
}

SedChangePtr SedModel::change(size_t pIndex) const
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

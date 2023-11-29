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
#include "utils.h"

#include "libopencor/file.h"

#include <filesystem>

namespace libOpenCOR {

static constexpr auto ID_PREFIX = "model";

SedModel::Impl::Impl(const FilePtr &pFile, const SedDocumentPtr &pDocument)
    : SedBase::Impl(pDocument->pimpl()->uniqueId(ID_PREFIX))
    , mFile(pFile)
{
}

bool SedModel::Impl::isValid()
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT CELLML FILES. THIS WILL CLEARLY CHANGE IN THE FUTURE...

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

    return mErrors.empty();
}

void SedModel::Impl::serialise(xmlNodePtr pNode, const std::string &pBasePath) const
{
    auto *node = xmlNewNode(nullptr, toConstXmlCharPtr("model"));

    SedBase::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("language"), toConstXmlCharPtr("urn:sedml:language:cellml"));

    auto source = pBasePath.empty() ?
                      urlPath(mFile->path()) :
#ifdef BUILDING_USING_MSVC
                      forwardSlashPath(relativePath(mFile->path(), pBasePath));
#else
                      relativePath(mFile->path(), pBasePath);
#endif

    xmlNewProp(node, toConstXmlCharPtr("source"), toConstXmlCharPtr(source));

    xmlAddChild(pNode, node);
}

SedModel::SedModel(const FilePtr &pFile, const SedDocumentPtr &pDocument)
    : SedBase(new Impl(pFile, pDocument))
{
}

SedModel::~SedModel()
{
    delete pimpl();
}

SedModel::Impl *SedModel::pimpl()
{
    return reinterpret_cast<Impl *>(SedBase::pimpl());
}

/*---GRY---
const SedModel::Impl *SedModel::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedBase::pimpl());
}
*/

SedModelPtr SedModel::create(const FilePtr &pFile, const SedDocumentPtr &pDocument)
{
    return SedModelPtr {new SedModel {pFile, pDocument}};
}

} // namespace libOpenCOR

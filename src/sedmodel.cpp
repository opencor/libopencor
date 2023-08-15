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

namespace libOpenCOR {

static constexpr auto ID_PREFIX = "model";
static constexpr auto CELLML_LANGUAGE = "urn:sedml:language:cellml";

SedModel::Impl::Impl(const FilePtr &pFile, const SedDocumentPtr &pDocument)
    : mFile(pFile)
    , mLanguage(CELLML_LANGUAGE)
{
    mId = pDocument->pimpl()->uniqueId(ID_PREFIX);
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

} // namespace libOpenCOR

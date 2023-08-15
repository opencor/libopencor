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

#include "libopencor/file.h"
#include "libopencor/sedmodel.h"

#include <iomanip>
#include <sstream>

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

std::string SedDocument::Impl::serialise()
{
    return {};
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

SedDocumentPtr SedDocument::create(const FilePtr &pFile)
{
    auto res = std::shared_ptr<SedDocument> {new SedDocument {}};

    res->pimpl()->initialise(pFile, res->shared_from_this());

    return res;
}

std::string SedDocument::serialise() const
{
    return pimpl()->serialise();
}

} // namespace libOpenCOR

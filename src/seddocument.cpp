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

#include "libopencor/seddocument.h"

#include "file_p.h"
#include "seddocument_p.h"

#include "libopencor/file.h"

namespace libOpenCOR {

SedDocument::Impl::Impl(const FilePtr &pFile)
{
    // Make sure that the given file is supported.

    switch (pFile->type()) {
    case File::Type::UNKNOWN_FILE:
        addError("A simulation experiment description cannot be created using an unknown file.");

        break;
    case File::Type::CELLML_FILE:
        // All good.

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

SedDocument::SedDocument(const FilePtr &pFile)
    : SedBase(new Impl(pFile))
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

/*---GRY---
const SedDocument::Impl *SedDocument::pimpl() const
{
    return reinterpret_cast<const Impl *>(SedBase::pimpl());
}
*/

SedDocumentPtr SedDocument::create(const FilePtr &pFile)
{
    return std::shared_ptr<SedDocument> {new SedDocument {pFile}};
}

} // namespace libOpenCOR

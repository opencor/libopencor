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

#include "sedmlfile_p.h"
#include "utils.h"

#include "libopencor/file.h"

#include "libsedmlbegin.h"
#include <sedml/SedDocument.h>
#include <sedml/SedReader.h>
#include "libsedmlend.h"

namespace libOpenCOR {

SedmlFile::Impl::Impl(libsedml::SedDocument *pDocument)
    : mDocument(pDocument)
{
}

SedmlFile::Impl::~Impl()
{
    delete mDocument;
}

SedmlFile::SedmlFile(libsedml::SedDocument *pDocument)
    : Logger(new Impl {pDocument})
{
}

SedmlFile::~SedmlFile()
{
    delete pimpl();
}

SedmlFile::Impl *SedmlFile::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedmlFile::Impl *SedmlFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}
*/

SedmlFilePtr SedmlFile::create(const FilePtr &pFile)
{
    // Try to retrieve a SED-ML document.

    if (!pFile->contents().empty()) {
        auto *document = libsedml::readSedMLFromString(toString(pFile->contents()).c_str());

        // A non-SED-ML file results in our SED-ML document having at least one error, the first of which being of id
        // libsedml::SedNotSchemaConformant (e.g., a CellML file, i.e. an XML file, but not a SED-ML one) or
        // XMLContentEmpty (e.g., a COMBINE archive, i.e. not an XML file). So, we use these facts to determine whether
        // our current SED-ML document is indeed a SED-ML file.

        if ((document->getNumErrors() == 0)
            || ((document->getError(0)->getErrorId() != libsedml::SedNotSchemaConformant)
                && (document->getError(0)->getErrorId() != libsbml::XMLContentEmpty))) {
            return SedmlFilePtr {new SedmlFile {document}};
        }

        delete document;
    }

    return nullptr;
}

} // namespace libOpenCOR

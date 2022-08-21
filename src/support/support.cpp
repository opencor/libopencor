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

#include "support.h"
#include "utils.h"

#include <combine/combinearchive.h>

#include <libcellml>

#include "libsedmlbegin.h"
#include <sedml/SedDocument.h>
#include <sedml/SedReader.h>
#include "libsedmlend.h"

namespace libOpenCOR::Support {

bool isCellmlFile(const std::string &pFileName)
{
    // Try to parse the file contents.

    auto [contents, size] = fileContents(pFileName);
    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(contents.get());

    return parser->issueCount() == 0;
}

bool isCombineArchive(const std::string &pFileName)
{
    // Try to retrieve a COMBINE archive.

    auto combineArchive = libcombine::CombineArchive();
    auto res = combineArchive.initializeFromArchive(pFileName);

    if (res) {
        combineArchive.cleanUp();
    }

    return res;
}

bool isSedmlFile(const std::string &pFileName)
{
    // Try to retrieve a SED-ML document.

    auto *sedmlDocument = libsedml::readSedML(pFileName.c_str());

    // A non-SED-ML file results in our SED-ML document having at least one
    // error, the first of which being of id libsedml::SedNotSchemaConformant
    // (e.g., a CellML file, i.e. an XML file, but not a SED-ML one) or
    // XMLContentEmpty (e.g., a COMBINE archive, i.e. not an XML file). So, we
    // use these facts to determine whether our current SED-ML document is
    // indeed a SED-ML file.

    auto res = (sedmlDocument->getNumErrors() == 0)
               || ((sedmlDocument->getError(0)->getErrorId() != libsedml::SedNotSchemaConformant)
                   && (sedmlDocument->getError(0)->getErrorId() != XMLContentEmpty));

    delete sedmlDocument; // NOLINT(cppcoreguidelines-owning-memory)

    return res;
}

} // namespace libOpenCOR::Support
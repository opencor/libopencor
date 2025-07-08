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

#pragma once

#include "unittestingexport.h"

#include "libopencor/logger.h"

namespace libsedml {
class SedDocument;
} // namespace libsedml

namespace libOpenCOR {

class SedmlFile;
using SedmlFilePtr = std::shared_ptr<SedmlFile>;

class LIBOPENCOR_UNIT_TESTING_EXPORT SedmlFile: public Logger
{
public:
    SedmlFile() = delete;
    ~SedmlFile() override;

    SedmlFile(const SedmlFile &pOther) = delete;
    SedmlFile(SedmlFile &&pOther) noexcept = delete;

    SedmlFile &operator=(const SedmlFile &pRhs) = delete;
    SedmlFile &operator=(SedmlFile &&pRhs) noexcept = delete;

    static SedmlFilePtr create(const FilePtr &pFile);

    void populateDocument(const SedDocumentPtr &pDocument);

private:
    class Impl;

    explicit SedmlFile(const FilePtr &pFile, libsedml::SedDocument *pDocument);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR

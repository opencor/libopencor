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

#include "logger_p.h"

#include "sedmlfile.h"
#include "utils.h"

namespace libOpenCOR {

class SedmlFile::Impl: public Logger::Impl
{
public:
    std::string mLocation;
    libsedml::SedDocument *mDocument;

    explicit Impl(const FilePtr &pFile, libsedml::SedDocument *pDocument);
    ~Impl() override;

    void populateDocument(const SedDocumentPtr &pDocument);
};

} // namespace libOpenCOR

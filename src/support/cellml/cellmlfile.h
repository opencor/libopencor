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

#include "cellmlfileruntime.h"
#include "unittestingexport.h"

#include "libcellml/analyser.h"
#include "libcellml/analysermodel.h"
#include "libcellml/analyservariable.h"
#include "libcellml/component.h"
#include "libcellml/generator.h"
#include "libcellml/generatorprofile.h"
#include "libcellml/importer.h"
#include "libcellml/interpreter.h"
#include "libcellml/model.h"
#include "libcellml/parser.h"
#include "libcellml/units.h"
#include "libcellml/variable.h"

#include "libopencor/logger.h"

#include <memory>

namespace libOpenCOR {

class CellmlFile;
using CellmlFilePtr = std::shared_ptr<CellmlFile>;

class CellmlFileRuntime;
using CellmlFileRuntimePtr = std::shared_ptr<CellmlFileRuntime>;

class LIBOPENCOR_UNIT_TESTING_EXPORT CellmlFile: public Logger
    , public std::enable_shared_from_this<CellmlFile>
{
public:
    CellmlFile() = delete;
    ~CellmlFile() override;

    CellmlFile(const CellmlFile &pOther) = delete;
    CellmlFile(CellmlFile &&pOther) noexcept = delete;

    CellmlFile &operator=(const CellmlFile &pRhs) = delete;
    CellmlFile &operator=(CellmlFile &&pRhs) noexcept = delete;

    static CellmlFilePtr create(const FilePtr &pFile);

    void populateDocument(const SedDocumentPtr &pDocument);

    libcellml::AnalyserModel::Type type() const;

    libcellml::AnalyserPtr analyser() const;
    libcellml::AnalyserModelPtr analyserModel() const;

    CellmlFileRuntimePtr runtime(const SolverNlaPtr &pNlaSolver = {}, bool pCompiled = true);

private:
    class Impl;

    explicit CellmlFile(const FilePtr &pFile, const libcellml::ModelPtr &pModel, bool pStrict);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR

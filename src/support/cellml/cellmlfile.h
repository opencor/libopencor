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

#include "libopencor/logger.h"

#include <libcellml>
#include <memory>

namespace libOpenCOR {

class CellmlFile;
using CellmlFilePtr = std::shared_ptr<CellmlFile>;

class CellmlFileRuntime;
using CellmlFileRuntimePtr = std::shared_ptr<CellmlFileRuntime>;

class CellmlFile: public Logger
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

    libcellml::ModelPtr model() const;

    CellmlFileRuntimePtr runtime();

private:
    class Impl;

    explicit CellmlFile(const libcellml::ModelPtr &pModel);

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR

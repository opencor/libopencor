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

#include "cellmlfile.h"
#include "combinearchive.h"
#include "logger_p.h"
#include "sedmlfile.h"

namespace libOpenCOR {

class Simulation::Impl: public Logger::Impl
{
public:
    const FilePtr &mFile;

    CellmlFilePtr mCellmlFile;
    SedmlFilePtr mSedmlFile;
    CombineArchivePtr mCombineArchive;

    Impl(const FilePtr &pFile);
    ~Impl() = default;

    bool supportedFile() const;

    void run();
    void pause();
    void resume();
    void stop();
};

} // namespace libOpenCOR

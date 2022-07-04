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

#include "supportedfilemanager.h"

namespace libOpenCOR {

class SedmlFileManager: public SupportedFileManager
{
public:
    static SedmlFileManager *instance();

    SedmlFileManager() = default;

    SedmlFileManager(const SedmlFileManager &pOther) = delete;
    SedmlFileManager(SedmlFileManager &&) noexcept = delete;

    SedmlFileManager &operator=(const SedmlFileManager &pRhs) = delete;
    SedmlFileManager &operator=(SedmlFileManager &&) noexcept = delete;
};

} // namespace libOpenCOR

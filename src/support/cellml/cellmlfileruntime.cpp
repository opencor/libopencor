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

#include "cellmlfileruntime_p.h"
#include "compiler.h"

namespace libOpenCOR {

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile)
{
    auto cellmlFileAnalyser = pCellmlFile->analyser();

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser);
#ifndef __EMSCRIPTEN__
    } else {
        // Generate some code for the model and compile it, should the analysis have been fine.

        auto generator = libcellml::Generator::create();
        auto generatorProfile = libcellml::GeneratorProfile::create();

        generatorProfile->setOriginCommentString("");
        generatorProfile->setImplementationHeaderString("");
        generatorProfile->setImplementationVersionString("");
        generatorProfile->setImplementationStateCountString("");
        generatorProfile->setImplementationVariableCountString("");
        generatorProfile->setImplementationLibcellmlVersionString("");
        generatorProfile->setImplementationVoiInfoString("");
        generatorProfile->setImplementationStateInfoString("");
        generatorProfile->setImplementationVariableInfoString("");
        generatorProfile->setImplementationCreateStatesArrayMethodString("");
        generatorProfile->setImplementationCreateVariablesArrayMethodString("");
        generatorProfile->setImplementationDeleteArrayMethodString("");

        generator->setModel(pCellmlFile->analyserModel());
        generator->setProfile(generatorProfile);

        auto compiler = Compiler::create();

#    ifdef CODE_COVERAGE_ENABLED
        compiler->compile(generator->implementationCode());
#    else
        if (!compiler->compile(generator->implementationCode())) {
            // The compilation failed, so add the issues it generated.

            addIssues(compiler);
        }
#    endif
#endif
    }
}

CellmlFileRuntime::CellmlFileRuntime(const CellmlFilePtr &pCellmlFile)
    : Logger(new Impl {pCellmlFile})
{
}

CellmlFileRuntime::~CellmlFileRuntime()
{
    delete pimpl();
}

CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}
*/

CellmlFileRuntimePtr CellmlFileRuntime::create(const CellmlFilePtr &pCellmlFile)
{
    return CellmlFileRuntimePtr {new CellmlFileRuntime {pCellmlFile}};
}

} // namespace libOpenCOR

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
    // Analyse the model.

    auto analyser = libcellml::Analyser::create();
    auto printer = libcellml::Printer::create();

    analyser->analyseModel(pCellmlFile->model());

    if (analyser->issueCount() != 0) {
        // The analyser reported some issues, so make them ours.

        for (size_t i = 0; i < analyser->issueCount(); ++i) {
            auto issue = analyser->issue(i);
            Issue::Type type = Issue::Type::ERROR;

#ifdef CODE_COVERAGE_ENABLED
            if (issue->level() == libcellml::Issue::Level::ERROR) {
                type = Issue::Type::ERROR;
            } else {
                type = Issue::Type::WARNING;
            }
#else
            switch (issue->level()) {
            case libcellml::Issue::Level::ERROR:
                type = Issue::Type::ERROR;

                break;
            case libcellml::Issue::Level::WARNING:
                type = Issue::Type::WARNING;

                break;
            default: // libcellml::Issue::Level::MESSAGE.
                type = Issue::Type::MESSAGE;

                break;
            }
#endif

            addIssue(issue->description(), type);
        }
    }

#ifndef __EMSCRIPTEN__
    // Generate some code for the model and compile it, should the analysis have been fine.

    if (mIssues.empty()) {
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

        generator->setModel(analyser->model());
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
    }
#endif
}

CellmlFileRuntime::CellmlFileRuntime(const CellmlFilePtr &pCellmlFile)
    : Logger(new Impl(pCellmlFile))
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

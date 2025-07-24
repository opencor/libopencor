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
#include "solvernla_p.h"

#include "cellmlfile.h"

namespace libOpenCOR {

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    auto cellmlFileAnalyser = pCellmlFile->analyser();

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser);
    } else {
        // Get an NLA solver, if needed.

        if (pNlaSolver != nullptr) {
            mNlaSolverAddress = nlaSolverAddress(pNlaSolver.get());
        }

        // Determine the type of the model.

        auto cellmlFileType = pCellmlFile->type();
        auto differentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                                 || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);

        // Generate some code for the given CellML file.

        auto generator = libcellml::Generator::create();
        auto generatorProfile = libcellml::GeneratorProfile::create();

        generatorProfile->setOriginCommentString("");
        generatorProfile->setImplementationHeaderString("");
        generatorProfile->setImplementationVersionString("");
        generatorProfile->setImplementationStateCountString("");
        generatorProfile->setImplementationConstantCountString("");
        generatorProfile->setImplementationComputedConstantCountString("");
        generatorProfile->setImplementationAlgebraicCountString("");
        generatorProfile->setImplementationExternalCountString("");
        generatorProfile->setImplementationLibcellmlVersionString("");
        generatorProfile->setImplementationVoiInfoString("");
        generatorProfile->setImplementationStateInfoString("");
        generatorProfile->setImplementationConstantInfoString("");
        generatorProfile->setImplementationComputedConstantInfoString("");
        generatorProfile->setImplementationAlgebraicInfoString("");
        generatorProfile->setImplementationExternalInfoString("");
        generatorProfile->setImplementationCreateStatesArrayMethodString("");
        generatorProfile->setImplementationCreateConstantsArrayMethodString("");
        generatorProfile->setImplementationCreateComputedConstantsArrayMethodString("");
        generatorProfile->setImplementationCreateAlgebraicArrayMethodString("");
        generatorProfile->setImplementationCreateExternalsArrayMethodString("");
        generatorProfile->setImplementationDeleteArrayMethodString("");

        if (pNlaSolver != nullptr) {
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long long size_t;

extern void nlaSolve(const char *, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
            generatorProfile->setNlaSolveCallString(differentialModel, false,
                                                    std::string("nlaSolve(\"").append(mNlaSolverAddress).append("\", objectiveFunction[INDEX], u, [SIZE], &rfi);\n"));
        }

        generator->setModel(pCellmlFile->analyserModel());
        generator->setProfile(generatorProfile);

#ifndef __EMSCRIPTEN__
        // Compile the generated code.

        mCompiler = Compiler::create();

#    ifdef CODE_COVERAGE_ENABLED
        mCompiler->compile(generator->implementationCode());
#    else
        if (!mCompiler->compile(generator->implementationCode())) {
            // The compilation failed, so add the issues it generated.

            addIssues(mCompiler);

            return;
        }
#    endif

        // Make sure that our compiler knows about nlaSolve(), if needed.

        if ((cellmlFileType == libcellml::AnalyserModel::Type::NLA)
            || (cellmlFileType == libcellml::AnalyserModel::Type::DAE)) {
#    ifndef CODE_COVERAGE_ENABLED
            const bool functionAdded =
#    endif
                mCompiler->addFunction("nlaSolve", reinterpret_cast<void *>(nlaSolve));

#    ifndef CODE_COVERAGE_ENABLED
            if (!functionAdded) {
                addIssues(mCompiler);

                return;
            }
#    endif
        }

        // Retrieve our algebraic/differential functions and make sure that we managed to retrieve them.

        if (differentialModel) {
            mInitialiseVariablesForDifferentialModel = reinterpret_cast<InitialiseVariablesForDifferentialModel>(mCompiler->function("initialiseVariables"));
            mComputeComputedConstants = reinterpret_cast<ComputeComputedConstants>(mCompiler->function("computeComputedConstants"));
            mComputeRates = reinterpret_cast<ComputeRates>(mCompiler->function("computeRates"));
            mComputeVariablesForDifferentialModel = reinterpret_cast<ComputeVariablesForDifferentialModel>(mCompiler->function("computeVariables"));

#    ifndef CODE_COVERAGE_ENABLED
            if ((mInitialiseVariablesForDifferentialModel == nullptr)
                || (mComputeComputedConstants == nullptr)
                || (mComputeRates == nullptr)
                || (mComputeVariablesForDifferentialModel == nullptr)) {
                if (cellmlFileType == libcellml::AnalyserModel::Type::ODE) {
                    addError("The functions needed to compute the ODE model could not be retrieved.");
                } else {
                    addError("The functions needed to compute the DAE model could not be retrieved.");
                }
            }
#    endif
        } else {
            mInitialiseVariablesForAlgebraicModel = reinterpret_cast<InitialiseVariablesForAlgebraicModel>(mCompiler->function("initialiseVariables"));
            mComputeComputedConstants = reinterpret_cast<ComputeComputedConstants>(mCompiler->function("computeComputedConstants"));
            mComputeVariablesForAlgebraicModel = reinterpret_cast<ComputeVariablesForAlgebraicModel>(mCompiler->function("computeVariables"));

#    ifndef CODE_COVERAGE_ENABLED
            if ((mInitialiseVariablesForAlgebraicModel == nullptr)
                || (mComputeComputedConstants == nullptr)
                || (mComputeVariablesForAlgebraicModel == nullptr)) {
                if (cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC) {
                    addError("The functions needed to compute the algebraic model could not be retrieved.");
                } else {
                    addError("The functions needed to compute the NLA model could not be retrieved.");
                }
            }
#    endif
        }
#endif
    }
}

CellmlFileRuntime::Impl::~Impl()
{
    delete[] mNlaSolverAddress;
}

CellmlFileRuntime::InitialiseVariablesForAlgebraicModel CellmlFileRuntime::Impl::initialiseVariablesForAlgebraicModel() const
{
    return mInitialiseVariablesForAlgebraicModel;
}

CellmlFileRuntime::InitialiseVariablesForDifferentialModel CellmlFileRuntime::Impl::initialiseVariablesForDifferentialModel() const
{
    return mInitialiseVariablesForDifferentialModel;
}

CellmlFileRuntime::ComputeComputedConstants CellmlFileRuntime::Impl::computeComputedConstants() const
{
    return mComputeComputedConstants;
}

CellmlFileRuntime::ComputeRates CellmlFileRuntime::Impl::computeRates() const
{
    return mComputeRates;
}

CellmlFileRuntime::ComputeVariablesForAlgebraicModel CellmlFileRuntime::Impl::computeVariablesForAlgebraicModel() const
{
    return mComputeVariablesForAlgebraicModel;
}

CellmlFileRuntime::ComputeVariablesForDifferentialModel CellmlFileRuntime::Impl::computeVariablesForDifferentialModel() const
{
    return mComputeVariablesForDifferentialModel;
}

CellmlFileRuntime::CellmlFileRuntime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
    : Logger(new Impl {pCellmlFile, pNlaSolver})
{
}

CellmlFileRuntime::~CellmlFileRuntime()
{
    delete pimpl();
}

CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

CellmlFileRuntimePtr CellmlFileRuntime::create(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    return CellmlFileRuntimePtr {new CellmlFileRuntime {pCellmlFile, pNlaSolver}};
}

CellmlFileRuntime::InitialiseVariablesForAlgebraicModel CellmlFileRuntime::initialiseVariablesForAlgebraicModel() const
{
    return pimpl()->initialiseVariablesForAlgebraicModel();
}

CellmlFileRuntime::InitialiseVariablesForDifferentialModel CellmlFileRuntime::initialiseVariablesForDifferentialModel() const
{
    return pimpl()->initialiseVariablesForDifferentialModel();
}

CellmlFileRuntime::ComputeComputedConstants CellmlFileRuntime::computeComputedConstants() const
{
    return pimpl()->computeComputedConstants();
}

CellmlFileRuntime::ComputeRates CellmlFileRuntime::computeRates() const
{
    return pimpl()->computeRates();
}

CellmlFileRuntime::ComputeVariablesForAlgebraicModel CellmlFileRuntime::computeVariablesForAlgebraicModel() const
{
    return pimpl()->computeVariablesForAlgebraicModel();
}

CellmlFileRuntime::ComputeVariablesForDifferentialModel CellmlFileRuntime::computeVariablesForDifferentialModel() const
{
    return pimpl()->computeVariablesForDifferentialModel();
}

} // namespace libOpenCOR

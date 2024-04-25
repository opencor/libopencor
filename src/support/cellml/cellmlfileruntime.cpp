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

#include "compiler.h"

namespace libOpenCOR {

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver, bool pCompiled)
{
    auto cellmlFileAnalyser = pCellmlFile->analyser();

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser);
    } else {
        // Get either a compiled or an interpreted version of the runtime.

#ifndef __EMSCRIPTEN__
        if (pCompiled) {
            // Determine the type of the model.

            auto differentialModel = libOpenCOR::differentialModel(pCellmlFile);

            // Generate some code for the given CellML file.

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

            if (pNlaSolver != nullptr) {
                mNlaSolverAddress = nlaSolverAddress(pNlaSolver.get());

                generatorProfile->setExternNlaSolveMethodString("typedef unsigned long long size_t;\n"
                                                                "\n"
                                                                "extern void nlaSolve(const char *, void (*objectiveFunction)(double *, double *, void *),\n"
                                                                "                     double *u, size_t n, void *data);\n");
                generatorProfile->setNlaSolveCallString(differentialModel,
                                                        std::string("nlaSolve(\"") + mNlaSolverAddress + "\", objectiveFunction[INDEX], u, [SIZE], &rfi);\n");
            }

            generator->setModel(pCellmlFile->analyserModel());
            generator->setProfile(generatorProfile);

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

            auto cellmlFileType = pCellmlFile->type();

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
                mInitialiseVariablesForDifferentialModel = reinterpret_cast<InitialiseVariablesForDifferentialModelFunction>(mCompiler->function("initialiseVariables"));
                mComputeComputedConstants = reinterpret_cast<ComputeComputedConstantsFunction>(mCompiler->function("computeComputedConstants"));
                mComputeRates = reinterpret_cast<ComputeRatesFunction>(mCompiler->function("computeRates"));
                mComputeVariablesForDifferentialModel = reinterpret_cast<ComputeVariablesForDifferentialModelFunction>(mCompiler->function("computeVariables"));

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
                mInitialiseVariablesForAlgebraicModel = reinterpret_cast<InitialiseVariablesForAlgebraicModelFunction>(mCompiler->function("initialiseVariables"));
                mComputeComputedConstants = reinterpret_cast<ComputeComputedConstantsFunction>(mCompiler->function("computeComputedConstants"));
                mComputeVariablesForAlgebraicModel = reinterpret_cast<ComputeVariablesForAlgebraicModelFunction>(mCompiler->function("computeVariables"));

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
        } else {
#endif
            mInterpreter = libcellml::Interpreter::create();

            mInterpreter->setModel(pCellmlFile->analyserModel());
#ifndef __EMSCRIPTEN__
        }
#endif
    }
}

#ifndef __EMSCRIPTEN__
CellmlFileRuntime::Impl::~Impl()
{
    delete[] mNlaSolverAddress;
}
#endif

CellmlFileRuntime::CellmlFileRuntime(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver, bool pCompiled)
    : Logger(new Impl {pCellmlFile, pNlaSolver, pCompiled})
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

const CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

CellmlFileRuntimePtr CellmlFileRuntime::create(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver,
                                               bool pCompiled)
{
    return CellmlFileRuntimePtr {new CellmlFileRuntime {pCellmlFile, pNlaSolver, pCompiled}};
}

bool CellmlFileRuntime::isCompiled() const
{
    return pimpl()->mInterpreter == nullptr;
}

libcellml::InterpreterPtr CellmlFileRuntime::interpreter() const
{
    return pimpl()->mInterpreter;
}

#ifndef __EMSCRIPTEN__
CellmlFileRuntime::InitialiseVariablesForAlgebraicModelFunction CellmlFileRuntime::initialiseCompiledVariablesForAlgebraicModel() const
{
    return pimpl()->mInitialiseVariablesForAlgebraicModel;
}

CellmlFileRuntime::InitialiseVariablesForDifferentialModelFunction CellmlFileRuntime::initialiseCompiledVariablesForDifferentialModel() const
{
    return pimpl()->mInitialiseVariablesForDifferentialModel;
}

CellmlFileRuntime::ComputeComputedConstantsFunction CellmlFileRuntime::computeCompiledComputedConstants() const
{
    return pimpl()->mComputeComputedConstants;
}

CellmlFileRuntime::ComputeRatesFunction CellmlFileRuntime::computeCompiledRates() const
{
    return pimpl()->mComputeRates;
}

CellmlFileRuntime::ComputeVariablesForAlgebraicModelFunction CellmlFileRuntime::computeCompiledVariablesForAlgebraicModel() const
{
    return pimpl()->mComputeVariablesForAlgebraicModel;
}

CellmlFileRuntime::ComputeVariablesForDifferentialModelFunction CellmlFileRuntime::computeCompiledVariablesForDifferentialModel() const
{
    return pimpl()->mComputeVariablesForDifferentialModel;
}
#endif

void CellmlFileRuntime::initialiseInterpretedVariables() const
{
    pimpl()->mInterpreter->initialiseVariables();
}

void CellmlFileRuntime::computeInterpretedComputedConstants() const
{
    pimpl()->mInterpreter->computeComputedConstants();
}

void CellmlFileRuntime::computeInterpretedRates(double pVoi) const
{
    pimpl()->mInterpreter->computeRates(pVoi);
}

void CellmlFileRuntime::computeInterpretedVariables(double pVoi) const
{
    pimpl()->mInterpreter->computeVariables(pVoi);
}

} // namespace libOpenCOR

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

#ifdef __EMSCRIPTEN__
// Some utilities.

namespace {

std::string exportJavaScriptName(const std::string &pName)
{
    return std::string("__attribute__((export_name(\"").append(pName).append("\")))\n");
}

void *instantiateWebAssemblyModule(UnsignedChars pWasmModule, bool pDifferentialModel, bool pIsOdeModel, bool pIsAlgebraicModel)
{
    // clang-format off
    return EM_ASM_PTR({
        try {
            const wasmBytes = new Uint8Array(HEAPU8.subarray($0, $0 + $1));
            const wasmModule = new WebAssembly.Module(wasmBytes);
            const wasmInstance = new WebAssembly.Instance(wasmModule, {
                env: {
                    __linear_memory: wasmMemory,
                    __indirect_function_table: wasmTable,

                    // Some standard C library functions.

                    memset: function(ptr, value, size) {
                        HEAPU8.fill(value, ptr, ptr + size);

                        return ptr;
                    },

                    // Arithmetic operators.

                    pow: Math.pow,
                    sqrt: Math.sqrt,
                    fabs: Math.abs,
                    exp: Math.exp,
                    log: Math.log,
                    log10: Math.log10,
                    ceil: Math.ceil,
                    floor: Math.floor,
                    fmin: Math.min,
                    fmax: Math.max,
                    fmod: function(x, y) { return x % y; },

                    // Trigonometric operators.

                    sin: Math.sin,
                    cos: Math.cos,
                    tan: Math.tan,

                    sinh: Math.sinh,
                    cosh: Math.cosh,
                    tanh: Math.tanh,

                    asin: Math.asin,
                    acos: Math.acos,
                    atan: Math.atan,

                    asinh: Math.asinh,
                    acosh: Math.acosh,
                    atanh: Math.atanh,

                    // Constants.

                    INFINITY: function() { return Infinity; },
                    NAN: function() { return NaN; }
                }
            });

            if ($2) {
                Module.initialiseVariables = wasmInstance.exports.initialiseVariables;
                Module.computeComputedConstants = wasmInstance.exports.computeComputedConstants;
                Module.computeRates = wasmInstance.exports.computeRates;
                Module.computeVariables = wasmInstance.exports.computeVariables;

                if ((Module.initialiseVariables === undefined)
                    || (Module.computeComputedConstants === undefined)
                    || (Module.computeRates === undefined)
                    || (Module.computeVariables === undefined)) {
                    throw new Error("The functions needed to compute the " + ($3 ? "ODE" : "DAE") + " model could not be retrieved.");
                }
            } else {
                Module.initialiseVariables = wasmInstance.exports.initialiseVariables;
                Module.computeComputedConstants = wasmInstance.exports.computeComputedConstants;
                Module.computeVariables = wasmInstance.exports.computeVariables;

                if ((Module.initialiseVariables === undefined)
                    || (Module.computeComputedConstants === undefined)
                    || (Module.computeVariables === undefined)) {
                    throw new Error("The functions needed to compute the " + ($4 ? "algebraic" : "NLA") + " model could not be retrieved.");
                }
            }

            return null;
        } catch (error) {
            const errorMessage = error.toString();
            const errorMessageLength = lengthBytesUTF8(errorMessage) + 1;
            const errorMessagePtr = _malloc(errorMessageLength);

            stringToUTF8(errorMessage, errorMessagePtr, errorMessageLength);

            return errorMessagePtr;
        }
    }, pWasmModule.data(), pWasmModule.size(), pDifferentialModel, pIsOdeModel, pIsAlgebraicModel); // clang-format on
}

} // namespace
#endif

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    auto cellmlFileAnalyser = pCellmlFile->analyser();

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser);
    } else {
//---ISSUE468--- NEED TO INVESTIGATE HOW TO HANDLE nlaSolve() FROM JavaScript.
#ifndef __EMSCRIPTEN__
        // Get an NLA solver, if needed.

        if (pNlaSolver != nullptr) {
            mNlaSolverAddress = nlaSolverAddress(pNlaSolver.get());
        }
#endif

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

        static constexpr auto WITH_EXTERNAL_VARIABLES = false;

#ifdef __EMSCRIPTEN__
        // Specify the name of the functions that we want to export.

        generatorProfile->setImplementationInitialiseVariablesMethodString(differentialModel,
                                                                           exportJavaScriptName("initialiseVariables").append(generatorProfile->implementationInitialiseVariablesMethodString(differentialModel)));
        generatorProfile->setImplementationComputeComputedConstantsMethodString(exportJavaScriptName("computeComputedConstants").append(generatorProfile->implementationComputeComputedConstantsMethodString()));
        generatorProfile->setImplementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES,
                                                                    exportJavaScriptName("computeRates").append(generatorProfile->implementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES)));
        generatorProfile->setImplementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                                        exportJavaScriptName("computeVariables").append(generatorProfile->implementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES)));
#endif

//---ISSUE468--- NEED TO INVESTIGATE HOW TO HANDLE nlaSolve() FROM JavaScript.
#ifndef __EMSCRIPTEN__
        if (pNlaSolver != nullptr) {
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long long size_t;

extern void nlaSolve(uintptr_t nlaSolverAddress, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    std::string("nlaSolve(").append(mNlaSolverAddress).append(", objectiveFunction[INDEX], u, [SIZE], &rfi);\n"));
        }
#endif

        generator->setModel(pCellmlFile->analyserModel());
        generator->setProfile(generatorProfile);

        // Compile the generated code.

        mCompiler = Compiler::create();

#ifdef __EMSCRIPTEN__
        if (!mCompiler->compile(generator->implementationCode(), mWasmModule)) {
            // The compilation failed, so add the issues it generated.

            addIssues(mCompiler);

            return;
        }

        // Instantiate the WebAssembly module.

        auto errorMessagePtr = instantiateWebAssemblyModule(mWasmModule, differentialModel,
                                                            cellmlFileType == libcellml::AnalyserModel::Type::ODE,
                                                            cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC);

        if (errorMessagePtr != nullptr) {
            std::string jsErrorMessage(reinterpret_cast<char *>(errorMessagePtr));

            free(errorMessagePtr);

            addError(std::string("The WebAssembly module could not be instantiated (").append(jsErrorMessage).append(")."));

            return;
        }
#else
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
                addError(std::string("The functions needed to compute the ").append((cellmlFileType == libcellml::AnalyserModel::Type::ODE) ? "ODE" : "DAE").append(" model could not be retrieved."));
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
                addError(std::string("The functions needed to compute the ").append((cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC) ? "algebraic" : "NLA").append(" model could not be retrieved."));
            }
#    endif
        }
#endif
    }
}

CellmlFileRuntime::Impl::~Impl()
{
//---ISSUE468--- NEED TO INVESTIGATE HOW TO HANDLE nlaSolve() FROM JavaScript.
#ifndef __EMSCRIPTEN__
    delete[] mNlaSolverAddress;
#endif
}

#ifdef __EMSCRIPTEN__
EM_JS(void, jsInitialiseVariablesForAlgebraicModel, (double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.initialiseVariables(pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::initialiseVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsInitialiseVariablesForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsInitialiseVariablesForDifferentialModel, (double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.initialiseVariables(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::initialiseVariablesForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsInitialiseVariablesForDifferentialModel(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeComputedConstants, (double *pConstants, double *pComputedConstants), {
    Module.computeComputedConstants(pConstants, pComputedConstants);
});

void CellmlFileRuntime::Impl::computeComputedConstants(double *pConstants, double *pComputedConstants) const
{
    jsComputeComputedConstants(pConstants, pComputedConstants);
}

EM_JS(void, jsComputeRates, (double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.computeRates(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeRates(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeVariablesForAlgebraicModel, (double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.computeVariables(pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeVariablesForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeVariablesForDifferentialModel, (double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.computeVariables(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeVariablesForDifferentialModel(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}
#else
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
#endif

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

#ifdef __EMSCRIPTEN__
void CellmlFileRuntime::initialiseVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->initialiseVariablesForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::initialiseVariablesForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->initialiseVariablesForDifferentialModel(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::computeComputedConstants(double *pConstants, double *pComputedConstants) const
{
    pimpl()->computeComputedConstants(pConstants, pComputedConstants);
}

void CellmlFileRuntime::computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->computeRates(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->computeVariablesForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->computeVariablesForDifferentialModel(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}
#else
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
#endif

} // namespace libOpenCOR

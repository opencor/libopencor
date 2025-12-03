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

intptr_t instantiateWebAssemblyModule(UnsignedChars pWasmModule, bool pDifferentialModel, bool pIsOdeModel,
                                      bool pIsAlgebraicModel, bool pHasObjectiveFunctions)
{
    // clang-format off
    return EM_ASM_INT({
        try {
            // Instantiate the WebAssembly module.

            const wasmBytes = new Uint8Array(HEAPU8.subarray($0, $0 + $1));
            const wasmModule = new WebAssembly.Module(wasmBytes);

            if (Module.wasmInstanceFunctions === undefined) {
                Module.wasmInstanceFunctions = new Map();
                Module.wasmInstanceFunctionsId = 0;
            }

            const wasmInstanceFunctionsId = ++Module.wasmInstanceFunctionsId;
            const wasmInstance = new WebAssembly.Instance(wasmModule, {
                env: {
                    __linear_memory: wasmMemory,
                    __indirect_function_table: wasmTable,

                    // Some standard C library functions.

                    free: _free,
                    malloc: _malloc,
                    memset: _memset,

                    // NLA solve function.

                    nlaSolve: function(nlaSolverAddress, objectiveFunctionIndex, u, n, data) {
                        Module.nlaSolve(Number(nlaSolverAddress), wasmInstanceFunctionsId, Number(objectiveFunctionIndex), Number(u), Number(n), Number(data));
                    },

                    // Arithmetic operators.

                    pow: _pow,
                    sqrt: _sqrt,
                    fabs: _fabs,
                    exp: _exp,
                    log: _log,
                    log10: _log10,
                    ceil: _ceil,
                    floor: _floor,
                    fmin: _fmin,
                    fmax: _fmax,
                    fmod: _fmod,

                    // Trigonometric operators.

                    sin: _sin,
                    cos: _cos,
                    tan: _tan,
                    sinh: _sinh,
                    cosh: _cosh,
                    tanh: _tanh,
                    asin: _asin,
                    acos: _acos,
                    atan: _atan,
                    asinh: _asinh,
                    acosh: _acosh,
                    atanh: _atanh,
                }
            });

            // Retrieve the functions needed to compute the model.

            const wasmInstanceFunctions = {};

            if ($2) {
                wasmInstanceFunctions.initialiseArrays = wasmInstance.exports.initialiseArrays;
                wasmInstanceFunctions.computeComputedConstants = wasmInstance.exports.computeComputedConstants;
                wasmInstanceFunctions.computeRates = wasmInstance.exports.computeRates;
                wasmInstanceFunctions.computeVariables = wasmInstance.exports.computeVariables;

                if ((wasmInstanceFunctions.initialiseArrays === undefined)
                    || (wasmInstanceFunctions.computeComputedConstants === undefined)
                    || (wasmInstanceFunctions.computeRates === undefined)
                    || (wasmInstanceFunctions.computeVariables === undefined)) {
                    throw new Error("The functions needed to compute the " + ($3 ? "ODE" : "DAE") + " model could not be retrieved.");
                }
            } else {
                wasmInstanceFunctions.initialiseArrays = wasmInstance.exports.initialiseArrays;
                wasmInstanceFunctions.computeComputedConstants = wasmInstance.exports.computeComputedConstants;
                wasmInstanceFunctions.computeVariables = wasmInstance.exports.computeVariables;

                if ((wasmInstanceFunctions.initialiseArrays === undefined)
                    || (wasmInstanceFunctions.computeComputedConstants === undefined)
                    || (wasmInstanceFunctions.computeVariables === undefined)) {
                    throw new Error("The functions needed to compute the " + ($4 ? "algebraic" : "NLA") + " model could not be retrieved.");
                }
            }

            // Retrieve the objective functions, if any.

            if ($5) {
                wasmInstanceFunctions.objectiveFunctions = {};

                for (let name in wasmInstance.exports) {
                    if (name.startsWith("objectiveFunction")) {
                        const objectiveFunctionIndex = parseInt(name.replace("objectiveFunction", ""), 10);

                        wasmInstanceFunctions.objectiveFunctions[objectiveFunctionIndex] = wasmInstance.exports[name];
                    }
                }

                if (Object.keys(wasmInstanceFunctions.objectiveFunctions).length === 0) {
                    throw new Error("The objective functions could not be retrieved.");
                }
            }

            // Store the WASM instance functions.

            Module.wasmInstanceFunctions.set(wasmInstanceFunctionsId, wasmInstanceFunctions);

            return wasmInstanceFunctionsId;
        } catch (error) {
            const errorMessage = error.toString();
            const errorMessageLength = lengthBytesUTF8(errorMessage) + 1;
            const errorMessagePtr = _malloc(errorMessageLength);

            stringToUTF8(errorMessage, errorMessagePtr, errorMessageLength);

            // Return the error message pointer as a negative number to indicate an error.

            return -errorMessagePtr;
        }
    }, pWasmModule.data(), pWasmModule.size(), pDifferentialModel, pIsOdeModel, pIsAlgebraicModel, pHasObjectiveFunctions); // clang-format on
}

} // namespace
#endif

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
    auto cellmlFileAnalyser {pCellmlFile->analyser()};

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser, "Analyser");
    } else {
        // Get an NLA solver, if needed.

        if (pNlaSolver != nullptr) {
            mNlaSolverAddress = nlaSolverAddress(pNlaSolver.get());
        }

        // Determine the type of the model.

        auto cellmlFileType {pCellmlFile->type()};
        auto differentialModel {(cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                                || (cellmlFileType == libcellml::AnalyserModel::Type::DAE)};

        // Generate some code for the given CellML file.

        auto generator {libcellml::Generator::create()};
        auto generatorProfile {libcellml::GeneratorProfile::create()};

        generatorProfile->setOriginCommentString("");
        generatorProfile->setImplementationHeaderString("");
        generatorProfile->setImplementationVersionString("");
        generatorProfile->setImplementationStateCountString("");
        generatorProfile->setImplementationConstantCountString("");
        generatorProfile->setImplementationComputedConstantCountString("");
        generatorProfile->setImplementationAlgebraicVariableCountString("");
        generatorProfile->setImplementationExternalVariableCountString("");
        generatorProfile->setImplementationLibcellmlVersionString("");
        generatorProfile->setImplementationVoiInfoString("");
        generatorProfile->setImplementationStateInfoString("");
        generatorProfile->setImplementationConstantInfoString("");
        generatorProfile->setImplementationComputedConstantInfoString("");
        generatorProfile->setImplementationAlgebraicVariableInfoString("");
        generatorProfile->setImplementationExternalVariableInfoString("");
        generatorProfile->setImplementationCreateStatesArrayMethodString("");
        generatorProfile->setImplementationCreateConstantsArrayMethodString("");
        generatorProfile->setImplementationCreateComputedConstantsArrayMethodString("");
        generatorProfile->setImplementationCreateAlgebraicVariablesArrayMethodString("");
        generatorProfile->setImplementationCreateExternalVariablesArrayMethodString("");
        generatorProfile->setImplementationDeleteArrayMethodString("");

        static constexpr auto WITH_EXTERNAL_VARIABLES {false};

#ifdef __EMSCRIPTEN__
        // Allocate the memory needed by our objective functions on the heap rather than on the stack.

        if (pNlaSolver != nullptr) {
            if (differentialModel) {
                generatorProfile->setFindRootMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                          R"(void findRoot[INDEX](double voi, double *states, double *rates, double *constants, double *computedConstants, double *algebraicVariables)
{
    RootFindingInfo *rfi = (RootFindingInfo *) malloc(sizeof(RootFindingInfo));
    double *u = (double *) malloc([SIZE] * sizeof(double));

    rfi->voi = voi;
    rfi->states = states;
    rfi->rates = rates;
    rfi->constants = constants;
    rfi->computedConstants = computedConstants;
    rfi->algebraicVariables = algebraicVariables;

[CODE]

    free(u);
    free(rfi);
}
)");
            } else {
                generatorProfile->setFindRootMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                          R"(void findRoot[INDEX](double *constants, double *computedConstants, double *algebraicVariables)
{
    RootFindingInfo *rfi = (RootFindingInfo *) malloc(sizeof(RootFindingInfo));
    double *u = (double *) malloc([SIZE] * sizeof(double));

    rfi->constants = constants;
    rfi->computedConstants = computedConstants;
    rfi->algebraicVariables = algebraicVariables;

[CODE]

    free(u);
    free(rfi);
}
)");
            }
        }

        // Export our various methods.

        generatorProfile->setImplementationInitialiseArraysMethodString(differentialModel,
                                                                        exportJavaScriptName("initialiseArrays").append(generatorProfile->implementationInitialiseArraysMethodString(differentialModel)));
        generatorProfile->setImplementationComputeComputedConstantsMethodString(differentialModel, exportJavaScriptName("computeComputedConstants").append(generatorProfile->implementationComputeComputedConstantsMethodString(differentialModel)));
        generatorProfile->setImplementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES,
                                                                    exportJavaScriptName("computeRates").append(generatorProfile->implementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES)));
        generatorProfile->setImplementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                                        exportJavaScriptName("computeVariables").append(generatorProfile->implementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES)));
#endif

        if (pNlaSolver != nullptr) {
            // Note: both uintptr_t and size_t are defined as follows:
            //        - Emscripten (wasm32): unsigned int (which is the same as unsigned long on 32 bits and is what we
            //                               need to use here since malloc() expects an unsigned long);
            //        - Windows (64 bits): unsigned long long; and
            //        - Linux/macOS (64 bits): unsigned long.

#ifdef __EMSCRIPTEN__
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long size_t;

extern void *malloc(size_t size);
extern void free(void *ptr);

extern void nlaSolve(uintptr_t nlaSolverAddress, size_t objectiveFunctionIndex, uintptr_t u, size_t n, uintptr_t data);
)");
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    std::string("nlaSolve(").append(mNlaSolverAddress).append(", [INDEX], (uintptr_t) u, [SIZE], (uintptr_t) rfi);\n"));
#else
#    ifdef BUILDING_USING_MSVC
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long long uintptr_t;
typedef unsigned long long size_t;

extern void nlaSolve(uintptr_t nlaSolverAddress, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
#    else
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long size_t;

extern void nlaSolve(uintptr_t nlaSolverAddress, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
#    endif
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    std::string("nlaSolve(").append(mNlaSolverAddress).append(", objectiveFunction[INDEX], u, [SIZE], &rfi);\n"));
#endif
        }

#ifdef __EMSCRIPTEN__
        // Export our various objective functions.

        auto implementationCode {generator->implementationCode(pCellmlFile->analyserModel(), generatorProfile)};

        if (pNlaSolver != nullptr) {
            std::vector<size_t> handledNlaSystemIndices;

            for (const auto &analyserEquation : pCellmlFile->analyserModel()->analyserEquations()) {
                if (analyserEquation->type() == libcellml::AnalyserEquation::Type::NLA) {
                    auto nlaSystemIndex {analyserEquation->nlaSystemIndex()};

                    if (std::find(handledNlaSystemIndices.begin(), handledNlaSystemIndices.end(), nlaSystemIndex) == handledNlaSystemIndices.end()) {
                        auto objectiveFunctionName {std::string("objectiveFunction").append(std::to_string(nlaSystemIndex))};

                        implementationCode.insert(implementationCode.find("void " + objectiveFunctionName),
                                                  exportJavaScriptName(objectiveFunctionName));

                        handledNlaSystemIndices.push_back(nlaSystemIndex);
                    }
                }
            }
        }
#endif

        // Compile the generated code.

        mCompiler = Compiler::create();

#ifdef __EMSCRIPTEN__
        if (!mCompiler->compile(implementationCode, mWasmModule)) {
            // The compilation failed, so add the issues it generated.

            addIssues(mCompiler, "Compiler");

            return;
        }

        // Instantiate the WebAssembly module.

        auto wasmInstanceFunctionsId {instantiateWebAssemblyModule(mWasmModule, differentialModel,
                                                                   cellmlFileType == libcellml::AnalyserModel::Type::ODE,
                                                                   cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC,
                                                                   pNlaSolver != nullptr)};

        if (wasmInstanceFunctionsId < 0) {
            // An error occurred and the error message pointer is the negative of the returned value.

            auto errorMessagePtr {reinterpret_cast<char *>(-wasmInstanceFunctionsId)};
            std::string jsErrorMessage(errorMessagePtr);

            free(errorMessagePtr);

            addError(std::string("The WebAssembly module could not be instantiated (").append(jsErrorMessage).append(")."));

            return;
        }

        // Keep track of the WASM instance functions ID.

        mWasmInstanceFunctionsId = wasmInstanceFunctionsId;
#else
#    ifdef CODE_COVERAGE_ENABLED
        mCompiler->compile(generator->implementationCode(pCellmlFile->analyserModel(), generatorProfile));
#    else
        if (!mCompiler->compile(generator->implementationCode(pCellmlFile->analyserModel(), generatorProfile))) {
            // The compilation failed, so add the issues it generated.

            addIssues(mCompiler, "Compiler");

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
                addIssues(mCompiler, "Compiler");

                return;
            }
#    endif
        }

        // Retrieve our algebraic/differential functions and make sure that we managed to retrieve them.

        if (differentialModel) {
            mInitialiseArraysForDifferentialModel = reinterpret_cast<InitialiseArraysForDifferentialModel>(mCompiler->function("initialiseArrays"));
            mComputeComputedConstantsForDifferentialModel = reinterpret_cast<ComputeComputedConstantsForDifferentialModel>(mCompiler->function("computeComputedConstants"));
            mComputeRates = reinterpret_cast<ComputeRates>(mCompiler->function("computeRates"));
            mComputeVariablesForDifferentialModel = reinterpret_cast<ComputeVariablesForDifferentialModel>(mCompiler->function("computeVariables"));

#    ifndef CODE_COVERAGE_ENABLED
            if ((mInitialiseArraysForDifferentialModel == nullptr)
                || (mComputeComputedConstantsForDifferentialModel == nullptr)
                || (mComputeRates == nullptr)
                || (mComputeVariablesForDifferentialModel == nullptr)) {
                addError(std::string("The functions needed to compute the ").append((cellmlFileType == libcellml::AnalyserModel::Type::ODE) ? "ODE" : "DAE").append(" model could not be retrieved."));
            }
#    endif
        } else {
            mInitialiseArraysForAlgebraicModel = reinterpret_cast<InitialiseArraysForAlgebraicModel>(mCompiler->function("initialiseArrays"));
            mComputeComputedConstantsForAlgebraicModel = reinterpret_cast<ComputeComputedConstantsForAlgebraicModel>(mCompiler->function("computeComputedConstants"));
            mComputeVariablesForAlgebraicModel = reinterpret_cast<ComputeVariablesForAlgebraicModel>(mCompiler->function("computeVariables"));

#    ifndef CODE_COVERAGE_ENABLED
            if ((mInitialiseArraysForAlgebraicModel == nullptr)
                || (mComputeComputedConstantsForAlgebraicModel == nullptr)
                || (mComputeVariablesForAlgebraicModel == nullptr)) {
                addError(std::string("The functions needed to compute the ").append((cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC) ? "algebraic" : "NLA").append(" model could not be retrieved."));
            }
#    endif
        }
#endif
    }
}

#ifdef __EMSCRIPTEN__
EM_JS(void, jsInitialiseArraysForAlgebraicModel, (intptr_t pWasmInstanceFunctionsId, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).initialiseArrays(pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::initialiseArraysForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsInitialiseArraysForAlgebraicModel(mWasmInstanceFunctionsId, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsInitialiseArraysForDifferentialModel, (intptr_t pWasmInstanceFunctionsId, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).initialiseArrays(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::initialiseArraysForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsInitialiseArraysForDifferentialModel(mWasmInstanceFunctionsId, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeComputedConstantsForAlgebraicModel, (intptr_t pWasmInstanceFunctionsId, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).computeComputedConstants(pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeComputedConstantsForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeComputedConstantsForAlgebraicModel(mWasmInstanceFunctionsId, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeComputedConstantsForDifferentialModel, (intptr_t pWasmInstanceFunctionsId, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).computeComputedConstants(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeComputedConstantsForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeComputedConstantsForDifferentialModel(mWasmInstanceFunctionsId, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeRates, (intptr_t pWasmInstanceFunctionsId, double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).computeRates(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeRates(mWasmInstanceFunctionsId, pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeVariablesForAlgebraicModel, (intptr_t pWasmInstanceFunctionsId, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).computeVariables(pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeVariablesForAlgebraicModel(mWasmInstanceFunctionsId, pConstants, pComputedConstants, pAlgebraic);
}

EM_JS(void, jsComputeVariablesForDifferentialModel, (intptr_t pWasmInstanceFunctionsId, double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic), {
    Module.wasmInstanceFunctions.get(pWasmInstanceFunctionsId).computeVariables(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
});

void CellmlFileRuntime::Impl::computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    jsComputeVariablesForDifferentialModel(mWasmInstanceFunctionsId, pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}
#else
CellmlFileRuntime::InitialiseArraysForAlgebraicModel CellmlFileRuntime::Impl::initialiseArraysForAlgebraicModel() const
{
    return mInitialiseArraysForAlgebraicModel;
}

CellmlFileRuntime::InitialiseArraysForDifferentialModel CellmlFileRuntime::Impl::initialiseArraysForDifferentialModel() const
{
    return mInitialiseArraysForDifferentialModel;
}

CellmlFileRuntime::ComputeComputedConstantsForAlgebraicModel CellmlFileRuntime::Impl::computeComputedConstantsForAlgebraicModel() const
{
    return mComputeComputedConstantsForAlgebraicModel;
}

CellmlFileRuntime::ComputeComputedConstantsForDifferentialModel CellmlFileRuntime::Impl::computeComputedConstantsForDifferentialModel() const
{
    return mComputeComputedConstantsForDifferentialModel;
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
void CellmlFileRuntime::initialiseArraysForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->initialiseArraysForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::initialiseArraysForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->initialiseArraysForDifferentialModel(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::computeComputedConstantsForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->computeComputedConstantsForAlgebraicModel(pConstants, pComputedConstants, pAlgebraic);
}

void CellmlFileRuntime::computeComputedConstantsForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraic) const
{
    pimpl()->computeComputedConstantsForDifferentialModel(pStates, pRates, pConstants, pComputedConstants, pAlgebraic);
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
CellmlFileRuntime::InitialiseArraysForAlgebraicModel CellmlFileRuntime::initialiseArraysForAlgebraicModel() const
{
    return pimpl()->initialiseArraysForAlgebraicModel();
}

CellmlFileRuntime::InitialiseArraysForDifferentialModel CellmlFileRuntime::initialiseArraysForDifferentialModel() const
{
    return pimpl()->initialiseArraysForDifferentialModel();
}

CellmlFileRuntime::ComputeComputedConstantsForAlgebraicModel CellmlFileRuntime::computeComputedConstantsForAlgebraicModel() const
{
    return pimpl()->computeComputedConstantsForAlgebraicModel();
}

CellmlFileRuntime::ComputeComputedConstantsForDifferentialModel CellmlFileRuntime::computeComputedConstantsForDifferentialModel() const
{
    return pimpl()->computeComputedConstantsForDifferentialModel();
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

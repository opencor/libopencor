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

#include <format>
#include <unordered_set>

namespace libOpenCOR {

CellmlFileRuntime::Impl::Impl(const CellmlFilePtr &pCellmlFile, const SolverNlaPtr &pNlaSolver)
{
#ifndef __EMSCRIPTEN__
    (void)pNlaSolver;
#endif

    auto cellmlFileAnalyser {pCellmlFile->analyser()};

    if (cellmlFileAnalyser->errorCount() != 0) {
        addIssues(cellmlFileAnalyser, "Analyser");
    } else {
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

        auto exportJavaScriptName = [](const std::string &pName) -> std::string {
            std::string exportName;

            exportName.reserve(pName.size() + 31); // NOLINT

            exportName += "__attribute__((export_name(\"";
            exportName += pName;
            exportName += "\")))\n";

            return exportName;
        };

        auto prependExportName = [&exportJavaScriptName](const std::string &pName, const std::string &pCode) {
            auto exportName {exportJavaScriptName(pName)};
            std::string res;

            res.reserve(exportName.size() + pCode.size());

            res += exportName;
            res += pCode;

            return res;
        };

        generatorProfile->setImplementationInitialiseArraysMethodString(differentialModel,
                                                                        prependExportName("initialiseArrays", generatorProfile->implementationInitialiseArraysMethodString(differentialModel)));
        generatorProfile->setImplementationComputeComputedConstantsMethodString(differentialModel,
                                                                                prependExportName("computeComputedConstants", generatorProfile->implementationComputeComputedConstantsMethodString(differentialModel)));
        generatorProfile->setImplementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES,
                                                                    prependExportName("computeRates", generatorProfile->implementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES)));
        generatorProfile->setImplementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                                        prependExportName("computeVariables", generatorProfile->implementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES)));
#endif

        if (pNlaSolver != nullptr) {
            // Note: both uintptr_t and size_t are defined as follows:
            //        - Emscripten (wasm32): unsigned int (which is the same as unsigned long on 32 bits and is what we
            //          need to use here since malloc() expects an unsigned long);
            //        - Windows (64 bits): unsigned long long; and
            //        - Linux/macOS (64 bits): unsigned long.

#ifdef __EMSCRIPTEN__
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long size_t;

extern void *malloc(size_t size);
extern void free(void *ptr);

extern uintptr_t nlaSolverAddress();
extern void nlaSolve(uintptr_t nlaSolverAddress, size_t computeObjectiveFunctionIndex, uintptr_t u, size_t n, uintptr_t data);
)");
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    "nlaSolve(nlaSolverAddress(), [INDEX], (uintptr_t) u, [SIZE], (uintptr_t) rfi);\n");
#else
#    ifdef BUILDING_USING_MSVC
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long long uintptr_t;
typedef unsigned long long size_t;

extern uintptr_t nlaSolverAddress();
extern void nlaSolve(uintptr_t nlaSolverAddress, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
#    else
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long size_t;

extern uintptr_t nlaSolverAddress();
extern void nlaSolve(uintptr_t nlaSolverAddress, void (*objectiveFunction)(double *, double *, void *),
                     double *u, size_t n, void *data);
)");
#    endif
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    "nlaSolve(nlaSolverAddress(), objectiveFunction[INDEX], u, [SIZE], &rfi);\n");
#endif
        }

#ifdef __EMSCRIPTEN__
        // Export our various objective functions.

        auto implementationCode {generator->implementationCode(pCellmlFile->analyserModel(), generatorProfile)};

        if (pNlaSolver != nullptr) {
            std::unordered_set<size_t> handledNlaSystemIndices;
            const auto &analyserEquations = pCellmlFile->analyserModel()->analyserEquations();

            handledNlaSystemIndices.reserve(analyserEquations.size());

            for (const auto &analyserEquation : analyserEquations) {
                if (analyserEquation->type() == libcellml::AnalyserEquation::Type::NLA) {
                    auto nlaSystemIndex {analyserEquation->nlaSystemIndex()};

                    if (!handledNlaSystemIndices.contains(nlaSystemIndex)) {
                        auto objectiveFunctionName {"objectiveFunction" + std::format("{}", nlaSystemIndex)};

                        implementationCode.insert(implementationCode.find("void " + objectiveFunctionName),
                                                  exportJavaScriptName(objectiveFunctionName));

                        handledNlaSystemIndices.insert(nlaSystemIndex);
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
            auto functionAdded =
#    endif
                mCompiler->addFunction("nlaSolverAddress", reinterpret_cast<void *>(nlaSolverAddress));

#    ifndef CODE_COVERAGE_ENABLED
            if (!functionAdded) {
                addIssues(mCompiler, "Compiler");

                return;
            }

            functionAdded =
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
                addError(std::string("The functions needed to compute the ")
                         + ((cellmlFileType == libcellml::AnalyserModel::Type::ODE) ? "ODE" : "DAE")
                         + " model could not be retrieved.");
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
                addError(std::string("The functions needed to compute the ")
                         + ((cellmlFileType == libcellml::AnalyserModel::Type::ALGEBRAIC) ? "algebraic" : "NLA")
                         + " model could not be retrieved.");
            }
#    endif
        }
#endif
    }
}

#ifdef __EMSCRIPTEN__
CellmlFileRuntime::Impl::~Impl()
{
    cleanupWorkerWasm();
}

// Lazily create a WebAssembly.Module + Instance in the current worker's private JavaScript scope, so dispatch methods
// can call the CellML-generated functions directly on the worker thread.

// clang-format off
EM_JS(void, initialiseWorkerWasmJS, (const void* wasmBytesPtr, size_t wasmBytesSize), {
    // Create a WebAssembly.Module + Instance in the current worker's private JavaScript scope, so dispatch methods can
    // call the CellML-generated functions directly on the worker thread.

    const wasmBytes = new Uint8Array(HEAPU8.buffer, wasmBytesPtr, wasmBytesSize);
    const wasmModule = new WebAssembly.Module(wasmBytes);
    const wasmInstance = new WebAssembly.Instance(wasmModule, {
        env: {
            __linear_memory: wasmMemory,
            __indirect_function_table: wasmTable,

            // Some standard C library functions.

            free: _free,
            malloc: _malloc,
            memset: _memset,

            // NLA solve function.

            nlaSolverAddress: function() {
                return globalThis.runtime.nlaSolverAddress;
            },
            nlaSolve: function(nlaSolverAddress, objectiveFunctionIndex, u, n, data) {
                Module.nlaSolve(nlaSolverAddress, objectiveFunctionIndex, u, n, data);
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
            atanh: _atanh
        }
    });
    const exports = wasmInstance.exports;
    const runtime = {
        initialiseArrays: exports.initialiseArrays,
        computeComputedConstants: exports.computeComputedConstants,
        computeRates: exports.computeRates,
        computeVariables: exports.computeVariables,
        computeObjectiveFunctions: {},
        computeObjectiveFunctionCount: 0
    };

    for (const key in exports) {
        if (key.indexOf("objectiveFunction") === 0) {
            runtime.computeObjectiveFunctions[parseInt(key.substring(17), 10)] = exports[key];

            ++runtime.computeObjectiveFunctionCount;
        }
    }

    globalThis.runtime = runtime;
}); // clang-format on

void CellmlFileRuntime::Impl::initialiseWorkerWasm() const
{
    initialiseWorkerWasmJS(mWasmModule.data(), mWasmModule.size());
}

void CellmlFileRuntime::Impl::cleanupWorkerWasm() const
{
    // clang-format off
    EM_ASM({
        delete globalThis.runtime;
    }); // clang-format on
}

void CellmlFileRuntime::Impl::setNlaSolverAddress(uintptr_t pAddress) const
{
    // clang-format off
    EM_ASM({
        globalThis.runtime.nlaSolverAddress = $0;
    }, pAddress); // clang-format on
}

// clang-format off
EM_JS(void, initialiseArraysForAlgebraicModelJS, (const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.initialiseArrays(constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::initialiseArraysForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    initialiseArraysForAlgebraicModelJS(pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, initialiseArraysForDifferentialModelJS, (const void* states, const void* rates, const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.initialiseArrays(states, rates, constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::initialiseArraysForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    initialiseArraysForDifferentialModelJS(pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, computeComputedConstantsForAlgebraicModelJS, (const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.computeComputedConstants(constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::computeComputedConstantsForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    computeComputedConstantsForAlgebraicModelJS(pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, computeComputedConstantsForDifferentialModelJS, (double voi, const void* states, const void* rates, const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.computeComputedConstants(voi, states, rates, constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::computeComputedConstantsForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    computeComputedConstantsForDifferentialModelJS(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, computeRatesJS, (double voi, const void* states, const void* rates, const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.computeRates(voi, states, rates, constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    computeRatesJS(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, computeVariablesForAlgebraicModelJS, (const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.computeVariables(constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    computeVariablesForAlgebraicModelJS(pConstants, pComputedConstants, pAlgebraicVariables);
}

// clang-format off
EM_JS(void, computeVariablesForDifferentialModelJS, (double voi, const void* states, const void* rates, const void* constants, const void* computedConstants, const void* algebraicVariables), {
    globalThis.runtime.computeVariables(voi, states, rates, constants, computedConstants, algebraicVariables);
}); // clang-format on

void CellmlFileRuntime::Impl::computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    computeVariablesForDifferentialModelJS(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
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
void CellmlFileRuntime::initialiseWorkerWasm() const
{
    pimpl()->initialiseWorkerWasm();
}

void CellmlFileRuntime::cleanupWorkerWasm() const
{
    pimpl()->cleanupWorkerWasm();
}

void CellmlFileRuntime::setNlaSolverAddress(uintptr_t pAddress) const
{
    pimpl()->setNlaSolverAddress(pAddress);
}

void CellmlFileRuntime::initialiseArraysForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->initialiseArraysForAlgebraicModel(pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::initialiseArraysForDifferentialModel(double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->initialiseArraysForDifferentialModel(pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::computeComputedConstantsForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->computeComputedConstantsForAlgebraicModel(pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::computeComputedConstantsForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->computeComputedConstantsForDifferentialModel(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::computeRates(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->computeRates(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::computeVariablesForAlgebraicModel(double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->computeVariablesForAlgebraicModel(pConstants, pComputedConstants, pAlgebraicVariables);
}

void CellmlFileRuntime::computeVariablesForDifferentialModel(double pVoi, double *pStates, double *pRates, double *pConstants, double *pComputedConstants, double *pAlgebraicVariables) const
{
    pimpl()->computeVariablesForDifferentialModel(pVoi, pStates, pRates, pConstants, pComputedConstants, pAlgebraicVariables);
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

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
#include <vector>

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

        // Restrict-qualify the pointer parameters of computeRates() and computeVariables() since they never point to
        // the same array (i.e. our ODE solvers and SedInstanceTask never pass the same array twice), something that
        // allows LLVM to optimise our code further (e.g., by not reloading the value of a state after having computed
        // a rate).

        if (differentialModel) {
            generatorProfile->setImplementationComputeRatesMethodString(WITH_EXTERNAL_VARIABLES,
                                                                        "void computeRates(double voi, double * restrict states, double * restrict rates, double * restrict constants, double * restrict computedConstants, double * restrict algebraicVariables)\n"
                                                                        "{\n"
                                                                        "[CODE]"
                                                                        "}\n");
            generatorProfile->setImplementationComputeVariablesMethodString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                                            "void computeVariables(double voi, double * restrict states, double * restrict rates, double * restrict constants, double * restrict computedConstants, double * restrict algebraicVariables)\n"
                                                                            "{\n"
                                                                            "[CODE]"
                                                                            "}\n");
        }

#ifdef __EMSCRIPTEN__
        // Note: our objective functions keep their data on the stack, as they do natively, and our WebAssembly instances
        //       have their own stack (see initialiseWorkerWasm()), the size of which is bounded by the number of variables
        //       in our model (since the unknowns of an NLA system are variables of our model).

        const auto analyserModel {pCellmlFile->analyserModel()};
        const auto variableCount {analyserModel->stateCount() + analyserModel->constantCount()
                                  + analyserModel->computedConstantCount() + analyserModel->algebraicVariableCount()};

        mWasmStackSize = WASM_STACK_BASE_SIZE + (WASM_STACK_SIZE_PER_VARIABLE * variableCount);

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
            //        - Emscripten (wasm32): unsigned int (which is the same as unsigned long on 32 bits, which is what we
            //          use here);
            //        - Windows (64 bits): unsigned long long; and
            //        - Linux/macOS (64 bits): unsigned long.

#ifdef __EMSCRIPTEN__
            generatorProfile->setExternNlaSolveMethodString(R"(typedef unsigned long uintptr_t;
typedef unsigned long size_t;

extern uintptr_t nlaSolverAddress();
extern void nlaSolve(uintptr_t nlaSolverAddress, size_t objectiveFunctionIndex, double *u, size_t n, void *data);
)");
            generatorProfile->setNlaSolveCallString(differentialModel, WITH_EXTERNAL_VARIABLES,
                                                    "nlaSolve(nlaSolverAddress(), [INDEX], u, [SIZE], &rfi);\n");
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
// Lazily create a WebAssembly.Module + Instance in the current thread's private JavaScript scope and install its
// exported functions into the current thread's WebAssembly table, so C++ can call them directly through function
// pointers without any JavaScript round trip. If some table slots have already been allocated in that table, then reuse
// them rather than grow the table again, which would otherwise both grow the table unboundedly and keep previously
// created WebAssembly instances alive forever.
// Note: we keep track of our table slots using the table itself rather than the current thread since a table belongs
//       to a JavaScript worker, which is reused by different threads over time (e.g., each asynchronous run is done in
//       a new thread, but most likely by the same worker).

// clang-format off
EM_JS(int, initialiseWorkerWasmJS, (const void* wasmBytesPtr, size_t wasmBytesSize, const void* wasmStackTop), {
    // Create a WebAssembly.Module + Instance in the current thread's private JavaScript scope, so C++ can call its
    // exported functions directly through the current thread's WebAssembly table.
    // Note: our instance has its own stack (see initialiseWorkerWasm()), the top of which must be 16-byte aligned.

    const wasmBytes = new Uint8Array(HEAPU8.buffer, wasmBytesPtr, wasmBytesSize);
    const wasmModule = new WebAssembly.Module(wasmBytes);
    const wasmInstance = new WebAssembly.Instance(wasmModule, {
        env: {
            __linear_memory: wasmMemory,
            __indirect_function_table: wasmTable,
            __stack_pointer: new WebAssembly.Global({ value: "i32", mutable: true }, wasmStackTop & ~15),

            // Some standard C library functions.

            memset: _memset,

            // NLA solve functions.
            // Note: these are exports of our main WebAssembly module, so they are called directly.

            nlaSolverAddress: _nlaSolverAddress,
            nlaSolve: _wasmNlaSolve,

            // Arithmetic operators.
            // Note: LLVM may optimise pow(b, x), where b is a power of two (e.g., 2.0, 4.0, or 0.5), to exp2(n*x), so we
            //       also need exp2().

            pow: _pow,
            sqrt: _sqrt,
            fabs: _fabs,
            exp: _exp,
            exp2: _exp2,
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

    // Install the exported functions into the shared WebAssembly table (i.e. the main module's table, so C++ can call
    // them), in an order known to our getters:
    //   - Slot 0: initialiseArrays();
    //   - Slot 1: computeComputedConstants();
    //   - Slot 2: computeRates() (only exported by differential models, so the slot is left empty otherwise);
    //   - Slot 3: computeVariables(); and
    //   - Slot 4+i: objectiveFunction<i>() (see wasmNlaSolve()).

    const functionNames = ["initialiseArrays", "computeComputedConstants", "computeRates", "computeVariables"];
    const objectiveFunctionIndices = [];

    for (const key in exports) {
        if (key.indexOf("objectiveFunction") === 0) {
            objectiveFunctionIndices.push(parseInt(key.substring(17), 10));
        }
    }

    // Reuse the table slots that we have already allocated in this table, if there are enough of them, otherwise
    // allocate new ones (and release the old ones).

    const functionCount = functionNames.length + ((objectiveFunctionIndices.length === 0) ? 0 : (Math.max(...objectiveFunctionIndices) + 1));
    let slots = wasmTable.libOpenCORSlots;

    if ((slots === undefined) || (slots.count < functionCount)) {
        if (slots !== undefined) {
            for (let i = 0; i < slots.count; ++i) {
                wasmTable.set(slots.base + i, null);
            }
        }

        slots = { base: wasmTable.grow(functionCount), count: functionCount };

        wasmTable.libOpenCORSlots = slots;
    }

    // Install our functions, installing each objective function at a slot determined by its NLA system index, so that
    // C++ can compute its table slot (see wasmNlaSolve()). Also, clear the slots that we don't need (e.g., the slot of
    // computeRates() for an algebraic model), so that no previously created WebAssembly instance is kept alive.

    for (let i = 0; i < slots.count; ++i) {
        const func = (i < functionNames.length) ? exports[functionNames[i]] : exports["objectiveFunction" + (i - functionNames.length)];

        wasmTable.set(slots.base + i, (func === undefined) ? null : func);
    }

    return slots.base;
}); // clang-format on

// The base index, in the current thread's WebAssembly table, of the current thread's runtime functions (each thread
// lazily creates its own WebAssembly instance, see initialiseWorkerWasmJS()). This is thread-local since the table
// slots are only valid on the thread on which they were installed.
// Note: the function pointers returned by computeRates(), initialiseArraysForDifferentialModel(), etc. are computed
//       from this thread-local base. They must only be called from the current thread (calling them from a different
//       thread, which has its own WebAssembly instance with different table slots, will silently execute the wrong
//       code). Solvers that cache these function pointers (see SolverOde::mComputeRates and
//       SolverCvodeUserData::computeRates) inherit this thread-affinity constraint.
// Similarly, sWasmStack is the stack of the current thread's WebAssembly instance (see initialiseWorkerWasmJS()). It is
// shared by all our instances on the current thread (since only one of them can be used at a time) and only ever grows.

namespace {
thread_local int sWasmFunctionBase = 0; // NOLINT
thread_local std::vector<double> sWasmStack; // NOLINT
} // namespace

void CellmlFileRuntime::Impl::initialiseWorkerWasm() const
{
    const auto wasmStackSize {(mWasmStackSize + sizeof(double) - 1) / sizeof(double)};

    if (sWasmStack.size() < wasmStackSize) {
        sWasmStack.resize(wasmStackSize);
    }

    sWasmFunctionBase = initialiseWorkerWasmJS(mWasmModule.data(), mWasmModule.size(), sWasmStack.data() + sWasmStack.size());
}

// The table slot offsets of the functions of our WebAssembly instances (see initialiseWorkerWasmJS()):
//   - Slot 0: initialiseArrays();
//   - Slot 1: computeComputedConstants();
//   - Slot 2: computeRates() (differential models only);
//   - Slot 3: computeVariables(); and
//   - Slot 4+i: objectiveFunction<i>().

static constexpr intptr_t INITIALISE_ARRAYS_TABLE_OFFSET = 0;
static constexpr intptr_t COMPUTE_COMPUTED_CONSTANTS_TABLE_OFFSET = 1;
static constexpr intptr_t COMPUTE_RATES_TABLE_OFFSET = 2;
static constexpr intptr_t COMPUTE_VARIABLES_TABLE_OFFSET = 3;
static constexpr intptr_t OBJECTIVE_FUNCTIONS_TABLE_OFFSET = 4;

// The function that our generated code calls to solve an NLA system (see initialiseWorkerWasmJS()), with the objective
// function given by its NLA system index. It is called directly from WebAssembly and it simply resolves the table slot
// of the objective function (a function pointer being a table slot in WebAssembly) before calling nlaSolve().

extern "C" void wasmNlaSolve(uintptr_t pNlaSolverAddress, size_t pObjectiveFunctionIndex, double *pU, size_t pN, void *pData)
{
    nlaSolve(pNlaSolverAddress,
             reinterpret_cast<SolverNla::ComputeObjectiveFunction>(sWasmFunctionBase + OBJECTIVE_FUNCTIONS_TABLE_OFFSET + static_cast<intptr_t>(pObjectiveFunctionIndex)),
             pU, pN, pData);
}

CellmlFileRuntime::InitialiseArraysForAlgebraicModel CellmlFileRuntime::Impl::initialiseArraysForAlgebraicModel() const
{
    return reinterpret_cast<InitialiseArraysForAlgebraicModel>(sWasmFunctionBase + INITIALISE_ARRAYS_TABLE_OFFSET);
}

CellmlFileRuntime::InitialiseArraysForDifferentialModel CellmlFileRuntime::Impl::initialiseArraysForDifferentialModel() const
{
    return reinterpret_cast<InitialiseArraysForDifferentialModel>(sWasmFunctionBase + INITIALISE_ARRAYS_TABLE_OFFSET);
}

CellmlFileRuntime::ComputeComputedConstantsForAlgebraicModel CellmlFileRuntime::Impl::computeComputedConstantsForAlgebraicModel() const
{
    return reinterpret_cast<ComputeComputedConstantsForAlgebraicModel>(sWasmFunctionBase + COMPUTE_COMPUTED_CONSTANTS_TABLE_OFFSET);
}

CellmlFileRuntime::ComputeComputedConstantsForDifferentialModel CellmlFileRuntime::Impl::computeComputedConstantsForDifferentialModel() const
{
    return reinterpret_cast<ComputeComputedConstantsForDifferentialModel>(sWasmFunctionBase + COMPUTE_COMPUTED_CONSTANTS_TABLE_OFFSET);
}

CellmlFileRuntime::ComputeRates CellmlFileRuntime::Impl::computeRates() const
{
    return reinterpret_cast<ComputeRates>(sWasmFunctionBase + COMPUTE_RATES_TABLE_OFFSET);
}

CellmlFileRuntime::ComputeVariablesForAlgebraicModel CellmlFileRuntime::Impl::computeVariablesForAlgebraicModel() const
{
    return reinterpret_cast<ComputeVariablesForAlgebraicModel>(sWasmFunctionBase + COMPUTE_VARIABLES_TABLE_OFFSET);
}

CellmlFileRuntime::ComputeVariablesForDifferentialModel CellmlFileRuntime::Impl::computeVariablesForDifferentialModel() const
{
    return reinterpret_cast<ComputeVariablesForDifferentialModel>(sWasmFunctionBase + COMPUTE_VARIABLES_TABLE_OFFSET);
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
    : Logger(std::make_unique<Impl>(pCellmlFile, pNlaSolver))
{
#ifdef CODE_COVERAGE_ENABLED
    (void)pimpl();
#endif
}

CellmlFileRuntime::~CellmlFileRuntime() = default;

CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const CellmlFileRuntime::Impl *CellmlFileRuntime::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
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
#endif

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

} // namespace libOpenCOR

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

#include "file_p.h"
#include "sedchangeattribute_p.h"
#include "sedinstancetask_p.h"
#include "sedmodel_p.h"
#include "sedtask_p.h"
#include "seduniformtimecourse_p.h"
#include "solvernla_p.h"
#include "solverode_p.h"

#include <cstdint>
#include <condition_variable>
#include <mutex>

namespace libOpenCOR {

#ifdef __EMSCRIPTEN__
// clang-format off
EM_JS(intptr_t, toFloat64ArrayJS, (const void* data, size_t size), {
    if (size === 0) {
        return Emval.toHandle(new Float64Array(0));
    }

    return Emval.toHandle(new Float64Array(HEAPU8.buffer, data, size));
}); // clang-format on

static emscripten::val toFloat64Array(const Doubles &data)
{
    return emscripten::val::take_ownership(reinterpret_cast<emscripten::EM_VAL>(toFloat64ArrayJS(data.data(), data.size())));
}
#endif

SedInstanceTaskPtr SedInstanceTask::Impl::create(const SedAbstractTaskPtr &pTask)
{
    auto res {SedInstanceTaskPtr {new SedInstanceTask {pTask}}};

    res->pimpl()->mOwner = res;

    // Initialise the instance task, but only if there are no issues with it.

#ifndef CODE_COVERAGE_ENABLED
    if (!res->hasIssues()) {
#endif
        res->pimpl()->initialise();
#ifndef CODE_COVERAGE_ENABLED
    }
#endif

    return res;
}

SedInstanceTask::Impl::Impl(const SedAbstractTaskPtr &pTask)
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT SedTask TASKS, HENCE WE ASSERT (FOR NOW) THAT pTask IS INDEED A SedTask
    //          OBJECT.

    auto task {std::dynamic_pointer_cast<SedTask>(pTask)};

    ASSERT_NE(task, nullptr);

    // Get a runtime for the model.

    mModel = task->pimpl()->mModel;

    auto cellmlFile {mModel->pimpl()->mFile->pimpl()->mCellmlFile};
    auto cellmlFileType {cellmlFile->type()};

    mDifferentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                         || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);
    mSimulation = task->pimpl()->mSimulation;
    mSedUniformTimeCourse = mDifferentialModel ? std::dynamic_pointer_cast<SedUniformTimeCourse>(mSimulation) : nullptr;

    const auto &odeSolver {mSimulation->odeSolver()};
    const auto &nlaSolver {mSimulation->nlaSolver()};

    mOdeSolver = (odeSolver != nullptr) ? std::dynamic_pointer_cast<SolverOde>(odeSolver->pimpl()->duplicate()) : nullptr;
    mNlaSolver = (nlaSolver != nullptr) ? std::dynamic_pointer_cast<SolverNla>(nlaSolver->pimpl()->duplicate()) : nullptr;
    mRuntime = cellmlFile->runtime(mNlaSolver);

#ifndef CODE_COVERAGE_ENABLED
    if (mRuntime->hasErrors()) {
        addIssues(mRuntime, "Runtime");

        return;
    }
#endif

    // Create our various arrays.

    mAnalyserModel = cellmlFile->analyserModel();
    mStateCount = mAnalyserModel->stateCount();
    mConstantCount = mAnalyserModel->constantCount();
    mComputedConstantCount = mAnalyserModel->computedConstantCount();
    mAlgebraicVariableCount = mAnalyserModel->algebraicVariableCount();

    if (mDifferentialModel) {
        mStateDoubles.resize(mStateCount, NAN);
        mRateDoubles.resize(mStateCount, NAN);

        mStates = mStateDoubles.data();
        mRates = mRateDoubles.data();

        mResults.states.resize(mStateCount, {});
        mResults.rates.resize(mStateCount, {});
    }

    mConstantDoubles.resize(mConstantCount, NAN);
    mComputedConstantDoubles.resize(mComputedConstantCount, NAN);
    mAlgebraicVariableDoubles.resize(mAlgebraicVariableCount, NAN);

    mConstants = mConstantDoubles.data();
    mComputedConstants = mComputedConstantDoubles.data();
    mAlgebraicVariables = mAlgebraicVariableDoubles.data();

    mResults.constants.resize(mConstantCount, {});
    mResults.computedConstants.resize(mComputedConstantCount, {});
    mResults.algebraicVariables.resize(mAlgebraicVariableCount, {});

    // Retrieve our various strings.

    if (mDifferentialModel) {
        auto variable = mAnalyserModel->voi()->variable();

        mVoiName = name(variable);
        mVoiUnit = variable->units()->name();

        const auto stateCount = mStateCount;

        mStateNames.resize(stateCount);
        mStateUnits.resize(stateCount);

        mRateNames.resize(stateCount);
        mRateUnits.resize(stateCount);

        const auto &states = mAnalyserModel->states();

        for (size_t i {0}; i < stateCount; ++i) {
            mStateNames[i] = name(states[i]->variable());
            mStateUnits[i] = states[i]->variable()->units()->name();

            mRateNames[i] = mStateNames[i] + "'";
            mRateUnits[i] = mStateUnits[i] + "/" + mVoiUnit;
        }
    }

    const auto constantCount = mConstantCount;

    mConstantNames.resize(constantCount);
    mConstantUnits.resize(constantCount);

    const auto &constants = mAnalyserModel->constants();

    for (size_t i {0}; i < constantCount; ++i) {
        mConstantNames[i] = name(constants[i]->variable());
        mConstantUnits[i] = constants[i]->variable()->units()->name();
    }

    const auto computedConstantCount = mComputedConstantCount;

    mComputedConstantNames.resize(computedConstantCount);
    mComputedConstantUnits.resize(computedConstantCount);

    const auto &computedConstants = mAnalyserModel->computedConstants();

    for (size_t i {0}; i < computedConstantCount; ++i) {
        mComputedConstantNames[i] = name(computedConstants[i]->variable());
        mComputedConstantUnits[i] = computedConstants[i]->variable()->units()->name();
    }

    const auto algebraicVariableCount = mAlgebraicVariableCount;

    mAlgebraicVariableNames.resize(algebraicVariableCount);
    mAlgebraicVariableUnits.resize(algebraicVariableCount);

    const auto &algebraicVariables = mAnalyserModel->algebraicVariables();

    for (size_t i {0}; i < algebraicVariableCount; ++i) {
        mAlgebraicVariableNames[i] = name(algebraicVariables[i]->variable());
        mAlgebraicVariableUnits[i] = algebraicVariables[i]->variable()->units()->name();
    }
}

void SedInstanceTask::Impl::trackResults(size_t pIndex)
{
    mResults.voi[pIndex] = mVoi;

    auto &states = mResults.states;
    auto &rates = mResults.rates;
    auto &constants = mResults.constants;
    auto &computedConstants = mResults.computedConstants;
    auto &algebraicVariables = mResults.algebraicVariables;

    for (size_t i {0}; i < mStateCount; ++i) {
        states[i][pIndex] = mStates[i]; // NOLINT
        rates[i][pIndex] = mRates[i]; // NOLINT
    }

    for (size_t i {0}; i < mConstantCount; ++i) {
        constants[i][pIndex] = mConstants[i]; // NOLINT
    }

    for (size_t i {0}; i < mComputedConstantCount; ++i) {
        computedConstants[i][pIndex] = mComputedConstants[i]; // NOLINT
    }

    for (size_t i {0}; i < mAlgebraicVariableCount; ++i) {
        algebraicVariables[i][pIndex] = mAlgebraicVariables[i]; // NOLINT
    }
}

void SedInstanceTask::Impl::applyChanges()
{
    for (const auto &change : mModel->changes()) {
        //---GRY--- AT THIS STAGE, WE ONLY SUPPORT ChangeAttribute CHANGES, HENCE WE ASSERT (FOR NOW) THAT change IS
        //          INDEED A SedChangeAttribute OBJECT.

        auto changeAttribute {std::dynamic_pointer_cast<SedChangeAttribute>(change)};

        ASSERT_NE(changeAttribute, nullptr);

        changeAttribute->pimpl()->apply(mOwner.lock(), mAnalyserModel);

        addIssues(changeAttribute, "Change attribute");
    }
}

void SedInstanceTask::Impl::initialise()
{
#ifdef __EMSCRIPTEN__
    // Initialise our per-worker WASM runtime data.

    mRuntime->initialiseWorkerWasm();
#endif

    // Set the NLA solver address so JIT-compiled code can resolve it at runtime.

    if (mNlaSolver != nullptr) {
#ifdef __EMSCRIPTEN__
        mRuntime->setNlaSolverAddress(reinterpret_cast<uintptr_t>(mNlaSolver.get()));
#else
        setNlaSolverAddress(reinterpret_cast<uintptr_t>(mNlaSolver.get()));
#endif
    }

    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.

    if (mSedUniformTimeCourse != nullptr) {
        mVoi = mSedUniformTimeCourse->pimpl()->mInitialTime;

#ifdef __EMSCRIPTEN__
        mRuntime->initialiseArraysForDifferentialModel(mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#else
        mRuntime->initialiseArraysForDifferentialModel()(mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#endif
    } else {
#ifdef __EMSCRIPTEN__
        mRuntime->initialiseArraysForAlgebraicModel(mConstants, mComputedConstants, mAlgebraicVariables);
#else
        mRuntime->initialiseArraysForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraicVariables);
#endif
    }

    applyChanges();

    if (mSedUniformTimeCourse != nullptr) {
#ifdef __EMSCRIPTEN__
        mRuntime->computeComputedConstantsForDifferentialModel(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeRates(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeVariablesForDifferentialModel(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#else
        mRuntime->computeComputedConstantsForDifferentialModel()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeRates()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#endif
    } else {
#ifdef __EMSCRIPTEN__
        mRuntime->computeComputedConstantsForAlgebraicModel(mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeVariablesForAlgebraicModel(mConstants, mComputedConstants, mAlgebraicVariables);
#else
        mRuntime->computeComputedConstantsForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraicVariables);
        mRuntime->computeVariablesForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraicVariables);
#endif
    }

    // Make sure that the NLA solver, should it have been used, didn't report any issues.

    if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
        addIssues(mNlaSolver, mNlaSolver->name());

        return;
    }

    // Initialise the ODE solver, if needed.

    if (mDifferentialModel) {
        if (!mOdeSolver->pimpl()->initialise(mVoi, mStateCount, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables, mRuntime)) {
            addIssues(mOdeSolver, mOdeSolver->name());

            return;
        }
    }
}

void SedInstanceTask::Impl::run(double pVoiStart, double pVoiEnd, double pVoiInterval, bool pTrackResults)
{
    // Track our initial results.

    size_t index {0};

    if (pTrackResults) {
        trackResults(index);
    }

    // Compute the differential model.

    auto *odeSolverPimpl {mOdeSolver->pimpl()};
    size_t voiCounter {0};

#ifndef __EMSCRIPTEN__
    const auto computeVariablesForDifferentialModel = mRuntime->computeVariablesForDifferentialModel();
#endif

    while (!fuzzyCompare(mVoi, pVoiEnd)) {
        // Check whether a pause or stop has been requested.

        const auto runControl = mRunControl->load(std::memory_order_relaxed);

        if ((runControl & INSTANCE_RUN_CONTROL_PAUSE) != 0) {
            std::unique_lock<std::mutex> pauseLock(*mPauseMutex);

            mPauseConditionVariable->wait(pauseLock, [this]() {
                const auto crtRunControl = mRunControl->load(std::memory_order_relaxed);

                return ((crtRunControl & INSTANCE_RUN_CONTROL_PAUSE) == 0) || ((crtRunControl & INSTANCE_RUN_CONTROL_STOP) != 0);
            });

            if ((mRunControl->load(std::memory_order_relaxed) & INSTANCE_RUN_CONTROL_STOP) != 0) {
                return;
            }
        }

        if ((runControl & INSTANCE_RUN_CONTROL_STOP) != 0) {
            return;
        }

        // Update our model's state.

        if (!odeSolverPimpl->solve(mVoi, std::min(pVoiStart + static_cast<double>(++voiCounter) * pVoiInterval, pVoiEnd))) {
            addIssues(mOdeSolver, mOdeSolver->name());

            return;
        }

#ifdef __EMSCRIPTEN__
        mRuntime->computeVariablesForDifferentialModel(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#else
        computeVariablesForDifferentialModel(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraicVariables);
#endif

        //---GRY--- WE NEED TO CHECK FOR POSSIBLE NLA ISSUES, BUT FOR CODE COVERAGE WE NEED A MODEL THAT WOULD TRIGGER
        //          NLA ISSUES HERE, WHICH WE DON'T HAVE YET HENCE WE DISABLE THE FOLLOWING CODE WHEN DOING CODE
        //          COVERAGE.

#ifndef CODE_COVERAGE_ENABLED
        if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
            addIssues(mNlaSolver, mNlaSolver->name());

            return;
        }
#endif

        // Update our progress.

        mCompletedSteps.fetch_add(1, std::memory_order_relaxed);

        // Track our results, if needed.

        if (pTrackResults) {
            trackResults(++index);
        }
    }
}

double SedInstanceTask::Impl::run()
{
    // Start our timer.

    auto startTime {std::chrono::high_resolution_clock::now()};

    // Reset our progress counters.

    const auto *sedUniformTimeCoursePimpl {mDifferentialModel ? mSedUniformTimeCourse->pimpl() : nullptr};
    const auto totalSteps {mDifferentialModel ? static_cast<size_t>(sedUniformTimeCoursePimpl->mNumberOfSteps) : 1};

    mCompletedSteps.store(0, std::memory_order_relaxed);
    mTotalSteps.store(totalSteps, std::memory_order_relaxed);

    // (Re)initialise our model.
    // Note: reinitialise our model because we initialised it when we created the instance task.

    initialise();

    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    if (mDifferentialModel) {
        // Run our simulation from the initial time to the output start time, without tracking our results.

        const auto voiInterval {(sedUniformTimeCoursePimpl->mOutputEndTime - sedUniformTimeCoursePimpl->mOutputStartTime) / sedUniformTimeCoursePimpl->mNumberOfSteps};

        run(sedUniformTimeCoursePimpl->mInitialTime, sedUniformTimeCoursePimpl->mOutputStartTime, voiInterval, false);

        if (hasIssues()) {
            return 0.0;
        }

        // Initialise our results structure.

        const auto resultsSize {totalSteps + 1};

        mResults.voi.assign(resultsSize, NAN);

        for (size_t i {0}; i < mStateCount; ++i) {
            mResults.states[i].assign(resultsSize, NAN);
            mResults.rates[i].assign(resultsSize, NAN);
        }

        for (size_t i {0}; i < mConstantCount; ++i) {
            mResults.constants[i].assign(resultsSize, NAN);
        }

        for (size_t i {0}; i < mComputedConstantCount; ++i) {
            mResults.computedConstants[i].assign(resultsSize, NAN);
        }

        for (size_t i {0}; i < mAlgebraicVariableCount; ++i) {
            mResults.algebraicVariables[i].assign(resultsSize, NAN);
        }

        // Run our simulation from the output start time to the output end time, tracking our results.

        run(sedUniformTimeCoursePimpl->mOutputStartTime, sedUniformTimeCoursePimpl->mOutputEndTime, voiInterval, true);

        if (hasIssues()) {
            return 0.0;
        }
    } else {
        // Track our results.

        for (size_t i {0}; i < mConstantCount; ++i) {
            mResults.constants[i].assign(1, mConstants[i]); // NOLINT
        }

        for (size_t i {0}; i < mComputedConstantCount; ++i) {
            mResults.computedConstants[i].assign(1, mComputedConstants[i]); // NOLINT
        }

        for (size_t i {0}; i < mAlgebraicVariableCount; ++i) {
            mResults.algebraicVariables[i].assign(1, mAlgebraicVariables[i]); // NOLINT
        }

        mCompletedSteps.store(1, std::memory_order_relaxed);
    }

    // Stop our timer and return the elapsed time in milliseconds.

    return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count();
}

double SedInstanceTask::Impl::progress() const
{
    const auto totalSteps {mTotalSteps.load(std::memory_order_relaxed)};

    if (totalSteps == 0) {
        return 0.0;
    }

    return static_cast<double>(mCompletedSteps.load(std::memory_order_relaxed)) / static_cast<double>(totalSteps);
}

const Doubles &SedInstanceTask::Impl::voi() const
{
    static const Doubles NO_DOUBLES;

    if (mDifferentialModel) {
        return mResults.voi;
    }

    return NO_DOUBLES;
}

const std::string &SedInstanceTask::Impl::voiName() const
{
    static const std::string NO_STRING;

    if (mDifferentialModel) {
        return mVoiName;
    }

    return NO_STRING;
}

const std::string &SedInstanceTask::Impl::voiUnit() const
{
    static const std::string NO_STRING;

    if (mDifferentialModel) {
        return mVoiUnit;
    }

    return NO_STRING;
}

size_t SedInstanceTask::Impl::stateCount() const
{
    return mStateCount;
}

const Doubles &SedInstanceTask::Impl::state(size_t pIndex) const
{
    static const Doubles NO_DOUBLES;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_DOUBLES;
    }

    return mResults.states[pIndex];
}

const std::string &SedInstanceTask::Impl::stateName(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_STRING;
    }

    return mStateNames[pIndex];
}

const std::string &SedInstanceTask::Impl::stateUnit(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_STRING;
    }

    return mStateUnits[pIndex];
}

size_t SedInstanceTask::Impl::rateCount() const
{
    return stateCount();
}

const Doubles &SedInstanceTask::Impl::rate(size_t pIndex) const
{
    static const Doubles NO_DOUBLES;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_DOUBLES;
    }

    return mResults.rates[pIndex];
}

const std::string &SedInstanceTask::Impl::rateName(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_STRING;
    }

    return mRateNames[pIndex];
}

const std::string &SedInstanceTask::Impl::rateUnit(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (!mDifferentialModel || (pIndex >= mStateCount)) {
        return NO_STRING;
    }

    return mRateUnits[pIndex];
}

size_t SedInstanceTask::Impl::constantCount() const
{
    return mConstantCount;
}

const Doubles &SedInstanceTask::Impl::constant(size_t pIndex) const
{
    static const Doubles NO_DOUBLES;

    if (pIndex >= mConstantCount) {
        return NO_DOUBLES;
    }

    return mResults.constants[pIndex];
}

const std::string &SedInstanceTask::Impl::constantName(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mConstantCount) {
        return NO_STRING;
    }

    return mConstantNames[pIndex];
}

const std::string &SedInstanceTask::Impl::constantUnit(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mConstantCount) {
        return NO_STRING;
    }

    return mConstantUnits[pIndex];
}

size_t SedInstanceTask::Impl::computedConstantCount() const
{
    return mComputedConstantCount;
}

const Doubles &SedInstanceTask::Impl::computedConstant(size_t pIndex) const
{
    static const Doubles NO_DOUBLES;

    if (pIndex >= mComputedConstantCount) {
        return NO_DOUBLES;
    }

    return mResults.computedConstants[pIndex];
}

const std::string &SedInstanceTask::Impl::computedConstantName(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mComputedConstantCount) {
        return NO_STRING;
    }

    return mComputedConstantNames[pIndex];
}

const std::string &SedInstanceTask::Impl::computedConstantUnit(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mComputedConstantCount) {
        return NO_STRING;
    }

    return mComputedConstantUnits[pIndex];
}

size_t SedInstanceTask::Impl::algebraicVariableCount() const
{
    return mAlgebraicVariableCount;
}

const Doubles &SedInstanceTask::Impl::algebraicVariable(size_t pIndex) const
{
    static const Doubles NO_DOUBLES;

    if (pIndex >= mAlgebraicVariableCount) {
        return NO_DOUBLES;
    }

    return mResults.algebraicVariables[pIndex];
}

const std::string &SedInstanceTask::Impl::algebraicVariableName(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mAlgebraicVariableCount) {
        return NO_STRING;
    }

    return mAlgebraicVariableNames[pIndex];
}

const std::string &SedInstanceTask::Impl::algebraicVariableUnit(size_t pIndex) const
{
    static const std::string NO_STRING;

    if (pIndex >= mAlgebraicVariableCount) {
        return NO_STRING;
    }

    return mAlgebraicVariableUnits[pIndex];
}

SedInstanceTask::SedInstanceTask(const SedAbstractTaskPtr &pTask)
    : Logger(new Impl(pTask))
{
}

SedInstanceTask::~SedInstanceTask()
{
    delete pimpl();
}

SedInstanceTask::Impl *SedInstanceTask::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const SedInstanceTask::Impl *SedInstanceTask::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

double SedInstanceTask::progress() const
{
    return pimpl()->progress();
}

const Doubles &SedInstanceTask::voi() const
{
    return pimpl()->voi();
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::voiAsArray() const
{
    static thread_local emscripten::val res;
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = voi();
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::voiName() const
{
    return pimpl()->voiName();
}

const std::string &SedInstanceTask::voiUnit() const
{
    return pimpl()->voiUnit();
}

size_t SedInstanceTask::stateCount() const
{
    return pimpl()->stateCount();
}

const Doubles &SedInstanceTask::state(size_t pIndex) const
{
    return pimpl()->state(pIndex);
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::stateAsArray(size_t pIndex) const
{
    static thread_local emscripten::val res;
    static thread_local auto cachedIndex {SIZE_MAX};
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = state(pIndex);
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedIndex != pIndex) || (cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
        cachedIndex = pIndex;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::stateName(size_t pIndex) const
{
    return pimpl()->stateName(pIndex);
}

const std::string &SedInstanceTask::stateUnit(size_t pIndex) const
{
    return pimpl()->stateUnit(pIndex);
}

size_t SedInstanceTask::rateCount() const
{
    return pimpl()->rateCount();
}

const Doubles &SedInstanceTask::rate(size_t pIndex) const
{
    return pimpl()->rate(pIndex);
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::rateAsArray(size_t pIndex) const
{
    static thread_local emscripten::val res;
    static thread_local auto cachedIndex {SIZE_MAX};
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = rate(pIndex);
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedIndex != pIndex) || (cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
        cachedIndex = pIndex;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::rateName(size_t pIndex) const
{
    return pimpl()->rateName(pIndex);
}

const std::string &SedInstanceTask::rateUnit(size_t pIndex) const
{
    return pimpl()->rateUnit(pIndex);
}

size_t SedInstanceTask::constantCount() const
{
    return pimpl()->constantCount();
}

const Doubles &SedInstanceTask::constant(size_t pIndex) const
{
    return pimpl()->constant(pIndex);
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::constantAsArray(size_t pIndex) const
{
    static thread_local emscripten::val res;
    static thread_local auto cachedIndex {SIZE_MAX};
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = constant(pIndex);
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedIndex != pIndex) || (cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
        cachedIndex = pIndex;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::constantName(size_t pIndex) const
{
    return pimpl()->constantName(pIndex);
}

const std::string &SedInstanceTask::constantUnit(size_t pIndex) const
{
    return pimpl()->constantUnit(pIndex);
}

size_t SedInstanceTask::computedConstantCount() const
{
    return pimpl()->computedConstantCount();
}

const Doubles &SedInstanceTask::computedConstant(size_t pIndex) const
{
    return pimpl()->computedConstant(pIndex);
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::computedConstantAsArray(size_t pIndex) const
{
    static thread_local emscripten::val res;
    static thread_local auto cachedIndex {SIZE_MAX};
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = computedConstant(pIndex);
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedIndex != pIndex) || (cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
        cachedIndex = pIndex;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::computedConstantName(size_t pIndex) const
{
    return pimpl()->computedConstantName(pIndex);
}

const std::string &SedInstanceTask::computedConstantUnit(size_t pIndex) const
{
    return pimpl()->computedConstantUnit(pIndex);
}

size_t SedInstanceTask::algebraicVariableCount() const
{
    return pimpl()->algebraicVariableCount();
}

const Doubles &SedInstanceTask::algebraicVariable(size_t pIndex) const
{
    return pimpl()->algebraicVariable(pIndex);
}

#ifdef __EMSCRIPTEN__
const emscripten::val &SedInstanceTask::algebraicVariableAsArray(size_t pIndex) const
{
    static thread_local emscripten::val res;
    static thread_local auto cachedIndex {SIZE_MAX};
    static thread_local const double *cachedDataPtr {nullptr};
    static thread_local auto cachedSize {SIZE_MAX};

    const auto &data = algebraicVariable(pIndex);
    const auto *dataPtr = data.data();
    const auto dataSize = data.size();

    if ((cachedIndex != pIndex) || (cachedDataPtr != dataPtr) || (cachedSize != dataSize)) {
        res = toFloat64Array(data);

        cachedDataPtr = dataPtr;
        cachedSize = dataSize;
        cachedIndex = pIndex;
    }

    return res;
}
#endif

const std::string &SedInstanceTask::algebraicVariableName(size_t pIndex) const
{
    return pimpl()->algebraicVariableName(pIndex);
}

const std::string &SedInstanceTask::algebraicVariableUnit(size_t pIndex) const
{
    return pimpl()->algebraicVariableUnit(pIndex);
}

} // namespace libOpenCOR

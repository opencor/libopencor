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

namespace libOpenCOR {

SedInstanceTaskPtr SedInstanceTask::Impl::create(const SedAbstractTaskPtr &pTask, bool pCompiled)
{
    return SedInstanceTaskPtr {new SedInstanceTask(pTask, pCompiled)};
}

SedInstanceTask::Impl::Impl(const SedAbstractTaskPtr &pTask, bool pCompiled)
    : mCompiled(pCompiled)
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT SedTask TASKS, HENCE WE ASSERT (FOR NOW) THAT pTask IS INDEED A SedTask
    //          OBJECT.

    auto task = std::dynamic_pointer_cast<SedTask>(pTask);

    ASSERT_NE(task, nullptr);

    // Get a runtime for the model.

    mModel = task->pimpl()->mModel;

    auto cellmlFile = mModel->pimpl()->mFile->pimpl()->mCellmlFile;
    auto cellmlFileType = cellmlFile->type();

    mDifferentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                         || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);
    mSimulation = task->pimpl()->mSimulation;

    auto odeSolver = mSimulation->odeSolver();
    auto nlaSolver = mSimulation->nlaSolver();

    mOdeSolver = (odeSolver != nullptr) ? std::dynamic_pointer_cast<SolverOde>(odeSolver->pimpl()->duplicate()) : nullptr;
    mNlaSolver = (nlaSolver != nullptr) ? std::dynamic_pointer_cast<SolverNla>(nlaSolver->pimpl()->duplicate()) : nullptr;
    mRuntime = cellmlFile->runtime(mNlaSolver, mCompiled);

#ifndef CODE_COVERAGE_ENABLED
    if (mRuntime->hasErrors()) {
        addIssues(mRuntime);

        return;
    }
#endif

    // Create our various arrays.

    mAnalyserModel = cellmlFile->analyserModel();

    if (mDifferentialModel) {
        mStateDoubles.resize(mAnalyserModel->stateCount(), NAN);
        mRateDoubles.resize(mAnalyserModel->stateCount(), NAN);

        mStates = mStateDoubles.data();
        mRates = mRateDoubles.data();

        mResults.states.resize(mAnalyserModel->stateCount(), {});
        mResults.rates.resize(mAnalyserModel->stateCount(), {});
    }

    mConstantDoubles.resize(mAnalyserModel->constantCount(), NAN);
    mComputedConstantDoubles.resize(mAnalyserModel->computedConstantCount(), NAN);
    mAlgebraicDoubles.resize(mAnalyserModel->algebraicCount(), NAN);

    mConstants = mConstantDoubles.data();
    mComputedConstants = mComputedConstantDoubles.data();
    mAlgebraic = mAlgebraicDoubles.data();

    mResults.constants.resize(mAnalyserModel->constantCount(), {});
    mResults.computedConstants.resize(mAnalyserModel->computedConstantCount(), {});
    mResults.algebraic.resize(mAnalyserModel->algebraicCount(), {});

    // Initialise our model.

    initialise();
}

void SedInstanceTask::Impl::trackResults(size_t pIndex)
{
    mResults.voi[pIndex] = mVoi;

    for (size_t i = 0; i < mAnalyserModel->stateCount(); ++i) {
        mResults.states[i][pIndex] = mStates[i]; // NOLINT
        mResults.rates[i][pIndex] = mRates[i]; // NOLINT
    }

    for (size_t i = 0; i < mAnalyserModel->constantCount(); ++i) {
        mResults.constants[i][pIndex] = mConstants[i]; // NOLINT
    }

    for (size_t i = 0; i < mAnalyserModel->computedConstantCount(); ++i) {
        mResults.computedConstants[i][pIndex] = mComputedConstants[i]; // NOLINT
    }

    for (size_t i = 0; i < mAnalyserModel->algebraicCount(); ++i) {
        mResults.algebraic[i][pIndex] = mAlgebraic[i]; // NOLINT
    }
}

namespace {

std::string name(const std::string pComponentName, const std::string pVariableName)
{
    return pComponentName + "/" + pVariableName;
}

std::string name(const libcellml::VariablePtr &pVariable)
{
    auto component = std::dynamic_pointer_cast<libcellml::Component>(pVariable->parent());

    return name(component->name(), pVariable->name());
}

} // namespace

void SedInstanceTask::Impl::applyChanges()
{
    for (const auto &change : mModel->changes()) {
        //---GRY--- AT THIS STAGE, WE ONLY SUPPORT ChangeAttribute CHANGES, HENCE WE ASSERT (FOR NOW) THAT change IS
        //          INDEED A SedChnageAttribute OBJECT.

        auto changeAttribute = std::dynamic_pointer_cast<SedChangeAttribute>(change);

        ASSERT_NE(changeAttribute, nullptr);

        // Check whether the change is for a state, a constant, a computed constant, or an algebraic variable.
        // Note: a change cannot be for a variable of integration or a rate, so we don't check for those.

        auto changeName = name(changeAttribute->componentName(), changeAttribute->variableName());
        auto isParameterSet = false;

        for (size_t i = 0; i < mAnalyserModel->stateCount(); ++i) {
            if (stateName(i) == changeName) {
                mStates[i] = toDouble(changeAttribute->newValue()); // NOLINT

                isParameterSet = true;

                break;
            }
        }

        if (!isParameterSet) {
            for (size_t i = 0; i < mAnalyserModel->constantCount(); ++i) {
                if (constantName(i) == changeName) {
                    mConstants[i] = toDouble(changeAttribute->newValue()); // NOLINT

                    isParameterSet = true;

                    break;
                }
            }
        }

        if (!isParameterSet) {
            for (size_t i = 0; i < mAnalyserModel->computedConstantCount(); ++i) {
                if (computedConstantName(i) == changeName) {
                    mComputedConstants[i] = toDouble(changeAttribute->newValue()); // NOLINT

                    isParameterSet = true;

                    break;
                }
            }
        }

        if (!isParameterSet) {
            for (size_t i = 0; i < mAnalyserModel->algebraicCount(); ++i) {
                if (algebraicName(i) == changeName) {
                    mAlgebraic[i] = toDouble(changeAttribute->newValue()); // NOLINT

                    break;
                }
            }
        }
    }
}

void SedInstanceTask::Impl::initialise()
{
    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.

    mSedUniformTimeCourse = mDifferentialModel ? std::dynamic_pointer_cast<SedUniformTimeCourse>(mSimulation) : nullptr;

    if (mSedUniformTimeCourse != nullptr) {
        mVoi = mSedUniformTimeCourse->pimpl()->mOutputStartTime;

#ifndef __EMSCRIPTEN__
        if (mCompiled) {
            mRuntime->initialiseCompiledVariablesForDifferentialModel()(mStates, mRates, mConstants, mComputedConstants, mAlgebraic);

            applyChanges();

            mRuntime->computeCompiledComputedConstants()(mConstants, mComputedConstants);
            mRuntime->computeCompiledRates()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);
            mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);
        } else {
#endif
            mRuntime->initialiseInterpretedVariablesForDifferentialModel()(mStates, mRates, mConstants, mComputedConstants, mAlgebraic);

            applyChanges();

            mRuntime->computeInterpretedComputedConstants()(mConstants, mComputedConstants);
            mRuntime->computeInterpretedRates()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);
            mRuntime->computeInterpretedVariablesForDifferentialModel()(mVoi, mStates, mRates, mConstants, mComputedConstants, mAlgebraic);
#ifndef __EMSCRIPTEN__
        }
    } else if (mCompiled) {
        mRuntime->initialiseCompiledVariablesForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraic);

        applyChanges();

        mRuntime->computeCompiledComputedConstants()(mConstants, mComputedConstants);
        mRuntime->computeCompiledVariablesForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraic);
#endif
    } else {
        mRuntime->initialiseInterpretedVariablesForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraic);

        applyChanges();

        mRuntime->computeInterpretedComputedConstants()(mConstants, mComputedConstants);
        mRuntime->computeInterpretedVariablesForAlgebraicModel()(mConstants, mComputedConstants, mAlgebraic);
    }

    // Make sure that the NLA solver, should it have been used, didn't report any issues.

    if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
        addIssues(mNlaSolver);

        return;
    }

    // Initialise the ODE solver, if needed.

    if (mDifferentialModel) {
        if (!mOdeSolver->pimpl()->initialise(mVoi, mAnalyserModel->stateCount(), mStates, mRates,
                                             mConstants, mComputedConstants, mAlgebraic,
                                             mRuntime->computeCompiledRates(), mRuntime->computeInterpretedRates())) {
            addIssues(mOdeSolver);

            return;
        }
    }
}

double SedInstanceTask::Impl::run()
{
    // Start our timer.

    auto startTime = std::chrono::high_resolution_clock::now();

    // (Re)initialise our model.
    // Note: reinitialise our model in case we are running our model multiple times.

    initialise();

    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    if (mDifferentialModel) {
        // Initialise our results structure.

        auto resultsSize = static_cast<size_t>(mSedUniformTimeCourse->pimpl()->mNumberOfSteps) + 1;

        mResults.voi.resize(resultsSize, NAN);

        for (size_t i = 0; i < mAnalyserModel->stateCount(); ++i) {
            mResults.states[i].resize(resultsSize, NAN);
            mResults.rates[i].resize(resultsSize, NAN);
        }

        for (size_t i = 0; i < mAnalyserModel->constantCount(); ++i) {
            mResults.constants[i].resize(resultsSize, NAN);
        }

        for (size_t i = 0; i < mAnalyserModel->computedConstantCount(); ++i) {
            mResults.computedConstants[i].resize(resultsSize, NAN);
        }

        for (size_t i = 0; i < mAnalyserModel->algebraicCount(); ++i) {
            mResults.algebraic[i].resize(resultsSize, NAN);
        }

        // Track our initial results.

        size_t index = 0;

        trackResults(index);

        // Compute the differential model.

        auto voiStart = mVoi;
        auto voiEnd = mSedUniformTimeCourse->pimpl()->mOutputEndTime;
        auto voiInterval = (voiEnd - mVoi) / mSedUniformTimeCourse->pimpl()->mNumberOfSteps;
        size_t voiCounter = 0;

        while (!fuzzyCompare(mVoi, voiEnd)) {
            if (!mOdeSolver->pimpl()->solve(mVoi, std::min(voiStart + static_cast<double>(++voiCounter) * voiInterval, voiEnd))) {
                addIssues(mOdeSolver);

                return 0.0;
            }

#ifndef __EMSCRIPTEN__
            if (mCompiled) {
                mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates,
                                                                         mConstants, mComputedConstants, mAlgebraic);
            } else {
#endif
                mRuntime->computeInterpretedVariablesForDifferentialModel()(mVoi, mStates, mRates,
                                                                            mConstants, mComputedConstants, mAlgebraic);
#ifndef __EMSCRIPTEN__
            }
#endif

            //---GRY--- WE NEED TO CHECK FOR POSSIBLE NLA ISSUES, BUT FOR CODE COVERAGE WE NEED A MODEL THAT WOULD
            //          TRIGGER NLA ISSUES HERE, WHICH WE DON'T HAVE YET HENCE WE DISABLE THE FOLLOWING CODE WHEN DOING
            //          CODE COVERAGE.

#ifndef CODE_COVERAGE_ENABLED
            if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
                addIssues(mNlaSolver);

                return 0.0;
            }
#endif

            trackResults(++index);
        }
    } else {
        // Track our results.

        for (size_t i = 0; i < mAnalyserModel->constantCount(); ++i) {
            mResults.constants[i].resize(1, mConstants[i]); // NOLINT
        }

        for (size_t i = 0; i < mAnalyserModel->computedConstantCount(); ++i) {
            mResults.computedConstants[i].resize(1, mComputedConstants[i]); // NOLINT
        }

        for (size_t i = 0; i < mAnalyserModel->algebraicCount(); ++i) {
            mResults.algebraic[i].resize(1, mAlgebraic[i]); // NOLINT
        }
    }

    // Stop our timer and return the elapsed time in milliseconds.

    return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count();
}

Doubles SedInstanceTask::Impl::voi() const
{
    if (mDifferentialModel) {
        return mResults.voi;
    }

    return {};
}

std::string SedInstanceTask::Impl::voiName() const
{
    if (mDifferentialModel) {
        return name(mAnalyserModel->voi()->variable());
    }

    return {};
}

std::string SedInstanceTask::Impl::voiUnit() const
{
    if (mDifferentialModel) {
        return mAnalyserModel->voi()->variable()->units()->name();
    }

    return {};
}

size_t SedInstanceTask::Impl::stateCount() const
{
    return mAnalyserModel->stateCount();
}

Doubles SedInstanceTask::Impl::state(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return mResults.states[pIndex];
}

std::string SedInstanceTask::Impl::stateName(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return name(mAnalyserModel->states()[pIndex]->variable());
}

std::string SedInstanceTask::Impl::stateUnit(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return mAnalyserModel->states()[pIndex]->variable()->units()->name();
}

size_t SedInstanceTask::Impl::rateCount() const
{
    return stateCount();
}

Doubles SedInstanceTask::Impl::rate(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return mResults.rates[pIndex];
}

std::string SedInstanceTask::Impl::rateName(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return name(mAnalyserModel->states()[pIndex]->variable()) + "'";
}

std::string SedInstanceTask::Impl::rateUnit(size_t pIndex) const
{
    if (!mDifferentialModel || (pIndex >= mAnalyserModel->stateCount())) {
        return {};
    }

    return mAnalyserModel->states()[pIndex]->variable()->units()->name() + "/" + voiUnit();
}

size_t SedInstanceTask::Impl::constantCount() const
{
    return mAnalyserModel->constantCount();
}

Doubles SedInstanceTask::Impl::constant(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->constantCount()) {
        return {};
    }

    return mResults.constants[pIndex];
}

std::string SedInstanceTask::Impl::constantName(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->constantCount()) {
        return {};
    }

    return name(mAnalyserModel->constants()[pIndex]->variable());
}

std::string SedInstanceTask::Impl::constantUnit(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->constantCount()) {
        return {};
    }

    return mAnalyserModel->constants()[pIndex]->variable()->units()->name();
}

size_t SedInstanceTask::Impl::computedConstantCount() const
{
    return mAnalyserModel->computedConstantCount();
}

Doubles SedInstanceTask::Impl::computedConstant(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->computedConstantCount()) {
        return {};
    }

    return mResults.computedConstants[pIndex];
}

std::string SedInstanceTask::Impl::computedConstantName(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->computedConstantCount()) {
        return {};
    }

    return name(mAnalyserModel->computedConstants()[pIndex]->variable());
}

std::string SedInstanceTask::Impl::computedConstantUnit(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->computedConstantCount()) {
        return {};
    }

    return mAnalyserModel->computedConstants()[pIndex]->variable()->units()->name();
}

size_t SedInstanceTask::Impl::algebraicCount() const
{
    return mAnalyserModel->algebraicCount();
}

Doubles SedInstanceTask::Impl::algebraic(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->algebraicCount()) {
        return {};
    }

    return mResults.algebraic[pIndex];
}

std::string SedInstanceTask::Impl::algebraicName(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->algebraicCount()) {
        return {};
    }

    return name(mAnalyserModel->algebraic()[pIndex]->variable());
}

std::string SedInstanceTask::Impl::algebraicUnit(size_t pIndex) const
{
    if (pIndex >= mAnalyserModel->algebraicCount()) {
        return {};
    }

    return mAnalyserModel->algebraic()[pIndex]->variable()->units()->name();
}

SedInstanceTask::SedInstanceTask(const SedAbstractTaskPtr &pTask, bool pCompiled)
    : Logger(new Impl(pTask, pCompiled))
{
}

SedInstanceTask::~SedInstanceTask()
{
    delete pimpl();
}

SedInstanceTask::Impl *SedInstanceTask::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const SedInstanceTask::Impl *SedInstanceTask::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

Doubles SedInstanceTask::voi() const
{
    return pimpl()->voi();
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::voiAsArray() const
{
    return emscripten::val::array(voi());
}
#endif

std::string SedInstanceTask::voiName() const
{
    return pimpl()->voiName();
}

std::string SedInstanceTask::voiUnit() const
{
    return pimpl()->voiUnit();
}

size_t SedInstanceTask::stateCount() const
{
    return pimpl()->stateCount();
}

Doubles SedInstanceTask::state(size_t pIndex) const
{
    return pimpl()->state(pIndex);
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::stateAsArray(size_t pIndex) const
{
    return emscripten::val::array(state(pIndex));
}
#endif

std::string SedInstanceTask::stateName(size_t pIndex) const
{
    return pimpl()->stateName(pIndex);
}

std::string SedInstanceTask::stateUnit(size_t pIndex) const
{
    return pimpl()->stateUnit(pIndex);
}

size_t SedInstanceTask::rateCount() const
{
    return pimpl()->rateCount();
}

Doubles SedInstanceTask::rate(size_t pIndex) const
{
    return pimpl()->rate(pIndex);
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::rateAsArray(size_t pIndex) const
{
    return emscripten::val::array(rate(pIndex));
}
#endif

std::string SedInstanceTask::rateName(size_t pIndex) const
{
    return pimpl()->rateName(pIndex);
}

std::string SedInstanceTask::rateUnit(size_t pIndex) const
{
    return pimpl()->rateUnit(pIndex);
}

size_t SedInstanceTask::constantCount() const
{
    return pimpl()->constantCount();
}

Doubles SedInstanceTask::constant(size_t pIndex) const
{
    return pimpl()->constant(pIndex);
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::constantAsArray(size_t pIndex) const
{
    return emscripten::val::array(constant(pIndex));
}
#endif

std::string SedInstanceTask::constantName(size_t pIndex) const
{
    return pimpl()->constantName(pIndex);
}

std::string SedInstanceTask::constantUnit(size_t pIndex) const
{
    return pimpl()->constantUnit(pIndex);
}

size_t SedInstanceTask::computedConstantCount() const
{
    return pimpl()->computedConstantCount();
}

Doubles SedInstanceTask::computedConstant(size_t pIndex) const
{
    return pimpl()->computedConstant(pIndex);
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::computedConstantAsArray(size_t pIndex) const
{
    return emscripten::val::array(computedConstant(pIndex));
}
#endif

std::string SedInstanceTask::computedConstantName(size_t pIndex) const
{
    return pimpl()->computedConstantName(pIndex);
}

std::string SedInstanceTask::computedConstantUnit(size_t pIndex) const
{
    return pimpl()->computedConstantUnit(pIndex);
}

size_t SedInstanceTask::algebraicCount() const
{
    return pimpl()->algebraicCount();
}

Doubles SedInstanceTask::algebraic(size_t pIndex) const
{
    return pimpl()->algebraic(pIndex);
}

#ifdef __EMSCRIPTEN__
emscripten::val SedInstanceTask::algebraicAsArray(size_t pIndex) const
{
    return emscripten::val::array(algebraic(pIndex));
}
#endif

std::string SedInstanceTask::algebraicName(size_t pIndex) const
{
    return pimpl()->algebraicName(pIndex);
}

std::string SedInstanceTask::algebraicUnit(size_t pIndex) const
{
    return pimpl()->algebraicUnit(pIndex);
}

} // namespace libOpenCOR

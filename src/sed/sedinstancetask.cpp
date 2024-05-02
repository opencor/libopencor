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

#include "sedinstancetask_p.h"

#include "file_p.h"
#include "sedmodel_p.h"
#include "sedtask_p.h"
#include "seduniformtimecourse_p.h"
#include "solvernla_p.h"
#include "solverode_p.h"

#include "utils.h"

#include "libopencor/sedsimulation.h"
#include "libopencor/sedsteadystate.h"

namespace libOpenCOR {

// #define PRINT_VALUES

#ifdef PRINT_VALUES
namespace {

void printHeader(const libcellml::AnalyserModelPtr &pAnalyserModel)
{
    printf("t"); // NOLINT

    for (auto &state : pAnalyserModel->states()) {
        printf(",%s", state->variable()->name().c_str()); // NOLINT
    }

    for (auto &variable : pAnalyserModel->variables()) {
        printf(",%s", variable->variable()->name().c_str()); // NOLINT
    }

    printf("\n"); // NOLINT
}

void printValues(const libcellml::AnalyserModelPtr &pAnalyserModel,
                 double pVoi, double *pStates, double *pVariables)
{
    printf("%f", pVoi); // NOLINT

    for (size_t i = 0; i < pAnalyserModel->states().size(); ++i) {
        printf(",%f", pStates[i]); // NOLINT
    }

    for (size_t i = 0; i < pAnalyserModel->variables().size(); ++i) {
        printf(",%f", pVariables[i]); // NOLINT
    }

    printf("\n"); // NOLINT
}

} // namespace
#endif

SedInstanceTaskPtr SedInstanceTask::Impl::create(const SedAbstractTaskPtr &pTask, bool pCompiled)
{
    return SedInstanceTaskPtr {new SedInstanceTask(pTask, pCompiled)};
}

SedInstanceTask::Impl::Impl(const SedAbstractTaskPtr &pTask, bool pCompiled)
    : mCompiled(pCompiled)
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT SedTask TASKS, HENCE WE ASSERT (FOR NOW) THAT pTask IS INDEED A SedTask
    //          OBJECT.

    auto task = dynamic_pointer_cast<SedTask>(pTask);

    ASSERT_NE(task, nullptr);

    // Get a runtime for the model.

    auto cellmlFile = task->pimpl()->mModel->pimpl()->mFile->pimpl()->mCellmlFile;
    auto cellmlFileType = cellmlFile->type();

    mDifferentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                         || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);
    mSimulation = task->pimpl()->mSimulation;

    auto odeSolver = mSimulation->odeSolver();
    auto nlaSolver = mSimulation->nlaSolver();

    mOdeSolver = (odeSolver != nullptr) ? dynamic_pointer_cast<SolverOde>(odeSolver->pimpl()->duplicate()) : nullptr;
    mNlaSolver = (nlaSolver != nullptr) ? dynamic_pointer_cast<SolverNla>(nlaSolver->pimpl()->duplicate()) : nullptr;
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
        mVariableDoubles.resize(mAnalyserModel->variableCount(), NAN);

        mStates = mStateDoubles.data();
        mRates = mRateDoubles.data();
        mVariables = mVariableDoubles.data();
    } else {
        mVariableDoubles.resize(mAnalyserModel->variableCount(), NAN);

        mVariables = mVariableDoubles.data();
    }

    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.

#ifdef PRINT_VALUES
    printHeader(mAnalyserModel);
#endif

    mSedUniformTimeCourse = mDifferentialModel ? dynamic_pointer_cast<SedUniformTimeCourse>(mSimulation) : nullptr;

    if (mSedUniformTimeCourse != nullptr) {
        mVoi = mSedUniformTimeCourse->pimpl()->mOutputStartTime;

#ifndef __EMSCRIPTEN__
        if (mCompiled) {
            mRuntime->initialiseCompiledVariablesForDifferentialModel()(mStates, mRates, mVariables);
            mRuntime->computeCompiledComputedConstants()(mVariables);
            mRuntime->computeCompiledRates()(mVoi, mStates, mRates, mVariables);
            mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables);
        } else {
#endif
            mRuntime->initialiseInterpretedVariablesForDifferentialModel()(mStates, mRates, mVariables);
            mRuntime->computeInterpretedComputedConstants()(mVariables);
            mRuntime->computeInterpretedRates()(mVoi, mStates, mRates, mVariables);
            mRuntime->computeInterpretedVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables);
#ifndef __EMSCRIPTEN__
        }
    } else if (mCompiled) {
        mRuntime->initialiseCompiledVariablesForAlgebraicModel()(mVariables);
        mRuntime->computeCompiledComputedConstants()(mVariables);
        mRuntime->computeCompiledVariablesForAlgebraicModel()(mVariables);
#endif
    } else {
        mRuntime->initialiseInterpretedVariablesForAlgebraicModel()(mVariables);
        mRuntime->computeInterpretedComputedConstants()(mVariables);
        mRuntime->computeInterpretedVariablesForAlgebraicModel()(mVariables);
    }

    // Make sure that the NLA solver, should it have been used, didn't report any issues.

    if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
        addIssues(mNlaSolver);

        return;
    }

    // Initialise the ODE solver, if needed.

    if (mDifferentialModel) {
        if (!mOdeSolver->pimpl()->initialise(mVoi, mAnalyserModel->stateCount(), mStates, mRates, mVariables, mRuntime->computeCompiledRates(), mRuntime->computeInterpretedRates())) {
            addIssues(mOdeSolver);

            return;
        }
    }

#ifdef PRINT_VALUES
    printValues(mAnalyserModel, mVoi, mStates, mVariables);
#endif
}

void SedInstanceTask::Impl::trackResults(size_t pIndex)
{
    mResults.voi[pIndex] = mVoi;

    for (size_t i = 0; i < mAnalyserModel->stateCount(); ++i) {
        mResults.states[i][pIndex] = mStates[i];
        mResults.rates[i][pIndex] = mRates[i];
    }

    for (size_t i = 0; i < mAnalyserModel->variableCount(); ++i) {
        mResults.variables[i][pIndex] = mVariables[i];
    }
}

void SedInstanceTask::Impl::run()
{
    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    if (mDifferentialModel) {
        // Initialise our results structure.

        auto resultsSize = mSedUniformTimeCourse->pimpl()->mNumberOfSteps + 1;

        mResults.voi.resize(resultsSize, NAN);
        mResults.states.resize(mAnalyserModel->stateCount(), Doubles(resultsSize, NAN));
        mResults.rates.resize(mAnalyserModel->stateCount(), Doubles(resultsSize, NAN));
        mResults.variables.resize(mAnalyserModel->variableCount(), Doubles(resultsSize, NAN));

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

                return;
            }

#ifndef __EMSCRIPTEN__
            if (mCompiled) {
                mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables);
            } else {
#endif
                mRuntime->computeInterpretedVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables);
#ifndef __EMSCRIPTEN__
            }
#endif

            //---GRY--- WE NEED TO CHECK FOR POSSIBLE NLA ISSUES, BUT FOR CODE COVERAGE WE NEED A MODEL THAT WOULD
            //          TRIGGER NLA ISSUES HERE, WHICH WE DON'T HAVE YET HENCE WE DISABLE THE FOLLOWING CODE WHEN DOING
            //          CODE COVERAGE.

#ifndef CODE_COVERAGE_ENABLED
            if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
                addIssues(mNlaSolver);

                return;
            }
#endif

            trackResults(++index);

#ifdef PRINT_VALUES
            printValues(mAnalyserModel, mVoi, mStates, mVariables);
#endif
        }
    }
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
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedInstanceTask::Impl *SedInstanceTask::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

} // namespace libOpenCOR

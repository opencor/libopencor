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
#include "libopencor/solvercvode.h"

#include "file_p.h"
#include "sedinstancetask_p.h"
#include "sedmodel_p.h"
#include "sedtask_p.h"
#include "seduniformtimecourse_p.h"
#include "solvernla_p.h"
#include "solverode_p.h"

#include "utils.h"

#include "libopencor/sedsimulation.h"
#include "libopencor/sedsteadystate.h"

namespace libOpenCOR {

#define PRINT_VALUES

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

    mDifferentialModel = differentialModel(cellmlFile);
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

    // Create/retrieve our various arrays.

    mAnalyserModel = cellmlFile->analyserModel();
    mInterpreter = mRuntime->interpreter();

    if (mDifferentialModel) {
        mStates = mCompiled ? new double[mAnalyserModel->stateCount()] : mInterpreter->states().data();
        mRates = mCompiled ? new double[mAnalyserModel->stateCount()] : mInterpreter->rates().data();
        mVariables = mCompiled ? new double[mAnalyserModel->variableCount()] : mInterpreter->variables().data();
    } else {
        mVariables = mCompiled ? new double[mAnalyserModel->variableCount()] : mInterpreter->variables().data();
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
            mRuntime->initialiseCompiledVariablesForDifferentialModel()(mStates, mRates, mVariables); // NOLINT
            mRuntime->computeCompiledComputedConstants()(mVariables); // NOLINT
            mRuntime->computeCompiledRates()(mVoi, mStates, mRates, mVariables); // NOLINT
            mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
        } else {
#endif
            mRuntime->initialiseInterpretedVariables();
            mRuntime->computeInterpretedComputedConstants();
            mRuntime->computeInterpretedRates(mVoi);
            mRuntime->computeInterpretedVariables(mVoi);
#ifndef __EMSCRIPTEN__
        }
    } else if (mCompiled) {
        mRuntime->initialiseCompiledVariablesForAlgebraicModel()(mVariables); // NOLINT
        mRuntime->computeCompiledComputedConstants()(mVariables); // NOLINT
        mRuntime->computeCompiledVariablesForAlgebraicModel()(mVariables); // NOLINT
#endif
    } else {
        mRuntime->initialiseInterpretedVariables();
        mRuntime->computeInterpretedComputedConstants();
        mRuntime->computeInterpretedVariables();
    }

    // Make sure that the NLA solver, should it have been used, didn't report any issues.

    if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
        addIssues(mNlaSolver);

        return;
    }

    // Initialise the ODE solver, if needed.

    if (mDifferentialModel) {
#ifndef __EMSCRIPTEN__
        if (!mOdeSolver->initialise(mVoi, mAnalyserModel->stateCount(), mStates, mRates, mVariables, mRuntime->computeCompiledRates())) {
#else
        //---GRY--- PASS A PROPER FUNTION TO COMPUTE THE RATES.
        if (!mOdeSolver->initialise(mVoi, mAnalyserModel->stateCount(), mStates, mRates, mVariables, nullptr)) {
#endif
            addIssues(mOdeSolver);

            return;
        }
    }
#ifdef PRINT_VALUES
    printValues(mAnalyserModel, mVoi, mStates, mVariables);
#endif
}

SedInstanceTask::Impl::~Impl()
{
    if (mRuntime->isCompiled()) {
        delete[] mStates;
        delete[] mRates;
        delete[] mVariables;
    }
}

void SedInstanceTask::Impl::run()
{
#ifndef __EMSCRIPTEN__
    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    if (mDifferentialModel) {
        // Compute the differential model.

        auto voiStart = mVoi;
        auto voiEnd = mSedUniformTimeCourse->pimpl()->mOutputEndTime;
        auto voiInterval = (voiEnd - mVoi) / mSedUniformTimeCourse->pimpl()->mNumberOfSteps;
        size_t voiCounter = 0;

        while (!fuzzyCompare(mVoi, voiEnd)) {
            if (!mOdeSolver->solve(mVoi, std::min(voiStart + static_cast<double>(++voiCounter) * voiInterval, voiEnd))) {
                addIssues(mOdeSolver);

                return;
            }

#    ifndef __EMSCRIPTEN__
            if (mCompiled) {
                mRuntime->computeCompiledVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
            } else {
#    endif
                mRuntime->computeInterpretedVariables(mVoi);
#    ifndef __EMSCRIPTEN__
            }
#    endif

            //---GRY--- WE NEED TO CHECK FOR POSSIBLE NLA ISSUES, BUT FOR CODE COVERAGE WE NEED A MODEL THAT WOULD ALLOW
            //          TRIGGER NLA ISSUES HERE, WHICH WE DON'T HAVE YET HENCE WE DISABLE THE FOLLOWING CODE WHEN DOING
            //          CODE COVERAGE.

#    ifndef CODE_COVERAGE_ENABLED
            if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
                addIssues(mNlaSolver);

                return;
            }
#    endif
#    ifdef PRINT_VALUES
            printValues(mAnalyserModel, mVoi, mStates, mVariables);
#    endif
        }
    }
#endif
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

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

SedInstanceTaskPtr SedInstanceTask::Impl::create(const SedAbstractTaskPtr &pTask)
{
    return SedInstanceTaskPtr {new SedInstanceTask(pTask)};
}

SedInstanceTask::Impl::Impl(const SedAbstractTaskPtr &pTask)
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT SedTask TASKS, HENCE WE ASSERT (FOR NOW) THAT pTask IS INDEED A SedTask
    //          OBJECT.

    auto task = dynamic_pointer_cast<SedTask>(pTask);

    ASSERT_NE(task, nullptr);

#ifndef __EMSCRIPTEN__
    // Get a runtime for the model.

    auto cellmlFile = task->pimpl()->mModel->pimpl()->mFile->pimpl()->mCellmlFile;

    mDifferentialModel = differentialModel(cellmlFile);
    mSimulation = task->pimpl()->mSimulation;

    auto odeSolver = mSimulation->odeSolver();
    auto nlaSolver = mSimulation->nlaSolver();

    mOdeSolver = (odeSolver != nullptr) ? dynamic_pointer_cast<SolverOde>(odeSolver->pimpl()->duplicate()) : nullptr;
    mNlaSolver = (nlaSolver != nullptr) ? dynamic_pointer_cast<SolverNla>(nlaSolver->pimpl()->duplicate()) : nullptr;
    mRuntime = cellmlFile->runtime(mNlaSolver);

#    ifndef CODE_COVERAGE_ENABLED
    if (mRuntime->hasErrors()) {
        addIssues(mRuntime);

        return;
    }
#    endif

    // Create our various arrays.

    mAnalyserModel = cellmlFile->analyserModel();

    if (mDifferentialModel) {
        mStates = new double[mAnalyserModel->stateCount()];
        mRates = new double[mAnalyserModel->stateCount()];
        mVariables = new double[mAnalyserModel->variableCount()];
    } else {
        mVariables = new double[mAnalyserModel->variableCount()];
    }

    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.
#    ifdef PRINT_VALUES
    printHeader(mAnalyserModel);
#    endif

    mSedUniformTimeCourse = mDifferentialModel ? dynamic_pointer_cast<SedUniformTimeCourse>(mSimulation) : nullptr;

    if (mDifferentialModel) {
        mVoi = mSedUniformTimeCourse->pimpl()->mOutputStartTime;

        mRuntime->initialiseVariablesForDifferentialModel()(mStates, mRates, mVariables); // NOLINT
        mRuntime->computeComputedConstants()(mVariables); // NOLINT
        mRuntime->computeRates()(mVoi, mStates, mRates, mVariables); // NOLINT
        mRuntime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
    } else {
        mRuntime->initialiseVariablesForAlgebraicModel()(mVariables); // NOLINT
        mRuntime->computeComputedConstants()(mVariables); // NOLINT
        mRuntime->computeVariablesForAlgebraicModel()(mVariables); // NOLINT
    }

    // Make sure that the NLA solver, should it have been used, didn't report any issues.

    if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
        addIssues(mNlaSolver);

        return;
    }

    // Initialise the ODE solver, if needed.

    if (mDifferentialModel) {
        if (!mOdeSolver->initialise(mVoi, mAnalyserModel->stateCount(), mStates, mRates, mVariables, mRuntime->computeRates())) {
            addIssues(mOdeSolver);

            return;
        }
    }
#    ifdef PRINT_VALUES
    printValues(mAnalyserModel, mVoi, mStates, mVariables);
#    endif
#endif
}

SedInstanceTask::Impl::~Impl()
{
    resetArrays();
}

void SedInstanceTask::Impl::resetArrays()
{
    delete[] mStates;
    delete[] mRates;
    delete[] mVariables;

    mStates = nullptr;
    mRates = nullptr;
    mVariables = nullptr;
}

void SedInstanceTask::Impl::run()
{
    // Make sure that the instance task doesn't have any issues.

    if (hasIssues()) {
        return;
    }

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

            mRuntime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT

            if ((mNlaSolver != nullptr) && mNlaSolver->hasIssues()) {
                addIssues(mNlaSolver);

                return;
            }
#    ifdef PRINT_VALUES
            printValues(mAnalyserModel, mVoi, mStates, mVariables);
#    endif
        }
    }
#endif
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
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedInstanceTask::Impl *SedInstanceTask::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

} // namespace libOpenCOR

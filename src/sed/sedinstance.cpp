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
#include "sedinstance_p.h"
#include "sedmodel_p.h"
#include "sedtask_p.h"
#include "seduniformtimecourse_p.h"

#include "utils.h"

#include "libopencor/seddocument.h"
#include "libopencor/sedsimulation.h"
#include "libopencor/sedsteadystate.h"

namespace libOpenCOR {

SedInstancePtr SedInstance::Impl::create(const SedDocumentPtr &pDocument)
{
    return SedInstancePtr {new SedInstance(pDocument)};
}

SedInstance::Impl::Impl(const SedDocumentPtr &pDocument)
    : Logger::Impl()
{
    // Check whether there are some outputs that should be generated or, failing that, whether there are some tasks that
    // could be run.
    //---GRY--- WE DON'T CURRENTLY SUPPORT OUTPUTS, SO WE JUST CHECK FOR TASKS FOR NOW.

    if (pDocument->hasTasks()) {
        // Make sure that all the tasks are valid.

        auto tasks = pDocument->tasks();
        auto tasksValid = true;

        for (const auto &task : tasks) {
            auto *taskPimpl = task->pimpl();

            taskPimpl->removeAllIssues();

            // Make sure that the task is valid.

            if (!taskPimpl->isValid()) {
                addIssues(task);

                tasksValid = false;
            }
        }

        // Create an instance of all the tasks, if they are all valid.

        if (tasksValid) {
            for (const auto &task : tasks) {
                createInstanceTask(task);
            }
        }
    } else {
        addError("The simulation experiment description does not contain any tasks to run.");
    }
}

SedInstance::Impl::~Impl()
{
    resetArrays();
}

void SedInstance::Impl::resetArrays()
{
    delete[] mStates;
    delete[] mRates;
    delete[] mVariables;

    mStates = nullptr;
    mRates = nullptr;
    mVariables = nullptr;
}

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

void SedInstance::Impl::createInstanceTask(const SedAbstractTaskPtr &pTask)
{
    //---GRY--- AT THIS STAGE, WE ONLY SUPPORT SedTask TASKS, HENCE WE ASSERT (FOR NOW) THAT pTask IS INDEED A SedTask.

    auto task = dynamic_pointer_cast<SedTask>(pTask);

    ASSERT_NE(task, nullptr);

#ifndef __EMSCRIPTEN__
    // Get a runtime for the model.

    auto *taskPimpl = task->pimpl();
    auto cellmlFile = taskPimpl->mModel->pimpl()->mFile->pimpl()->mCellmlFile;
    auto simulation = taskPimpl->mSimulation;

    mRuntime = cellmlFile->runtime(simulation->nlaSolver());

#    ifndef CODE_COVERAGE_ENABLED
    if (mRuntime->hasErrors()) {
        addIssues(mRuntime);

        return;
    }
#    endif

    // Create our various arrays.

    resetArrays();

    auto cellmlFileType = cellmlFile->type();
    auto differentialModel = (cellmlFileType == libcellml::AnalyserModel::Type::ODE)
                             || (cellmlFileType == libcellml::AnalyserModel::Type::DAE);
    auto analyserModel = cellmlFile->analyserModel();

    if (differentialModel) {
        mStates = new double[analyserModel->stateCount()];
        mRates = new double[analyserModel->stateCount()];
        mVariables = new double[analyserModel->variableCount()];
    } else {
        mVariables = new double[analyserModel->variableCount()];
    }

    // Initialise our model, which means that for an ODE/DAE model we need to initialise our states, rates, and
    // variables, compute computed constants, rates, and variables, while for an algebraic/NLA model we need to
    // initialise our variables and compute computed constants and variables.
#    ifdef PRINT_VALUES
    printHeader(analyserModel);
#    endif

    auto uniformTimeCourseSimulation = differentialModel ? dynamic_pointer_cast<SedUniformTimeCourse>(simulation) : nullptr;
    auto *uniformTimeCourseSimulationPimpl = differentialModel ? uniformTimeCourseSimulation->pimpl() : nullptr;

    if (differentialModel) {
        mVoi = uniformTimeCourseSimulationPimpl->mOutputStartTime;

        mRuntime->initialiseVariablesForDifferentialModel()(mStates, mRates, mVariables); // NOLINT
        mRuntime->computeComputedConstants()(mVariables); // NOLINT
        mRuntime->computeRates()(mVoi, mStates, mRates, mVariables); // NOLINT
        mRuntime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
    } else {
        mRuntime->initialiseVariablesForAlgebraicModel()(mVariables); // NOLINT
        mRuntime->computeComputedConstants()(mVariables); // NOLINT
        mRuntime->computeVariablesForAlgebraicModel()(mVariables); // NOLINT
    }

    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    auto nlaSolver = differentialModel ? uniformTimeCourseSimulation->nlaSolver() : dynamic_pointer_cast<SedSteadyState>(simulation)->nlaSolver();

    if (differentialModel) {
        // Initialise the ODE solver.

        auto odeSolver = uniformTimeCourseSimulation->odeSolver();

        odeSolver->initialise(mVoi, analyserModel->stateCount(), mStates, mRates, mVariables, mRuntime->computeRates());
#    ifdef PRINT_VALUES
        printValues(analyserModel, mVoi, mStates, mVariables);
#    endif

        // Compute the differential model.

        auto voiStart = mVoi;
        auto voiEnd = uniformTimeCourseSimulationPimpl->mOutputEndTime;
        auto voiInterval = (voiEnd - mVoi) / uniformTimeCourseSimulationPimpl->mNumberOfSteps;
        size_t voiCounter = 0;

        while (!fuzzyCompare(mVoi, voiEnd)) {
            if (!odeSolver->solve(mVoi, std::min(voiStart + static_cast<double>(++voiCounter) * voiInterval, voiEnd))) {
                addIssues(odeSolver);

                return;
            }

            if ((nlaSolver != nullptr) && nlaSolver->hasIssues()) {
                addIssues(nlaSolver);

                return;
            }

            mRuntime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
#    ifdef PRINT_VALUES
            printValues(analyserModel, mVoi, mStates, mVariables);
#    endif
        }
    } else if ((nlaSolver != nullptr) && nlaSolver->hasIssues()) {
        addIssues(nlaSolver);
    }
#endif
}

SedInstance::SedInstance(const SedDocumentPtr &pDocument)
    : Logger(new Impl(pDocument))
{
}

SedInstance::~SedInstance()
{
    delete pimpl();
}

SedInstance::Impl *SedInstance::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedInstance::Impl *SedInstance::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

} // namespace libOpenCOR

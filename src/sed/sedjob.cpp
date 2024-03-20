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
#include "sedjob_p.h"
#include "sedmodel_p.h"
#include "seduniformtimecourse_p.h"

#include "utils.h"

#include "libopencor/file.h"
#include "libopencor/sedmodel.h"
#include "libopencor/sedsimulation.h"
#include "libopencor/sedsteadystate.h"
#include "libopencor/seduniformtimecourse.h"

namespace libOpenCOR {

SedJobPtr SedJob::Impl::create()
{
    return SedJobPtr {new SedJob()};
}

SedJob::Impl::~Impl()
{
    resetArrays();
}

void SedJob::Impl::resetArrays()
{
    delete[] mStates;
    delete[] mRates;
    delete[] mVariables;

    mStates = nullptr;
    mRates = nullptr;
    mVariables = nullptr;
}

void SedJob::Impl::run(const SedModelPtr &pModel, const SedSimulationPtr &pSimulation)
{
    // Get a runtime for the model.

    auto cellmlFile = pModel->pimpl()->mFile->pimpl()->mCellmlFile;
    auto runtime = cellmlFile->runtime(pSimulation->nlaSolver());

#    ifndef CODE_COVERAGE_ENABLED
    if (runtime->hasErrors()) {
        addIssues(runtime);

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

    auto uniformTimeCourseSimulation = differentialModel ? dynamic_pointer_cast<SedUniformTimeCourse>(pSimulation) : nullptr;
    auto *uniformTimeCourseSimulationPimpl = differentialModel ? uniformTimeCourseSimulation->pimpl() : nullptr;

    if (differentialModel) {
        mVoi = uniformTimeCourseSimulationPimpl->mOutputStartTime;

        runtime->initialiseVariablesForDifferentialModel()(mStates, mRates, mVariables); // NOLINT
        runtime->computeComputedConstants()(mVariables); // NOLINT
        runtime->computeRates()(mVoi, mStates, mRates, mVariables); // NOLINT
        runtime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
    } else {
        runtime->initialiseVariablesForAlgebraicModel()(mVariables); // NOLINT
        runtime->computeComputedConstants()(mVariables); // NOLINT
        runtime->computeVariablesForAlgebraicModel()(mVariables); // NOLINT
    }

    // Compute our model, unless it's an algebraic/NLA model in which case we are already done.

    auto nlaSolver = differentialModel ? uniformTimeCourseSimulation->nlaSolver() : dynamic_pointer_cast<SedSteadyState>(pSimulation)->nlaSolver();

    if (differentialModel) {
        // Initialise the ODE solver.

        auto odeSolver = uniformTimeCourseSimulation->odeSolver();

        odeSolver->initialise(mVoi, analyserModel->stateCount(), mStates, mRates, mVariables, runtime->computeRates());
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

            runtime->computeVariablesForDifferentialModel()(mVoi, mStates, mRates, mVariables); // NOLINT
#    ifdef PRINT_VALUES
            printValues(analyserModel, mVoi, mStates, mVariables);
#    endif
        }
    } else if ((nlaSolver != nullptr) && nlaSolver->hasIssues()) {
        addIssues(nlaSolver);
    }
}

SedJob::SedJob()
    : Logger(new Impl())
{
}

SedJob::~SedJob()
{
    delete pimpl();
}

SedJob::Impl *SedJob::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedJob::Impl *SedJob::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

} // namespace libOpenCOR

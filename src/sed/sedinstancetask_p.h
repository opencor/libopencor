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

#pragma once

#include "logger_p.h"

#include "cellmlfileruntime.h"
#include "utils.h"

#include "libopencor/sedinstancetask.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <span>

namespace libOpenCOR {

// Combined status flags for pausing/stopping a running instance.
// Note: to use a single atomic reduces the number of atomic loads in the hot simulation loop.

enum InstanceRunControl : unsigned
{
    INSTANCE_RUN_CONTROL_NONE = 0,
    INSTANCE_RUN_CONTROL_PAUSE = 1 << 0,
    INSTANCE_RUN_CONTROL_STOP = 1 << 1,
};

struct SedInstanceTaskResults
{
    size_t resultsSize {0};

    Doubles voi;
    Doubles states;
    Doubles rates;
    Doubles constants;
    Doubles computedConstants;
    Doubles algebraicVariables;
};

using SedInstanceTaskWeakPtr = std::weak_ptr<SedInstanceTask>;

class SedInstanceTask::Impl: public Logger::Impl
{
public:
    SedInstanceTaskWeakPtr mOwner;
    CellmlFileRuntimePtr mRuntime;
    SedSimulationPtr mSimulation;
    SedUniformTimeCoursePtr mSedUniformTimeCourse;
    SedModelPtr mModel;
    bool mDifferentialModel;
    libcellml::AnalyserModelPtr mAnalyserModel;
    SolverOdePtr mOdeSolver;
    SolverNlaPtr mNlaSolver;

    size_t mStateCount {0};
    size_t mConstantCount {0};
    size_t mComputedConstantCount {0};
    size_t mAlgebraicVariableCount {0};

    double mVoi {0.0};
    double *mStates {nullptr};
    double *mRates {nullptr};
    double *mConstants {nullptr};
    double *mComputedConstants {nullptr};
    double *mAlgebraicVariables {nullptr};

    Doubles mStateDoubles;
    Doubles mRateDoubles;
    Doubles mConstantDoubles;
    Doubles mComputedConstantDoubles;
    Doubles mAlgebraicVariableDoubles;

    SedInstanceTaskResults mResults;

    std::atomic<size_t> mCompletedSteps {0};
    std::atomic<size_t> mTotalSteps {0};

    const std::atomic<unsigned> *mRunControl {nullptr};

    std::condition_variable *mPauseConditionVariable {nullptr};
    std::mutex *mPauseMutex {nullptr};

    std::string mVoiName;
    std::string mVoiUnit;
    Strings mStateNames;
    Strings mStateUnits;
    Strings mRateNames;
    Strings mRateUnits;
    Strings mConstantNames;
    Strings mConstantUnits;
    Strings mComputedConstantNames;
    Strings mComputedConstantUnits;
    Strings mAlgebraicVariableNames;
    Strings mAlgebraicVariableUnits;

    static SedInstanceTaskPtr create(const SedAbstractTaskPtr &pTask);

    explicit Impl(const SedAbstractTaskPtr &pTask);

    void trackResults(size_t pIndex);

    void applyChanges();
    void initialise();
    void run(double pVoiStart, double pVoiEnd, double pVoiInterval, bool pTrackResults);
    double run();

    double progress() const noexcept;

    std::span<const double> voi() const noexcept;
    const std::string &voiName() const noexcept;
    const std::string &voiUnit() const noexcept;

    size_t stateCount() const noexcept;
    std::span<const double> state(size_t pIndex) const noexcept;
    const std::string &stateName(size_t pIndex) const noexcept;
    const std::string &stateUnit(size_t pIndex) const noexcept;

    size_t rateCount() const noexcept;
    std::span<const double> rate(size_t pIndex) const noexcept;
    const std::string &rateName(size_t pIndex) const noexcept;
    const std::string &rateUnit(size_t pIndex) const noexcept;

    size_t constantCount() const noexcept;
    std::span<const double> constant(size_t pIndex) const noexcept;
    const std::string &constantName(size_t pIndex) const noexcept;
    const std::string &constantUnit(size_t pIndex) const noexcept;

    size_t computedConstantCount() const noexcept;
    std::span<const double> computedConstant(size_t pIndex) const noexcept;
    const std::string &computedConstantName(size_t pIndex) const noexcept;
    const std::string &computedConstantUnit(size_t pIndex) const noexcept;

    size_t algebraicVariableCount() const noexcept;
    std::span<const double> algebraicVariable(size_t pIndex) const noexcept;
    const std::string &algebraicVariableName(size_t pIndex) const noexcept;
    const std::string &algebraicVariableUnit(size_t pIndex) const noexcept;
};

} // namespace libOpenCOR

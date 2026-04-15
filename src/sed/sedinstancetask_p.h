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

namespace libOpenCOR {

struct SedInstanceTaskResults
{
    Doubles voi;
    std::vector<Doubles> states;
    std::vector<Doubles> rates;
    std::vector<Doubles> constants;
    std::vector<Doubles> computedConstants;
    std::vector<Doubles> algebraic;
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
    Doubles mAlgebraicDoubles;

    SedInstanceTaskResults mResults;

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
    double run();

    const Doubles &voi() const;
    const std::string &voiName() const;
    const std::string &voiUnit() const;

    size_t stateCount() const;
    const Doubles &state(size_t pIndex) const;
    const std::string &stateName(size_t pIndex) const;
    const std::string &stateUnit(size_t pIndex) const;

    size_t rateCount() const;
    const Doubles &rate(size_t pIndex) const;
    const std::string &rateName(size_t pIndex) const;
    const std::string &rateUnit(size_t pIndex) const;

    size_t constantCount() const;
    const Doubles &constant(size_t pIndex) const;
    const std::string &constantName(size_t pIndex) const;
    const std::string &constantUnit(size_t pIndex) const;

    size_t computedConstantCount() const;
    const Doubles &computedConstant(size_t pIndex) const;
    const std::string &computedConstantName(size_t pIndex) const;
    const std::string &computedConstantUnit(size_t pIndex) const;

    size_t algebraicVariableCount() const;
    const Doubles &algebraicVariable(size_t pIndex) const;
    const std::string &algebraicVariableName(size_t pIndex) const;
    const std::string &algebraicVariableUnit(size_t pIndex) const;
};

} // namespace libOpenCOR

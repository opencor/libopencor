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

#include "cellmlfileruntime.h"
#include "logger_p.h"

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

class SedInstanceTask::Impl: public Logger::Impl
{
public:
    CellmlFileRuntimePtr mRuntime;
    SedSimulationPtr mSimulation;
    SedUniformTimeCoursePtr mSedUniformTimeCourse;
    bool mDifferentialModel;
    libcellml::AnalyserModelPtr mAnalyserModel;
    SolverOdePtr mOdeSolver;
    SolverNlaPtr mNlaSolver;
    bool mCompiled = true;

    double mVoi = 0.0;
    double *mStates = nullptr;
    double *mRates = nullptr;
    double *mConstants = nullptr;
    double *mComputedConstants = nullptr;
    double *mAlgebraic = nullptr;

    Doubles mStateDoubles;
    Doubles mRateDoubles;
    Doubles mConstantDoubles;
    Doubles mComputedConstantDoubles;
    Doubles mAlgebraicDoubles;

    SedInstanceTaskResults mResults;

    static SedInstanceTaskPtr create(const SedAbstractTaskPtr &pTask, bool pCompiled);

    explicit Impl(const SedAbstractTaskPtr &pTask, bool pCompiled);

    void trackResults(size_t pIndex);

    void initialise();
    void run();

    Doubles voi() const;
    std::string voiName() const;
    std::string voiUnit() const;

    size_t stateCount() const;
    Doubles state(size_t pIndex) const;
    std::string stateName(size_t pIndex) const;
    std::string stateUnit(size_t pIndex) const;

    size_t rateCount() const;
    Doubles rate(size_t pIndex) const;
    std::string rateName(size_t pIndex) const;
    std::string rateUnit(size_t pIndex) const;

    size_t constantCount() const;
    Doubles constant(size_t pIndex) const;
    std::string constantName(size_t pIndex) const;
    std::string constantUnit(size_t pIndex) const;

    size_t computedConstantCount() const;
    Doubles computedConstant(size_t pIndex) const;
    std::string computedConstantName(size_t pIndex) const;
    std::string computedConstantUnit(size_t pIndex) const;

    size_t algebraicCount() const;
    Doubles algebraic(size_t pIndex) const;
    std::string algebraicName(size_t pIndex) const;
    std::string algebraicUnit(size_t pIndex) const;
};

} // namespace libOpenCOR

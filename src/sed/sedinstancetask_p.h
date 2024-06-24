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
    std::vector<Doubles> variables;
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
    double *mVariables = nullptr;

    Doubles mStateDoubles;
    Doubles mRateDoubles;
    Doubles mVariableDoubles;

    SedInstanceTaskResults mResults;

    static SedInstanceTaskPtr create(const SedAbstractTaskPtr &pTask, bool pCompiled);

    explicit Impl(const SedAbstractTaskPtr &pTask, bool pCompiled);

    void trackResults(size_t pIndex);

    void initialise();
    void run();

    Doubles state(size_t pIndex) const;
    Doubles rate(size_t pIndex) const;
    Doubles variable(size_t pIndex) const;

    Doubles voi() const;
    std::string voiName() const;
    std::string voiUnit() const;

    size_t stateCount() const;
    std::string stateName(size_t pIndex) const;
    std::string stateUnit(size_t pIndex) const;

    size_t rateCount() const;
    std::string rateName(size_t pIndex) const;
    std::string rateUnit(size_t pIndex) const;

    size_t variableCount() const;
    std::string variableName(size_t pIndex) const;
    std::string variableUnit(size_t pIndex) const;
};

} // namespace libOpenCOR

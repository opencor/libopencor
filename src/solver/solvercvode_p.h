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

#include "solverode_p.h"

#include "libopencor/solvercvode.h"

#include "sundialsbegin.h"
#include "sundials/sundials_linearsolver.h"
#include "sundials/sundials_nonlinearsolver.h"
#include "sundialsend.h"

namespace libOpenCOR {

struct SolverCvodeUserData
{
    double *constants = nullptr;
    double *computedConstants = nullptr;
    double *algebraic = nullptr;

    CellmlFileRuntime::ComputeCompiledRates computeCompiledRates = nullptr;
    CellmlFileRuntime::ComputeInterpretedRates computeInterpretedRates = nullptr;
};

class SolverCvode::Impl: public SolverOde::Impl
{
public:
    std::string mErrorMessage;

    static constexpr auto DEFAULT_MAXIMUM_STEP = 0.0;
    static constexpr auto DEFAULT_MAXIMUM_NUMBER_OF_STEPS = 500;
    static constexpr auto DEFAULT_INTEGRATION_METHOD = IntegrationMethod::BDF;
    static constexpr auto DEFAULT_ITERATION_TYPE = IterationType::NEWTON;
    static constexpr auto DEFAULT_LINEAR_SOLVER = LinearSolver::DENSE;
    static constexpr auto DEFAULT_PRECONDITIONER = Preconditioner::BANDED;
    static constexpr auto DEFAULT_UPPER_HALF_BANDWIDTH = 0;
    static constexpr auto DEFAULT_LOWER_HALF_BANDWIDTH = 0;
    static constexpr auto DEFAULT_RELATIVE_TOLERANCE = 1.0e-7;
    static constexpr auto DEFAULT_ABSOLUTE_TOLERANCE = 1.0e-7;
    static constexpr auto DEFAULT_INTERPOLATE_SOLUTION = true;

    double mMaximumStep = DEFAULT_MAXIMUM_STEP;
    int mMaximumNumberOfSteps = DEFAULT_MAXIMUM_NUMBER_OF_STEPS;
    IntegrationMethod mIntegrationMethod = DEFAULT_INTEGRATION_METHOD;
    IterationType mIterationType = DEFAULT_ITERATION_TYPE;
    LinearSolver mLinearSolver = DEFAULT_LINEAR_SOLVER;
    Preconditioner mPreconditioner = DEFAULT_PRECONDITIONER;
    int mUpperHalfBandwidth = DEFAULT_UPPER_HALF_BANDWIDTH;
    int mLowerHalfBandwidth = DEFAULT_LOWER_HALF_BANDWIDTH;
    double mRelativeTolerance = DEFAULT_RELATIVE_TOLERANCE;
    double mAbsoluteTolerance = DEFAULT_ABSOLUTE_TOLERANCE;
    bool mInterpolateSolution = DEFAULT_INTERPOLATE_SOLUTION;

    SUNContext mSunContext = nullptr;

    void *mSolver = nullptr;

    N_Vector mStatesVector = nullptr;

    SUNMatrix mSunMatrix = nullptr;
    SUNLinearSolver mSunLinearSolver = nullptr;
    SUNNonlinearSolver mSunNonLinearSolver = nullptr;

    SolverCvodeUserData mUserData;

    explicit Impl();
    ~Impl() override;

    void populate(libsedml::SedAlgorithm *pAlgorithm) override;

    SolverPtr duplicate() override;

    void resetInternals();

    StringStringMap properties() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates,
                    double *pConstants, double *pComputedConstants, double *pAlgebraic,
                    CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                    CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates) override;
    /*---GRY--- TO BE UNCOMMENTED ONCE WE ACTUALLY NEED IT.
    bool reinitialise(double pVoi) override;
    */

    double maximumStep() const;
    void setMaximumStep(double pMaximumStep);

    int maximumNumberOfSteps() const;
    void setMaximumNumberOfSteps(int pMaximumNumberOfSteps);

    IntegrationMethod integrationMethod() const;
    void setIntegrationMethod(IntegrationMethod pIntegrationMethod);

    IterationType iterationType() const;
    void setIterationType(IterationType pIterationType);

    LinearSolver linearSolver() const;
    void setLinearSolver(LinearSolver pLinearSolver);

    Preconditioner preconditioner() const;
    void setPreconditioner(Preconditioner pPreconditioner);

    int upperHalfBandwidth() const;
    void setUpperHalfBandwidth(int pUpperHalfBandwidth);

    int lowerHalfBandwidth() const;
    void setLowerHalfBandwidth(int pLowerHalfBandwidth);

    double relativeTolerance() const;
    void setRelativeTolerance(double pRelativeTolerance);

    double absoluteTolerance() const;
    void setAbsoluteTolerance(double pAbsoluteTolerance);

    bool interpolateSolution() const;
    void setInterpolateSolution(bool pInterpolateSolution);

    bool solve(double &pVoi, double pVoiEnd) override;
};

} // namespace libOpenCOR

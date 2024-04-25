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
#include "sundials/sundials_matrix.h"
#include "sundials/sundials_nonlinearsolver.h"
#include "sundialsend.h"

namespace libOpenCOR {

struct SolverCvodeUserData
{
    double *variables = nullptr;

    CellmlFileRuntime::ComputeCompiledRates computeCompiledRates = nullptr;
    CellmlFileRuntime::ComputeInterpretedRates computeInterpretedRates = nullptr;
};

class SolverCvode::Impl: public SolverOde::Impl
{
public:
    std::string mErrorMessage;

    double mMaximumStep = 0.0;
    int mMaximumNumberOfSteps = 500;
    IntegrationMethod mIntegrationMethod = IntegrationMethod::BDF;
    IterationType mIterationType = IterationType::NEWTON;
    LinearSolver mLinearSolver = LinearSolver::DENSE;
    Preconditioner mPreconditioner = Preconditioner::BANDED;
    int mUpperHalfBandwidth = 0;
    int mLowerHalfBandwidth = 0;
    double mRelativeTolerance = 1.0e-7;
    double mAbsoluteTolerance = 1.0e-7;
    bool mInterpolateSolution = true;

    SUNContext mSunContext = nullptr;

    void *mSolver = nullptr;

    N_Vector mStatesVector = nullptr;

    SUNMatrix mSunMatrix = nullptr;
    SUNLinearSolver mSunLinearSolver = nullptr;
    SUNNonlinearSolver mSunNonLinearSolver = nullptr;

    SolverCvodeUserData mUserData;

    explicit Impl();
    ~Impl() override;

    SolverPtr duplicate() override;

    void resetInternals();

    StringStringMap properties() const override;

    bool initialise(double pVoi, size_t pSize, double *pStates, double *pRates, double *pVariables,
                    CellmlFileRuntime::ComputeCompiledRates pComputeCompiledRates,
                    CellmlFileRuntime::ComputeInterpretedRates pComputeInterpretedRates) override;
    bool reinitialise(double pVoi) override;

    bool solve(double &pVoi, double pVoiEnd) override;
};

} // namespace libOpenCOR

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

#include "solvernla_p.h"

#include "libopencor/solverkinsol.h"

#include "sundialsbegin.h"
#include "sundials/sundials_linearsolver.h"
#include "sundials/sundials_matrix.h"
#include "sundialsend.h"

namespace libOpenCOR {

struct SolverKinsolUserData
{
    SolverNla::ComputeSystem computeSystem = nullptr;

    void *userData = nullptr;
};

struct SolverKinsolData
{
    SUNContext context = nullptr;

    void *solver = nullptr;

    N_Vector uVector = nullptr;
    N_Vector onesVector = nullptr;

    SUNMatrix sunMatrix = nullptr;
    SUNLinearSolver sunLinearSolver = nullptr;

    SolverKinsolUserData *userData = nullptr;
};

class SolverKinsol::Impl: public SolverNla::Impl
{
public:
    int mMaximumNumberOfIterations = 200;
    LinearSolver mLinearSolver = LinearSolver::DENSE;
    int mUpperHalfBandwidth = 0;
    int mLowerHalfBandwidth = 0;

    std::map<ComputeSystem, SolverKinsolData> mData;

    explicit Impl();
    ~Impl() override;

    StringStringMap properties() const override;

    bool solve(ComputeSystem pComputeSystem, double *pU, size_t pN, void *pUserData) override;
};

} // namespace libOpenCOR

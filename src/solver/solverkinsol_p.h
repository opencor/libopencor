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

#include "sundials/sundials_context.h"
#include "sundials/sundials_linearsolver.h"
#include "sundials/sundials_matrix.h"
#include "sundials/sundials_nvector.h"

namespace libOpenCOR {

class SolverKinsol::Impl final: public SolverNla::Impl
{
public:
    std::string mErrorMessage;

    static constexpr auto DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS {200};
    static constexpr auto DEFAULT_LINEAR_SOLVER {LinearSolver::DENSE};
    static constexpr auto DEFAULT_UPPER_HALF_BANDWIDTH {0};
    static constexpr auto DEFAULT_LOWER_HALF_BANDWIDTH {0};

    int mMaximumNumberOfIterations {DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS};
    LinearSolver mLinearSolver {DEFAULT_LINEAR_SOLVER};
    int mUpperHalfBandwidth {DEFAULT_UPPER_HALF_BANDWIDTH};
    int mLowerHalfBandwidth {DEFAULT_LOWER_HALF_BANDWIDTH};

    SUNContext mSunContext {nullptr};

    // Note: KINSOL and its associated objects below are cached and reused across solve() calls to avoid repeated
    //       creation/destruction overhead. This means a single SolverKinsol instance is NOT thread-safe for concurrent
    //       solve() calls (multiple threads sharing the same solver instance would race on these members). The intended
    //       usage model is one solver instance per simulation thread.

    void *mSolver {nullptr};

    N_Vector mU {nullptr};
    N_Vector mOnes {nullptr};

    SUNMatrix mSunMatrix {nullptr};
    SUNLinearSolver mSunLinearSolver {nullptr};

    size_t mCachedN {0};
    LinearSolver mCachedLinearSolver {DEFAULT_LINEAR_SOLVER};
    int mCachedUpperHalfBandwidth {DEFAULT_UPPER_HALF_BANDWIDTH};
    int mCachedLowerHalfBandwidth {DEFAULT_LOWER_HALF_BANDWIDTH};

    explicit Impl();
    ~Impl() override;

    void freeSolverObjects();

    void populate(libsedml::SedAlgorithm *pAlgorithm) override;

    SolverPtr duplicate() override;

    StringStringMap properties() const override;

    int maximumNumberOfIterations() const noexcept;
    void setMaximumNumberOfIterations(int pMaximumNumberOfIterations);

    LinearSolver linearSolver() const noexcept;
    void setLinearSolver(LinearSolver pLinearSolver);

    int upperHalfBandwidth() const noexcept;
    void setUpperHalfBandwidth(int pUpperHalfBandwidth);

    int lowerHalfBandwidth() const noexcept;
    void setLowerHalfBandwidth(int pLowerHalfBandwidth);

    bool solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData) override;
};

} // namespace libOpenCOR

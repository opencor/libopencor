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

#include <map>

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

    // A KINSOL solver and its associated objects, as well as the linear solver settings with which they were created
    // and whether their linear solver has been set up (i.e. whether there is a Jacobian that can be reused).

    struct KinsolObjects
    {
        void *solver {nullptr};

        N_Vector u {nullptr};
        N_Vector ones {nullptr};

        SUNMatrix sunMatrix {nullptr};
        SUNLinearSolver sunLinearSolver {nullptr};

        LinearSolver linearSolver {DEFAULT_LINEAR_SOLVER};
        int upperHalfBandwidth {DEFAULT_UPPER_HALF_BANDWIDTH};
        int lowerHalfBandwidth {DEFAULT_LOWER_HALF_BANDWIDTH};

        bool linearSolverSetUp {false};
    };

    // Note: we keep some KINSOL objects for each NLA system that we solve, i.e. for each objective function and size,
    //       and reuse them across solve() calls since creating them is expensive and a model may have several NLA
    //       systems, which are typically solved in turn (e.g., each time the rates of a DAE model are computed). This
    //       means that a SolverKinsol instance is NOT thread-safe for concurrent solve() calls (multiple threads
    //       sharing the same solver instance would race on these objects). The intended usage model is one solver
    //       instance per simulation thread.

    std::map<std::pair<uintptr_t, size_t>, KinsolObjects> mKinsolObjects;

    explicit Impl();
    ~Impl() override;

    static void freeKinsolObjects(KinsolObjects &pKinsolObjects);

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

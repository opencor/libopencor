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

namespace libOpenCOR {

class SolverKinsol::Impl: public SolverNla::Impl
{
public:
    std::string mErrorMessage;

    static constexpr auto DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS = 200;
    static constexpr auto DEFAULT_LINEAR_SOLVER = LinearSolver::DENSE;
    static constexpr auto DEFAULT_UPPER_HALF_BANDWIDTH = 0;
    static constexpr auto DEFAULT_LOWER_HALF_BANDWIDTH = 0;

    int mMaximumNumberOfIterations = DEFAULT_MAXIMUM_NUMBER_OF_ITERATIONS;
    LinearSolver mLinearSolver = DEFAULT_LINEAR_SOLVER;
    int mUpperHalfBandwidth = DEFAULT_UPPER_HALF_BANDWIDTH;
    int mLowerHalfBandwidth = DEFAULT_LOWER_HALF_BANDWIDTH;

    explicit Impl();

    void populate(libsedml::SedAlgorithm *pAlgorithm) override;

    SolverPtr duplicate() override;

    StringStringMap properties() const override;

    int maximumNumberOfIterations() const;
    void setMaximumNumberOfIterations(int pMaximumNumberOfIterations);

    LinearSolver linearSolver() const;
    void setLinearSolver(LinearSolver pLinearSolver);

    int upperHalfBandwidth() const;
    void setUpperHalfBandwidth(int pUpperHalfBandwidth);

    int lowerHalfBandwidth() const;
    void setLowerHalfBandwidth(int pLowerHalfBandwidth);

#ifdef __EMSCRIPTEN__
    bool solve(size_t pComputeObjectiveFunctionIndex, double *pU, size_t pN, void *pUserData) override;
#else
    bool solve(ComputeObjectiveFunction pComputeObjectiveFunction, double *pU, size_t pN, void *pUserData) override;
#endif
};

} // namespace libOpenCOR

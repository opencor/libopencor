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

#include "libopencor/solverode.h"

#include "tests/utils.h"

namespace OdeModel {

#include "../../res/api/solver/ode/model.h"

std::tuple<std::shared_ptr<libOpenCOR::SolverOde>, double *, double *, double *> initialise(const std::string &pSolverName);
void compute(const std::shared_ptr<libOpenCOR::SolverOde> &pSolver,
             double *pStates, double *pRates, double *pVariables,
             const libOpenCOR::Doubles &pFinalStates, const libOpenCOR::Doubles &pAbsoluteErrors);
void finalise(double *pStates, double *pRates, double *pVariables);

} // namespace OdeModel

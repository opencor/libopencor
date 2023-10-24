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

#include "solvernla.h"

#include "tests/utils.h"

namespace NlaModel {

namespace Model1 {

#include "../../res/api/solver/nla/model1.h"

std::tuple<std::shared_ptr<libOpenCOR::SolverNla>, double *> initialise(const std::string &pSolverName);
void compute(double *pVariables, const libOpenCOR::Doubles &pSolutions, const libOpenCOR::Doubles &pAbsoluteErrors);
void finalise(double *pVariables);

} // namespace Model1

namespace Model2 {

#include "../../res/api/solver/nla/model2.h"

std::tuple<std::shared_ptr<libOpenCOR::SolverNla>, double *> initialise(const std::string &pSolverName);
void compute(double *pVariables, const libOpenCOR::Doubles &pSolutions, const libOpenCOR::Doubles &pAbsoluteErrors);
void finalise(double *pVariables);

} // namespace Model2

} // namespace NlaModel

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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace libOpenCOR {

// Shared pointers.

class File;
using FilePtr = std::shared_ptr<File>; /**< Type definition for the shared @ref File pointer. */

class Issue;
using IssuePtr = std::shared_ptr<Issue>; /**< Type definition for the shared @ref Issue pointer. */

class Logger;
using LoggerPtr = std::shared_ptr<Logger>; /**< Type definition for the shared @ref Logger pointer. */

class SolverCvode;
using SolverCvodePtr = std::shared_ptr<SolverCvode>; /**< Type definition for the shared @ref SolverCvode pointer. */

class SolverForwardEuler;
using SolverForwardEulerPtr = std::shared_ptr<SolverForwardEuler>; /**< Type definition for the shared @ref SolverForwardEuler pointer. */

class SolverFourthOrderRungeKutta;
using SolverFourthOrderRungeKuttaPtr = std::shared_ptr<SolverFourthOrderRungeKutta>; /**< Type definition for the shared @ref SolverFourthOrderRungeKutta pointer. */

class SolverHeun;
using SolverHeunPtr = std::shared_ptr<SolverHeun>; /**< Type definition for the shared @ref SolverHeun pointer. */

class SolverKinsol;
using SolverKinsolPtr = std::shared_ptr<SolverKinsol>; /**< Type definition for the shared @ref SolverKinsol pointer. */

class SolverSecondOrderRungeKutta;
using SolverSecondOrderRungeKuttaPtr = std::shared_ptr<SolverSecondOrderRungeKutta>; /**< Type definition for the shared @ref SolverSecondOrderRungeKutta pointer. */

// Vectors.

using IssuePtrVector = std::vector<IssuePtr>; /**< Type definition for a vector of @ref Issue pointers. */
using UnsignedCharVector = std::vector<unsigned char>; /**< Type definition for a vector of unsigned characters. */

} // namespace libcellml

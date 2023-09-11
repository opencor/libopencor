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

#include <memory>
#include <string>
#include <variant>

namespace libOpenCOR {

class File;
using FilePtr = std::shared_ptr<File>; /**< Type definition for the shared @ref File pointer. */

class Issue;
using IssuePtr = std::shared_ptr<Issue>; /**< Type definition for the shared @ref Issue pointer. */

class Logger;
using LoggerPtr = std::shared_ptr<Logger>; /**< Type definition for the shared @ref Logger pointer. */

class Solver;
using SolverPtr = std::shared_ptr<Solver>; /**< Type definition for the shared @ref Solver pointer. */

class SolverForwardEuler;
using SolverForwardEulerPtr = std::shared_ptr<SolverForwardEuler>; /**< Type definition for the shared @ref SolverForwardEuler pointer. */

class SolverInfo;
using SolverInfoPtr = std::shared_ptr<SolverInfo>; /**< Type definition for the shared @ref SolverInfo pointer. */

class SolverProperty;
using SolverPropertyPtr = std::shared_ptr<SolverProperty>; /**< Type definition for the shared @ref SolverProperty pointer. */

using SolverPropertyValue = std::variant<bool, int, double, std::string>; /**< Type definition for a @ref SolverProperty value. */

class SolverUnknown;
using SolverUnknownPtr = std::shared_ptr<SolverUnknown>; /**< Type definition for the shared @ref SolverUnknown pointer. */

} // namespace libcellml

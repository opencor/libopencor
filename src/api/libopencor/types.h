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

namespace libOpenCOR {

class File;
using FilePtr = std::shared_ptr<File>; /**< Type definition for the shared @ref File pointer. */

class Issue;
using IssuePtr = std::shared_ptr<Issue>; /**< Type definition for the shared @ref Issue pointer. */

class Logger;
using LoggerPtr = std::shared_ptr<Logger>; /**< Type definition for the shared @ref Logger pointer. */

class SedAbstractTask;
using SedAbstractTaskPtr = std::shared_ptr<SedAbstractTask>; /**< Type definition for the shared @ref SedAbstractTask pointer. */

class SedAlgorithmParameter;
using SedAlgorithmParameterPtr = std::shared_ptr<SedAlgorithmParameter>; /**< Type definition for the shared @ref SedAlgorithmParameter pointer. */

class SedDataDescription;
using SedDataDescriptionPtr = std::shared_ptr<SedDataDescription>; /**< Type definition for the shared @ref SedDataDescription pointer. */

class SedDataGenerator;
using SedDataGeneratorPtr = std::shared_ptr<SedDataGenerator>; /**< Type definition for the shared @ref SedDataGenerator pointer. */

class SedDocument;
using SedDocumentPtr = std::shared_ptr<SedDocument>; /**< Type definition for the shared @ref SedDocument pointer. */

class SedModel;
using SedModelPtr = std::shared_ptr<SedModel>; /**< Type definition for the shared @ref SedModel pointer. */

class SedOutput;
using SedOutputPtr = std::shared_ptr<SedOutput>; /**< Type definition for the shared @ref SedOutput pointer. */

class SedSimulation;
using SedSimulationPtr = std::shared_ptr<SedSimulation>; /**< Type definition for the shared @ref SedSimulation pointer. */

class SedStyle;
using SedStylePtr = std::shared_ptr<SedStyle>; /**< Type definition for the shared @ref SedStyle pointer. */

class Solver;
using SolverPtr = std::shared_ptr<Solver>; /**< Type definition for the shared @ref Solver pointer. */

class SolverInfo;
using SolverInfoPtr = std::shared_ptr<SolverInfo>; /**< Type definition for the shared @ref SolverInfo pointer. */

class SolverProperty;
using SolverPropertyPtr = std::shared_ptr<SolverProperty>; /**< Type definition for the shared @ref SolverProperty pointer. */

} // namespace libcellml

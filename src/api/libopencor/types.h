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

class SedAbstractTask;
using SedAbstractTaskPtr = std::shared_ptr<SedAbstractTask>; /**< Type definition for the shared @ref SedAbstractTask pointer. */

class SedBase;
using SedBasePtr = std::shared_ptr<SedBase>; /**< Type definition for the shared @ref SedBase pointer. */

class SedDataDescription;
using SedDataDescriptionPtr = std::shared_ptr<SedDataDescription>; /**< Type definition for the shared @ref SedDataDescription pointer. */

class SedDataGenerator;
using SedDataGeneratorPtr = std::shared_ptr<SedDataGenerator>; /**< Type definition for the shared @ref SedDataGenerator pointer. */

class SedDocument;
using SedDocumentPtr = std::shared_ptr<SedDocument>; /**< Type definition for the shared @ref SedDocument pointer. */

class SedInstance;
using SedInstancePtr = std::shared_ptr<SedInstance>; /**< Type definition for the shared @ref SedInstance pointer. */

class SedInstanceTask;
using SedInstanceTaskPtr = std::shared_ptr<SedInstanceTask>; /**< Type definition for the shared @ref SedInstanceTask pointer. */

class SedModel;
using SedModelPtr = std::shared_ptr<SedModel>; /**< Type definition for the shared @ref SedModel pointer. */

class SedOneStep;
using SedOneStepPtr = std::shared_ptr<SedOneStep>; /**< Type definition for the shared @ref SedOneStep pointer. */

class SedOutput;
using SedOutputPtr = std::shared_ptr<SedOutput>; /**< Type definition for the shared @ref SedOutput pointer. */

class SedRepeatedTask;
using SedRepeatedTaskPtr = std::shared_ptr<SedRepeatedTask>; /**< Type definition for the shared @ref SedRepeatedTask pointer. */

class SedSimulation;
using SedSimulationPtr = std::shared_ptr<SedSimulation>; /**< Type definition for the shared @ref SedSimulation pointer. */

class SedSteadyState;
using SedSteadyStatePtr = std::shared_ptr<SedSteadyState>; /**< Type definition for the shared @ref SedSteadyState pointer. */

class SedStyle;
using SedStylePtr = std::shared_ptr<SedStyle>; /**< Type definition for the shared @ref SedStyle pointer. */

class SedTask;
using SedTaskPtr = std::shared_ptr<SedTask>; /**< Type definition for the shared @ref SedTask pointer. */

class SedUniformTimeCourse;
using SedUniformTimeCoursePtr = std::shared_ptr<SedUniformTimeCourse>; /**< Type definition for the shared @ref SedUniformTimeCourse pointer. */

class Solver;
using SolverPtr = std::shared_ptr<Solver>; /**< Type definition for the shared @ref Solver pointer. */

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

class SolverNla;
using SolverNlaPtr = std::shared_ptr<SolverNla>; /**< Type definition for the shared @ref SolverNla pointer. */

class SolverOde;
using SolverOdePtr = std::shared_ptr<SolverOde>; /**< Type definition for the shared @ref SolverOde pointer. */

class SolverOdeFixedStep;
using SolverOdeFixedStepPtr = std::shared_ptr<SolverOdeFixedStep>; /**< Type definition for the shared @ref SolverOdeFixedStep pointer. */

class SolverSecondOrderRungeKutta;
using SolverSecondOrderRungeKuttaPtr = std::shared_ptr<SolverSecondOrderRungeKutta>; /**< Type definition for the shared @ref SolverSecondOrderRungeKutta pointer. */

// Vectors.

using Doubles = std::vector<double>; /**< Type definition for a vector of doubles. */
using IssuePtrs = std::vector<IssuePtr>; /**< Type definition for a vector of @ref Issue pointers. */
using SedAbstractTaskPtrs = std::vector<SedAbstractTaskPtr>; /**< Type definition for a vector of @ref SedAbstractTask pointers. */
using SedDataDescriptionPtrs = std::vector<SedDataDescriptionPtr>; /**< Type definition for a vector of @ref SedDataDescription pointers. */
using SedDataGeneratorPtrs = std::vector<SedDataGeneratorPtr>; /**< Type definition for a vector of @ref SedDataGenerator pointers. */
using SedInstanceTaskPtrs = std::vector<SedInstanceTaskPtr>; /**< Type definition for a vector of @ref SedInstanceTask pointers. */
using SedModelPtrs = std::vector<SedModelPtr>; /**< Type definition for a vector of @ref SedModel pointers. */
using SedOutputPtrs = std::vector<SedOutputPtr>; /**< Type definition for a vector of @ref SedOutput pointers. */
using SedSimulationPtrs = std::vector<SedSimulationPtr>; /**< Type definition for a vector of @ref SedSimulation pointers. */
using SedStylePtrs = std::vector<SedStylePtr>; /**< Type definition for a vector of @ref SedStyle pointers. */
using UnsignedChars = std::vector<unsigned char>; /**< Type definition for a vector of unsigned characters. */

} // namespace libOpenCOR

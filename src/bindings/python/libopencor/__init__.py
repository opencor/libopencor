# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from .module import __doc__, __version__
from .module import (
    # File API.
    File,
    FileManager,
    # Issue API.
    Issue,
    # Logger API.
    Logger,
    # Sed API.
    SedAbstractTask,
    SedAnalysis,
    SedBase,
    SedChange,
    SedChangeAttribute,
    SedDataDescription,
    SedDataGenerator,
    SedDocument,
    SedInstance,
    SedInstanceTask,
    SedModel,
    SedOneStep,
    SedOutput,
    # ---GRY---
    # SedRepeatedTask,
    SedSimulation,
    SedSteadyState,
    SedStyle,
    SedTask,
    SedUniformTimeCourse,
    # Solver API.
    Solver,
    SolverCvode,
    SolverForwardEuler,
    SolverFourthOrderRungeKutta,
    SolverHeun,
    SolverKinsol,
    SolverSecondOrderRungeKutta,
    # Version API.
    version,
    version_string,
    clang_version,
    clang_version_string,
    libcellml_version,
    libcellml_version_string,
    libcombine_version,
    libcombine_version_string,
    libcurl_version,
    libcurl_version_string,
    libsedml_version,
    libsedml_version_string,
    llvm_version,
    llvm_version_string,
    sundials_version,
    sundials_version_string,
)

__all__ = (
    # File API.
    "File",
    "FileManager",
    # Issue API.
    "Issue",
    # Logger API.
    "Logger",
    # Sed API.
    "SedAbstractTask",
    "SedAnalysis",
    "SedBase",
    "SedChange",
    "SedChangeAttribute",
    "SedDataDescription",
    "SedDataGenerator",
    "SedDocument",
    "SedInstance",
    "SedInstanceTask",
    "SedModel",
    "SedOneStep",
    "SedOutput",
    # ---GRY---
    # "SedRepeatedTask",
    "SedSimulation",
    "SedSteadyState",
    "SedStyle",
    "SedTask",
    "SedUniformTimeCourse",
    # Solver API.
    "Solver",
    "SolverCvode",
    "SolverForwardEuler",
    "SolverFourthOrderRungeKutta",
    "SolverHeun",
    "SolverKinsol",
    "SolverSecondOrderRungeKutta",
    # Version API.
    "version",
    "version_string",
    "clang_version",
    "clang_version_string",
    "libcellml_version",
    "libcellml_version_string",
    "libcombine_version",
    "libcombine_version_string",
    "libcurl_version",
    "libcurl_version_string",
    "libsedml_version",
    "libsedml_version_string",
    "llvm_version",
    "llvm_version_string",
    "sundials_version",
    "sundials_version_string",
)

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

#include "tests/utils.h"

#include <libopencor>

namespace {

std::string cvodeExpectedSerialisation(const std::string &pSource, const std::map<std::string, std::string> &pParameters = {})
{
    auto integrationMethod = pParameters.find("KISAO:0000475");
    auto iterationType = pParameters.find("KISAO:0000476");
    auto linearSolver = pParameters.find("KISAO:0000477");
    auto preconditioner = pParameters.find("KISAO:0000478");
    auto interpolateSolution = pParameters.find("KISAO:0000481");

    return std::string(R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source=")")
        .append(pSource)
        .append(R"("/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value=")")
        .append((integrationMethod != pParameters.end()) ? integrationMethod->second : "BDF")
        .append(R"("/>
          <algorithmParameter kisaoID="KISAO:0000476" value=")")
        .append((iterationType != pParameters.end()) ? iterationType->second : "Newton")
        .append(R"("/>
          <algorithmParameter kisaoID="KISAO:0000477" value=")")
        .append((linearSolver != pParameters.end()) ? linearSolver->second : "Dense")
        .append(R"("/>
          <algorithmParameter kisaoID="KISAO:0000478" value=")")
        .append((preconditioner != pParameters.end()) ? preconditioner->second : "Banded")
        .append(R"("/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value=")")
        .append((interpolateSolution != pParameters.end()) ? interpolateSolution->second : "true")
        .append(R"("/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)");
}

std::string kinsolExpectedSerialisation(const std::map<std::string, std::string> &pParameters = {})
{
    auto linearSolver = pParameters.find("KISAO:0000477");

    return std::string(R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="api/sed/nla.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value=")")
        .append((linearSolver != pParameters.end()) ? linearSolver->second : "Dense")
        .append(R"("/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)");
}

} // namespace

TEST(SerialiseSedTest, localCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedTest, localCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SOME_CELLML_CONTENTS));

    auto document = libOpenCOR::SedDocument::create(file);

#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(document->serialise(), cvodeExpectedSerialisation("file:///P:/some/path/file.txt"));
#else
    EXPECT_EQ(document->serialise(), cvodeExpectedSerialisation("file:///some/path/file.txt"));
#endif
}

TEST(SerialiseSedTest, relativeLocalCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(std::string(libOpenCOR::RESOURCE_LOCATION).append("/../..")), cvodeExpectedSerialisation("tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedTest, relativeLocalCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE);

    file->setContents(libOpenCOR::charArrayToUnsignedChars(libOpenCOR::SOME_CELLML_CONTENTS));

    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedTest, remoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::REMOTE_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedTest, remoteCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(), cvodeExpectedSerialisation("https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedTest, relativeRemoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(std::string(libOpenCOR::REMOTE_BASE_PATH).append("/../..")), cvodeExpectedSerialisation("tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedTest, daeModel)
{
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="api/sed/dae.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
      <nlaAlgorithm xmlns="https://opencor.ws/libopencor" kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </nlaAlgorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

TEST(SerialiseSedTest, nlaModel)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), kinsolExpectedSerialisation());
}

TEST(SerialiseSedTest, algebraicModel)
{
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="api/sed/algebraic.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <steadyState id="simulation1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

TEST(SerialiseSedTest, fixedStepOdeSolver)
{
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="1000" numberOfSteps="1000">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];

    simulation->setOdeSolver(libOpenCOR::SolverForwardEuler::create());

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

TEST(SerialiseSedTest, cvodeSolverWithAdamsMoultonInterationMethod)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000475", "Adams-Moulton"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithFunctionalIterationType)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000476", "Functional"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Banded"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithDiagonalLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Diagonal"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "GMRES"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "BiCGStab"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithTfqmrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "TFQMR"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000478", "No"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithNoInterpolateSolution)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000481", "false"}}));
}

TEST(SerialiseSedTest, kinsolWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), kinsolExpectedSerialisation({{"KISAO:0000477", "Banded"}}));
}

TEST(SerialiseSedTest, kinsolWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), kinsolExpectedSerialisation({{"KISAO:0000477", "GMRES"}}));
}

TEST(SerialiseSedTest, kinsolWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), kinsolExpectedSerialisation({{"KISAO:0000477", "BiCGStab"}}));
}

TEST(SerialiseSedTest, kinsolWithTfmqrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = std::dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), kinsolExpectedSerialisation({{"KISAO:0000477", "TFQMR"}}));
}

TEST(SerialiseSedTest, oneStepSimulation)
{
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model1" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <oneStep id="simulation1" step="1"/>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model1" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    document->removeSimulation(document->simulations()[0]);

    auto simulation = libOpenCOR::SedOneStep::create(document);

    document->addSimulation(simulation);

    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

TEST(SerialiseSedTest, sedmlFile)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::WARNING, "The model 'cellml_2.cellml' could not be found in the file manager. It has been automatically added to it."},
    };
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfModels>
    <model id="model" language="urn:sedml:language:cellml" source="cellml_2.cellml"/>
  </listOfModels>
  <listOfSimulations>
    <uniformTimeCourse id="simulation1" initialTime="0" outputStartTime="0" outputEndTime="50" numberOfSteps="50000">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </uniformTimeCourse>
  </listOfSimulations>
  <listOfTasks>
    <task id="task1" modelReference="model" simulationReference="simulation1"/>
  </listOfTasks>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

TEST(SerialiseSedTest, sedSimulation)
{
    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES = {
        {libOpenCOR::Issue::Type::WARNING, "The solver 'KISAO:1234567' is not recognised. The CVODE solver will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."},
        {libOpenCOR::Issue::Type::WARNING, "The step ('KISAO:0000483') cannot be equal to '-1.23'. It must be greater or equal to 0. A step of 1 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The step ('KISAO:0000483') cannot be equal to 'nan'. It must be greater or equal to 0. A step of 1 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The step ('KISAO:0000483') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A step of 1 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."},
        {libOpenCOR::Issue::Type::WARNING, "The relative tolerance ('KISAO:0000209') cannot be equal to '-1e-03'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The absolute tolerance ('KISAO:0000211') cannot be equal to '-1e-05'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of steps ('KISAO:0000415') cannot be equal to '-369'. It must be greater than 0. A maximum number of steps of 500 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum step ('KISAO:0000467') cannot be equal to '-0.01'. It must be greater or equal to 0. A maximum step of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The relative tolerance ('KISAO:0000209') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The absolute tolerance ('KISAO:0000211') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of steps ('KISAO:0000415') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of steps of 500 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum step ('KISAO:0000467') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A maximum step of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The integration method ('KISAO:0000475') cannot be equal to 'Unknown'. It must be equal to 'BDF' or 'Adams-Moulton'. A BDF integration method will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The iteration type ('KISAO:0000476') cannot be equal to 'Unknown'. It must be equal to 'Functional' or 'Newton'. A Newton iteration type will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The preconditioner ('KISAO:0000478') cannot be equal to 'Unknown'. It must be equal to 'No' or 'Banded'. A Banded preconditioner will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The interpolate solution parameter ('KISAO:0000481') cannot be equal to 'True'. It must be equal to 'true' or 'false'. A value of true will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The relative tolerance ('KISAO:0000209') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A relative tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The absolute tolerance ('KISAO:0000211') cannot be equal to '1.23e456789'. It must be greater or equal to 0. An absolute tolerance of 1e-07 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of steps ('KISAO:0000415') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of steps of 500 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum step ('KISAO:0000467') cannot be equal to '1.23e456789'. It must be greater or equal to 0. A maximum step of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'Diagonal', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The parameter 'KISAO:1234567' is not recognised. It will be ignored."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of iterations ('KISAO:0000486') cannot be equal to '-123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '-1'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '-2'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of iterations ('KISAO:0000486') cannot be equal to 'NotANumber'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to 'NotANumber'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to 'NotANumber'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The maximum number of iterations ('KISAO:0000486') cannot be equal to '1234567890123'. It must be greater than 0. A maximum number of iterations of 200 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The upper half-bandwidth ('KISAO:0000479') cannot be equal to '1234567890123'. It must be greater or equal to 0. An upper half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The lower half-bandwidth ('KISAO:0000480') cannot be equal to '1234567890123'. It must be greater or equal to 0. A lower half-bandwidth of 0 will be used instead."},
        {libOpenCOR::Issue::Type::WARNING, "The linear solver ('KISAO:0000477') cannot be equal to 'Unknown'. It must be equal to 'Dense', 'Banded', 'GMRES', 'BiCGStab', or 'TFQMR'. A Dense linear solver will be used instead."},
    };
    static const std::string expectedSerialisation = R"(<?xml version="1.0" encoding="UTF-8"?>
<sedML xmlns="http://sed-ml.org/sed-ml/level1/version4" level="1" version="4">
  <listOfSimulations>
    <oneStep id="oneStep1" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep2" step="3.69">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1.23"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep3" step="3.69">
      <algorithm kisaoID="KISAO:0000032">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep4" step="3.69">
      <algorithm kisaoID="KISAO:0000301">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep5" step="3.69">
      <algorithm kisaoID="KISAO:0000381">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep6" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="0.001"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-05"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="369"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0.01"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="1"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="2"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="false"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep7" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="Adams-Moulton"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Functional"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="No"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="false"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep8" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Diagonal"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep9" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="GMRES"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep10" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="BiCGStab"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep11" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="TFQMR"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <oneStep id="oneStep12" step="3.69">
      <algorithm kisaoID="KISAO:0000019">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000209" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000211" value="1e-07"/>
          <algorithmParameter kisaoID="KISAO:0000415" value="500"/>
          <algorithmParameter kisaoID="KISAO:0000467" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000475" value="BDF"/>
          <algorithmParameter kisaoID="KISAO:0000476" value="Newton"/>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000478" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000481" value="true"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </oneStep>
    <steadyState id="steadyState1">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="1"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="2"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="123"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState2">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Banded"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState3">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="GMRES"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState4">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="BiCGStab"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState5">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="TFQMR"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <steadyState id="steadyState6">
      <algorithm kisaoID="KISAO:0000282">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000477" value="Dense"/>
          <algorithmParameter kisaoID="KISAO:0000479" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000480" value="0"/>
          <algorithmParameter kisaoID="KISAO:0000486" value="200"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </steadyState>
    <analysis id="analysis">
      <algorithm kisaoID="KISAO:0000030">
        <listOfAlgorithmParameters>
          <algorithmParameter kisaoID="KISAO:0000483" value="1.23"/>
        </listOfAlgorithmParameters>
      </algorithm>
    </analysis>
  </listOfSimulations>
</sedML>
)";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/simulations.sedml"));
    auto document = libOpenCOR::SedDocument::create(file);

    // EXPECT_EQ_ISSUES(document, EXPECTED_ISSUES);
    EXPECT_EQ(document->serialise(libOpenCOR::RESOURCE_LOCATION), expectedSerialisation);
}

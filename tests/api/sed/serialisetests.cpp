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

#include "gtest/gtest.h"

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

    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
           "  <listOfModels>\n"
           "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\""
           + pSource + "\"/>\n"
                       "  </listOfModels>\n"
                       "  <listOfSimulations>\n"
                       "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\">\n"
                       "      <algorithm kisaoID=\"KISAO:0000019\">\n"
                       "        <listOfAlgorithmParameters>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000209\" value=\"1e-07\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000211\" value=\"1e-07\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000415\" value=\"500\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000467\" value=\"0\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000475\" value=\""
           + ((integrationMethod != pParameters.end()) ?
                  integrationMethod->second :
                  "BDF")
           + "\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000476\" value=\""
           + ((iterationType != pParameters.end()) ?
                  iterationType->second :
                  "Newton")
           + "\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000477\" value=\""
           + ((linearSolver != pParameters.end()) ?
                  linearSolver->second :
                  "Dense")
           + "\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000478\" value=\""
           + ((preconditioner != pParameters.end()) ?
                  preconditioner->second :
                  "Banded")
           + "\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000479\" value=\"0\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000480\" value=\"0\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000481\" value=\""
           + ((interpolateSolution != pParameters.end()) ?
                  interpolateSolution->second :
                  "true")
           + "\"/>\n"
             "        </listOfAlgorithmParameters>\n"
             "      </algorithm>\n"
             "    </uniformTimeCourse>\n"
             "  </listOfSimulations>\n"
             "  <listOfTasks>\n"
             "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
             "  </listOfTasks>\n"
             "</sedML>\n";
}

std::string kinsolExpectedSerialisation(const std::map<std::string, std::string> &pParameters = {})
{
    auto linearSolver = pParameters.find("KISAO:0000477");

    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
           "  <listOfModels>\n"
           "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"api/sed/nla.cellml\"/>\n"
           "  </listOfModels>\n"
           "  <listOfSimulations>\n"
           "    <steadyState id=\"simulation1\">\n"
           "      <algorithm kisaoID=\"KISAO:0000282\">\n"
           "        <listOfAlgorithmParameters>\n"
           "          <algorithmParameter kisaoID=\"KISAO:0000477\" value=\""
           + ((linearSolver != pParameters.end()) ?
                  linearSolver->second :
                  "Dense")
           + "\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000479\" value=\"0\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000480\" value=\"0\"/>\n"
             "          <algorithmParameter kisaoID=\"KISAO:0000486\" value=\"200\"/>\n"
             "        </listOfAlgorithmParameters>\n"
             "      </algorithm>\n"
             "    </steadyState>\n"
             "  </listOfSimulations>\n"
             "  <listOfTasks>\n"
             "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
             "  </listOfTasks>\n"
             "</sedML>\n";
}

} // namespace

TEST(SerialiseSedTest, localCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml"));
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

    EXPECT_EQ(document->serialise(std::string(libOpenCOR::LOCAL_BASE_PATH).append("/../..")), cvodeExpectedSerialisation("tests/res/cellml_2.cellml"));
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
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"api/sed/dae.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\">\n"
                                                     "      <algorithm kisaoID=\"KISAO:0000019\">\n"
                                                     "        <listOfAlgorithmParameters>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000209\" value=\"1e-07\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000211\" value=\"1e-07\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000415\" value=\"500\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000467\" value=\"0\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000475\" value=\"BDF\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000476\" value=\"Newton\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000477\" value=\"Dense\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000478\" value=\"Banded\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000479\" value=\"0\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000480\" value=\"0\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000481\" value=\"true\"/>\n"
                                                     "        </listOfAlgorithmParameters>\n"
                                                     "      </algorithm>\n"
                                                     "      <nlaAlgorithm xmlns=\"https://opencor.ws/libopencor\" kisaoID=\"KISAO:0000282\">\n"
                                                     "        <listOfAlgorithmParameters>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000477\" value=\"Dense\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000479\" value=\"0\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000480\" value=\"0\"/>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000486\" value=\"200\"/>\n"
                                                     "        </listOfAlgorithmParameters>\n"
                                                     "      </nlaAlgorithm>\n"
                                                     "    </uniformTimeCourse>\n"
                                                     "  </listOfSimulations>\n"
                                                     "  <listOfTasks>\n"
                                                     "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
                                                     "  </listOfTasks>\n"
                                                     "</sedML>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), expectedSerialisation);
}

TEST(SerialiseSedTest, nlaModel)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), kinsolExpectedSerialisation());
}

TEST(SerialiseSedTest, algebraicModel)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"api/sed/algebraic.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <steadyState id=\"simulation1\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "  <listOfTasks>\n"
                                                     "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
                                                     "  </listOfTasks>\n"
                                                     "</sedML>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), expectedSerialisation);
}

TEST(SerialiseSedTest, fixedStepOdeSolver)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\">\n"
                                                     "      <algorithm kisaoID=\"KISAO:0000030\">\n"
                                                     "        <listOfAlgorithmParameters>\n"
                                                     "          <algorithmParameter kisaoID=\"KISAO:0000483\" value=\"1\"/>\n"
                                                     "        </listOfAlgorithmParameters>\n"
                                                     "      </algorithm>\n"
                                                     "    </uniformTimeCourse>\n"
                                                     "  </listOfSimulations>\n"
                                                     "  <listOfTasks>\n"
                                                     "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
                                                     "  </listOfTasks>\n"
                                                     "</sedML>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];

    simulation->setOdeSolver(libOpenCOR::SolverForwardEuler::create());

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), expectedSerialisation);
}

TEST(SerialiseSedTest, cvodeSolverWithAdamsMoultonInterationMethod)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000475", "Adams-Moulton"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithFunctionalIterationType)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000476", "Functional"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Banded"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithDiagonalLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Diagonal"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "GMRES"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "BiCGStab"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithTfqmrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "TFQMR"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000478", "No"}}));
}

TEST(SerialiseSedTest, cvodeSolverWithNoInterpolateSolution)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000481", "false"}}));
}

TEST(SerialiseSedTest, kinsolWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), kinsolExpectedSerialisation({{"KISAO:0000477", "Banded"}}));
}

TEST(SerialiseSedTest, kinsolWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), kinsolExpectedSerialisation({{"KISAO:0000477", "GMRES"}}));
}

TEST(SerialiseSedTest, kinsolWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), kinsolExpectedSerialisation({{"KISAO:0000477", "BiCGStab"}}));
}

TEST(SerialiseSedTest, kinsolWithTfmqrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = document->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), kinsolExpectedSerialisation({{"KISAO:0000477", "TFQMR"}}));
}

TEST(SerialiseSedTest, oneStepSimulation)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <oneStep id=\"simulation1\" step=\"1\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "  <listOfTasks>\n"
                                                     "    <task id=\"task1\" modelReference=\"model1\" simulationReference=\"simulation1\"/>\n"
                                                     "  </listOfTasks>\n"
                                                     "</sedML>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    document->removeSimulation(document->simulations()[0]);

    auto simulation = libOpenCOR::SedOneStep::create(document);

    document->addSimulation(simulation);

    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), expectedSerialisation);
}

TEST(SerialiseSedTest, sedmlFile)
{
    static const libOpenCOR::ExpectedIssues expectedIssues = {
        {libOpenCOR::Issue::Type::WARNING, "The model 'cellml_2.cellml' could not be found. It has been automatically added, but it is empty."},
    };
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"50\" numberOfSteps=\"50000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sedML>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));
    auto document = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ_ISSUES(document, expectedIssues);
    EXPECT_EQ(document->serialise(libOpenCOR::LOCAL_BASE_PATH), expectedSerialisation);
}

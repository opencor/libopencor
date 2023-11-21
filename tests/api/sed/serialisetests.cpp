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
           "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
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
             "</sed>\n";
}

std::string kinsolExpectedSerialisation(const std::map<std::string, std::string> &pParameters = {})
{
    auto linearSolver = pParameters.find("KISAO:0000477");

    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
           "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
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
             "</sed>\n";
}

} // namespace

TEST(SerialiseSedDocumentTest, localCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, localCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));

    auto sed = libOpenCOR::SedDocument::create(file);

#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(sed->serialise(), cvodeExpectedSerialisation("file:///P:/some/path/file.txt"));
#else
    EXPECT_EQ(sed->serialise(), cvodeExpectedSerialisation("file:///some/path/file.txt"));
#endif
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath().append("../..")), cvodeExpectedSerialisation("tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE);

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));

    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::REMOTE_BASE_PATH), cvodeExpectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(), cvodeExpectedSerialisation("https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, relativeRemoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(std::string(libOpenCOR::REMOTE_BASE_PATH).append("/../..")), cvodeExpectedSerialisation("tests/res/cellml_2.cellml"));
}

TEST(ModelTypeSedDocumentTest, daeModel)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
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
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/dae.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), expectedSerialisation);
}

TEST(ModelTypeSedDocumentTest, nlaModel)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), kinsolExpectedSerialisation());
}

TEST(ModelTypeSedDocumentTest, algebraicModel)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"api/sed/algebraic.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <steadyState id=\"simulation1\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/algebraic.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, fixedStepOdeSolver)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
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
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];

    simulation->setOdeSolver(libOpenCOR::SolverForwardEuler::create());

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithAdamsMoultonInterationMethod)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIntegrationMethod(libOpenCOR::SolverCvode::IntegrationMethod::ADAMS_MOULTON);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000475", "Adams-Moulton"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithFunctionalIterationType)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setIterationType(libOpenCOR::SolverCvode::IterationType::FUNCTIONAL);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000476", "Functional"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BANDED);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Banded"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithDiagonalLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::DIAGONAL);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "Diagonal"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::GMRES);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "GMRES"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::BICGSTAB);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "BiCGStab"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithTfqmrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setLinearSolver(libOpenCOR::SolverCvode::LinearSolver::TFQMR);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000477", "TFQMR"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithNoPreconditioner)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setPreconditioner(libOpenCOR::SolverCvode::Preconditioner::NO);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000478", "No"}}));
}

TEST(SerialiseSedDocumentTest, cvodeSolverWithNoInterpolateSolution)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverCvode>(simulation->odeSolver());

    solver->setInterpolateSolution(false);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), cvodeExpectedSerialisation("cellml_2.cellml", {{"KISAO:0000481", "false"}}));
}

TEST(ModelTypeSedDocumentTest, kinsolWithBandedLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BANDED);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), kinsolExpectedSerialisation({{"KISAO:0000477", "Banded"}}));
}

TEST(ModelTypeSedDocumentTest, kinsolWithGmresLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::GMRES);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), kinsolExpectedSerialisation({{"KISAO:0000477", "GMRES"}}));
}

TEST(ModelTypeSedDocumentTest, kinsolWithBicgstabLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::BICGSTAB);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), kinsolExpectedSerialisation({{"KISAO:0000477", "BiCGStab"}}));
}

TEST(ModelTypeSedDocumentTest, kinsolWithTfmqrLinearSolver)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/sed/nla.cellml"));
    auto sed = libOpenCOR::SedDocument::create(file);
    auto simulation = sed->simulations()[0];
    auto solver = dynamic_pointer_cast<libOpenCOR::SolverKinsol>(simulation->nlaSolver());

    solver->setLinearSolver(libOpenCOR::SolverKinsol::LinearSolver::TFQMR);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), kinsolExpectedSerialisation({{"KISAO:0000477", "TFQMR"}}));
}

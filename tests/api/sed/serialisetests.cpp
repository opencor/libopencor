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

TEST(SerialiseSedDocumentTest, localCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, localCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
#ifdef BUILDING_USING_MSVC
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"file:///P:/some/path/file.txt\"/>\n"
#else
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"file:///some/path/file.txt\"/>\n"
#endif
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    sed->initialise(file);

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath().append("../..")), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    sed->initialise(file);

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::REMOTE_BASE_PATH), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(), expectedSerialisation);
}

TEST(SerialiseSedDocumentTest, relativeRemoteCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model1\" language=\"urn:sedml:language:cellml\" source=\"tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "  <listOfSimulations>\n"
                                                     "    <uniformTimeCourse id=\"simulation1\" initialTime=\"0\" outputStartTime=\"0\" outputEndTime=\"1000\" numberOfSteps=\"1000\"/>\n"
                                                     "  </listOfSimulations>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(std::string(libOpenCOR::REMOTE_BASE_PATH).append("/../..")), expectedSerialisation);
}

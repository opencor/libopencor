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

std::string expectedSerialisation(const std::string &pSource)
{
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
                       "          <algorithmParameter kisaoID=\"KISAO:0000475\" value=\"BDF\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000476\" value=\"Newton\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000477\" value=\"Dense\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000478\" value=\"Banded\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000479\" value=\"0\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000480\" value=\"0\"/>\n"
                       "          <algorithmParameter kisaoID=\"KISAO:0000481\" value=\"true\"/>\n"
                       "        </listOfAlgorithmParameters>\n"
                       "      </algorithm>\n"
                       "    </uniformTimeCourse>\n"
                       "  </listOfSimulations>\n"
                       "</sed>\n";
}

} // namespace

TEST(SerialiseSedDocumentTest, localCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath()), expectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, localCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    sed->initialise(file);

#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(sed->serialise(), expectedSerialisation("file:///P:/some/path/file.txt"));
#else
    EXPECT_EQ(sed->serialise(), expectedSerialisation("file:///some/path/file.txt"));
#endif
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::resourcePath().append("../..")), expectedSerialisation("tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    file->setContents(libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    sed->initialise(file);

    EXPECT_EQ(sed->serialise(), expectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(libOpenCOR::REMOTE_BASE_PATH), expectedSerialisation("cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithoutBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(), expectedSerialisation("https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml"));
}

TEST(SerialiseSedDocumentTest, relativeRemoteCellmlFileWithBasePath)
{
    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create();

    sed->initialise(file);

    EXPECT_EQ(sed->serialise(std::string(libOpenCOR::REMOTE_BASE_PATH).append("/../..")), expectedSerialisation("tests/res/cellml_2.cellml"));
}

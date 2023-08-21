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
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise(libOpenCOR::resourcePath()));
}

TEST(SerialiseSedDocumentTest, localCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
#ifdef BUILDING_USING_MSVC
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"file:///P:/some/path/file.txt\"/>\n"
#else
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"file:///some/path/file.txt\"/>\n"
#endif
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::LOCAL_FILE,
                                         libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise());
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise(libOpenCOR::resourcePath().append("../..")));
}

TEST(SerialiseSedDocumentTest, relativeLocalCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::CELLML_2_FILE,
                                         libOpenCOR::charArrayToVector(libOpenCOR::SOME_CELLML_CONTENTS));
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise());
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise(libOpenCOR::REMOTE_BASE_PATH));
}

TEST(SerialiseSedDocumentTest, remoteCellmlFileWithoutBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise());
}

TEST(SerialiseSedDocumentTest, relativeRemoteCellmlFileWithBasePath)
{
    static const std::string expectedSerialisation = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                     "<sed xmlns=\"http://sed-ml.org/sed-ml/level1/version4\" level=\"1\" version=\"4\">\n"
                                                     "  <listOfModels>\n"
                                                     "    <model id=\"model000000001\" language=\"urn:sedml:language:cellml\" source=\"tests/res/cellml_2.cellml\"/>\n"
                                                     "  </listOfModels>\n"
                                                     "</sed>\n";

    auto file = libOpenCOR::File::create(libOpenCOR::REMOTE_FILE);
    auto sed = libOpenCOR::SedDocument::create(file);

    EXPECT_EQ(expectedSerialisation, sed->serialise(std::string(libOpenCOR::REMOTE_BASE_PATH).append("/../..")));
}

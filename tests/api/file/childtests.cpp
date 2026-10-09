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

#include "utils.h"

#include "tests/utils.h"

#include <libopencor>

namespace {

void doTestDataset(const std::string &pNumber, const std::vector<std::string> &pSpecificChildFileNames)
{
    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/file/dataset_" + pNumber + ".omex"))};

    EXPECT_TRUE(file->hasChildFiles());
    EXPECT_EQ(file->childFileCount(), pSpecificChildFileNames.size() + 1);
    EXPECT_EQ(file->childFileNames().size(), pSpecificChildFileNames.size() + 1);
    EXPECT_EQ(file->childFiles().size(), pSpecificChildFileNames.size() + 1);

    size_t index {std::numeric_limits<std::size_t>::max()};
    auto simulationFile {file->childFile("simulation.json")};

    EXPECT_NE(file->childFile(++index), nullptr);
    EXPECT_NE(simulationFile, nullptr);

    for (const auto &specificChildFileName : pSpecificChildFileNames) {
        EXPECT_NE(file->childFile(++index), nullptr);
        EXPECT_NE(file->childFile(specificChildFileName), nullptr);
    }

    EXPECT_EQ(file->childFile(++index), nullptr);
    EXPECT_EQ(file->childFile(libOpenCOR::resourcePath("unknown_file.txt")), nullptr);

    EXPECT_EQ(libOpenCOR::toString(simulationFile->contents()), libOpenCOR::textFileContents(libOpenCOR::resourcePath("api/file/dataset_" + pNumber + ".json")));
}

} // namespace

TEST(ChildFileTest, noChildFiles)
{
    auto filePath {libOpenCOR::resourcePath("unknown_file.txt")};
    auto file {libOpenCOR::File::create(filePath)};

    EXPECT_FALSE(file->hasChildFiles());
    EXPECT_EQ(file->childFileCount(), 0U);
    EXPECT_EQ(file->childFileNames().size(), 0U);
    EXPECT_EQ(file->childFiles().size(), 0U);
    EXPECT_EQ(file->childFile(0), nullptr);
    EXPECT_EQ(file->childFile(filePath), nullptr);
}

TEST(ChildFileTest, dataset135)
{
    doTestDataset("135", {"HumanSAN_Fabbri_Fantini_Wilders_Severi_2017.cellml"});
}

TEST(ChildFileTest, dataset157)
{
    doTestDataset("157", {"fabbri_et_al_based_composite_SAN_model.cellml", "fabbri_et_al_based_composite_SAN_model.sedml"});
}

TEST(ChildFileTest, remoteVirtualCombineArchives)
{
    // Two remote COMBINE archives that are not downloaded must not share their child files.

    auto file135 {libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_BASE_PATH) + "/api/file/dataset_135.omex", false)};
    auto file157 {libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_BASE_PATH) + "/api/file/dataset_157.omex", false)};

    file135->setContents(libOpenCOR::fileContents(libOpenCOR::resourcePath("api/file/dataset_135.omex")));
    file157->setContents(libOpenCOR::fileContents(libOpenCOR::resourcePath("api/file/dataset_157.omex")));

    EXPECT_EQ(file135->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
    EXPECT_EQ(file157->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);

    auto simulationFile135 {file135->childFile("simulation.json")};
    auto simulationFile157 {file157->childFile("simulation.json")};

    ASSERT_NE(simulationFile135, nullptr);
    ASSERT_NE(simulationFile157, nullptr);
    EXPECT_NE(simulationFile135, simulationFile157);
    EXPECT_EQ(libOpenCOR::toString(simulationFile135->contents()), libOpenCOR::textFileContents(libOpenCOR::resourcePath("api/file/dataset_135.json")));
    EXPECT_EQ(libOpenCOR::toString(simulationFile157->contents()), libOpenCOR::textFileContents(libOpenCOR::resourcePath("api/file/dataset_157.json")));
}

TEST(ChildFileTest, remoteCombineArchive)
{
    // The child files of a remote COMBINE archive must be retrievable using their file name, even if the local copy of
    // the COMBINE archive is in a directory that is accessed through a symbolic link (e.g., on macOS, the temporary
    // directory is "/var/...", which is a symbolic link to "/private/var/...").

    auto file {libOpenCOR::File::create(std::string(libOpenCOR::REMOTE_BASE_PATH) + "/api/file/dataset_135.omex")};
    auto simulationFile {file->childFile("simulation.json")};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
    ASSERT_NE(simulationFile, nullptr);
    EXPECT_EQ(libOpenCOR::toString(simulationFile->contents()), libOpenCOR::textFileContents(libOpenCOR::resourcePath("api/file/dataset_135.json")));
}

TEST(ChildFileTest, combineArchiveReplacedWithOtherContents)
{
    // Replacing the contents of a COMBINE archive with some other contents must release its child files.

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.omex"))};
    auto &fileManager {libOpenCOR::FileManager::instance()};

    EXPECT_EQ(fileManager.fileCount(), 3U);

    file->setContents(libOpenCOR::fileContents(libOpenCOR::resourcePath("cellml_2.cellml")));

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
    EXPECT_FALSE(file->hasChildFiles());
    EXPECT_EQ(fileManager.fileCount(), 1U);
}

TEST(ChildFileTest, combineArchiveWithFilesOutsideOfIt)
{
    // A COMBINE archive may reference files that are located outside of it (e.g., "../sibling.cellml" or even the
    // COMBINE archive itself), in which case they must be ignored rather than replace the contents of another managed
    // file (or result in an infinite recursion).

    static const libOpenCOR::ExpectedIssues EXPECTED_ISSUES {{
        {libOpenCOR::Issue::Type::WARNING, "COMBINE archive: the file '../sibling.cellml' is not located within the COMBINE archive. It has been ignored."},
        {libOpenCOR::Issue::Type::WARNING, "COMBINE archive: the file '%2E%2E/encoded_sibling.cellml' is not located within the COMBINE archive. It has been ignored."},
        {libOpenCOR::Issue::Type::WARNING, "COMBINE archive: the file '../entries_outside_of_archive.omex' is not located within the COMBINE archive. It has been ignored."},
    }};

    auto sibling {libOpenCOR::File::create(libOpenCOR::resourcePath("api/file/sibling.cellml"), false)};
    auto someContents {libOpenCOR::charArrayToUnsignedChars("Some contents.")};

    sibling->setContents(someContents);

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("api/file/entries_outside_of_archive.omex"))};

    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
    EXPECT_EQ(file->childFileCount(), 1U);
    EXPECT_NE(file->childFile("model.cellml"), nullptr);
    EXPECT_EQ_ISSUES(file, EXPECTED_ISSUES);
    EXPECT_EQ(sibling->contents(), someContents);
    EXPECT_EQ(libOpenCOR::FileManager::instance().fileCount(), 3U);
}

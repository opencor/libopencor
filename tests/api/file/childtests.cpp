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
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath("api/file/dataset_" + pNumber + ".omex"));

    EXPECT_TRUE(file->hasChildFiles());

    auto childFileNames = file->childFileNames();

    EXPECT_EQ(childFileNames.size(), pSpecificChildFileNames.size() + 1);

    auto childFileNameDir = libOpenCOR::pathToString(libOpenCOR::stringToPath(file->fileName() + ".contents/"));

    for (const auto &childFileName : childFileNames) {
        EXPECT_TRUE(childFileName.starts_with(childFileNameDir));
    }

    EXPECT_EQ(file->childFiles().size(), pSpecificChildFileNames.size() + 1);

    auto simulationFile = file->childFile(childFileNameDir + "simulation.json");

    for (const auto &specificChildFileName : pSpecificChildFileNames) {
        EXPECT_NE(file->childFile(childFileNameDir + specificChildFileName), nullptr);
    }

    EXPECT_NE(simulationFile, nullptr);
    EXPECT_EQ(file->childFile(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE)), nullptr);

    EXPECT_EQ(libOpenCOR::toString(simulationFile->contents()), libOpenCOR::textFileContents(libOpenCOR::resourcePath("api/file/dataset_" + pNumber + ".json")));
}

} // namespace

TEST(ChildFileTest, noChildFiles)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));

    EXPECT_FALSE(file->hasChildFiles());
    EXPECT_EQ(file->childFileNames().size(), 0);
    EXPECT_EQ(file->childFiles().size(), 0);
    EXPECT_EQ(file->childFile(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE)), nullptr);
}

TEST(ChildFileTest, dataset135)
{
    doTestDataset("135", {"HumanSAN_Fabbri_Fantini_Wilders_Severi_2017.cellml"});
}

TEST(ChildFileTest, dataset157)
{
    doTestDataset("157", {"fabbri_et_al_based_composite_SAN_model.cellml", "fabbri_et_al_based_composite_SAN_model.sedml"});
}

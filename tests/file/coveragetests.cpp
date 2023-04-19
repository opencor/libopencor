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

static constexpr const char *UNDERCONSTRAINED_CELLML_LOCAL_FILE = "underconstrained.cellml";
/*---GRY--- TO BE ENABLED ONCE IT HAS BEEN MERGED IN THE MASTER BRANCH.
static constexpr const char *UNDERCONSTRAINED_CELLML_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/underconstrained.cellml";
*/

TEST(CoverageTest, underconstrainedCellmlLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(UNDERCONSTRAINED_CELLML_LOCAL_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

/*---GRY--- TO BE ENABLED ONCE IT HAS BEEN MERGED IN THE MASTER BRANCH.
TEST(CoverageTest, underconstrainedCellmlRemoteFile)
{
    auto file = libOpenCOR::File::create(UNDERCONSTRAINED_CELLML_REMOTE_FILE);

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}
*/

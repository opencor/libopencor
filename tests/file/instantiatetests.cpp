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

static constexpr const char *NON_RESOLVABLE_LOCAL_FILE = "non_resolvable_file.txt";

static constexpr const char *NON_RESOLVABLE_REMOTE_FILE = "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/non_resolvable_file.txt";

TEST(InstantiateFileTest, instantiateNonResolvableLocalFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(NON_RESOLVABLE_LOCAL_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateNonResolvableRemoteFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(NON_RESOLVABLE_REMOTE_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

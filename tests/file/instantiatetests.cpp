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

TEST(InstantiateFileTest, instantiateNonRetrievableFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::NON_RETRIEVABLE_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
}

TEST(InstantiateFileTest, instantiateUnknownFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateSbmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SBML_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateErrorSedmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_SEDML_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCellml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_1_X_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::OK);
}

TEST(InstantiateFileTest, instantiateSedml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_1_X_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCombine1xArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_1_X_ARCHIVE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCellml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::OK);
}

TEST(InstantiateFileTest, instantiateSedml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

TEST(InstantiateFileTest, instantiateCombine2Archive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));

    EXPECT_EQ(file->instantiate(), libOpenCOR::File::Status::NON_INSTANTIABLE_FILE);
}

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

TEST(ResolveFileTest, resolveNonRetrievableFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::NON_RETRIEVABLE_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::NON_RETRIEVABLE_FILE);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNRESOLVED);
}

TEST(ResolveFileTest, resolveUnknownFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::UNKNOWN_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveSbmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SBML_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::UNKNOWN_FILE);
}

TEST(ResolveFileTest, resolveErrorSedmlFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::ERROR_SEDML_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCellml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_1_X_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml1xFile)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_1_X_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine1xArchive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_1_X_ARCHIVE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

TEST(ResolveFileTest, resolveCellml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::CELLML_2_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::CELLML_FILE);
}

TEST(ResolveFileTest, resolveSedml2File)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::SEDML_2_FILE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::SEDML_FILE);
}

TEST(ResolveFileTest, resolveCombine2Archive)
{
    auto file = libOpenCOR::File::create(libOpenCOR::resourcePath(libOpenCOR::COMBINE_2_ARCHIVE));

    EXPECT_EQ(file->resolve(), libOpenCOR::File::Status::OK);
    EXPECT_EQ(file->type(), libOpenCOR::File::Type::COMBINE_ARCHIVE);
}

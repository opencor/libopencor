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

TEST(UtilsTest, encodeUrl)
{
    EXPECT_EQ(libOpenCOR::encodeUrl(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
              "%20!%22#$%25&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D~");
}

TEST(UtilsTest, decodeUrl)
{
    EXPECT_EQ(libOpenCOR::decodeUrl("%20!%22#$%25&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D~"),
              " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
    EXPECT_EQ(libOpenCOR::decodeUrl("%"), "%");
    EXPECT_EQ(libOpenCOR::decodeUrl("%X"), "%X");
    EXPECT_EQ(libOpenCOR::decodeUrl("%XX"), "%XX");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2X"), "%2X");
}

TEST(UtilsTest, isInfOrNan)
{
    EXPECT_FALSE(libOpenCOR::isInfOrNan(0.0));
    EXPECT_TRUE(libOpenCOR::isInfOrNan(libOpenCOR::INF));
    EXPECT_TRUE(libOpenCOR::isInfOrNan(-libOpenCOR::INF));
    EXPECT_TRUE(libOpenCOR::isInfOrNan(libOpenCOR::NAN));
}

TEST(UtilsTest, canonicalFileName)
{
    // Existing file.

    EXPECT_EQ(libOpenCOR::canonicalFileName(std::string(libOpenCOR::RESOURCE_LOCATION) + "/api/../cellml_2.cellml"),
              libOpenCOR::resourcePath("cellml_2.cellml"));

    // Non-existing absolute file.

    EXPECT_EQ(libOpenCOR::canonicalFileName(std::string(libOpenCOR::RESOURCE_LOCATION) + "/non/existing/../../file.txt"),
              libOpenCOR::resourcePath("file.txt"));

    // Non-existing relative files, which must be normalised lexically (i.e. without using the current working directory
    // and while keeping leading ".." components).

#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(libOpenCOR::canonicalFileName(R"(some\.\relative\..\..\path\.\..\dir\file.txt)"), R"(dir\file.txt)");
    EXPECT_EQ(libOpenCOR::canonicalFileName("some/./relative/../../path/./../dir/file.txt"), R"(dir\file.txt)");
    EXPECT_EQ(libOpenCOR::canonicalFileName("../models/lorenz.cellml"), R"(..\models\lorenz.cellml)");
    EXPECT_EQ(libOpenCOR::canonicalFileName("a/../../b"), R"(..\b)");
    EXPECT_EQ(libOpenCOR::canonicalFileName("../../a/./b/../c"), R"(..\..\a\c)");
#else
    EXPECT_EQ(libOpenCOR::canonicalFileName("some/./relative/../../path/./../dir/file.txt"), "dir/file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("../models/lorenz.cellml"), "../models/lorenz.cellml");
    EXPECT_EQ(libOpenCOR::canonicalFileName("a/../../b"), "../b");
    EXPECT_EQ(libOpenCOR::canonicalFileName("../../a/./b/../c"), "../../a/c");
#endif
    EXPECT_EQ(libOpenCOR::canonicalFileName("non_existing_file.txt"), "non_existing_file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName(""), "");

    // File names that are too long for the file system.

    static const std::string TOO_LONG_NAME(5000, 'a');

#ifndef BUILDING_ON_WINDOWS
    EXPECT_EQ(libOpenCOR::canonicalFileName("/" + TOO_LONG_NAME), "/" + TOO_LONG_NAME);
    EXPECT_EQ(libOpenCOR::canonicalFileName("/" + TOO_LONG_NAME + "/../file.txt"), "/file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName(TOO_LONG_NAME + "/../file.txt"), "file.txt");
#endif
    EXPECT_NO_THROW(libOpenCOR::canonicalFileName("/" + TOO_LONG_NAME));
    EXPECT_NO_THROW(libOpenCOR::canonicalFileName(TOO_LONG_NAME));
}

TEST(UtilsTest, canonicalUrl)
{
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com"), "https://example.com");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/"), "https://example.com/");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/model.cellml"), "https://example.com/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/./b/../model.cellml"), "https://example.com/a/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/b/c/./../../g"), "https://example.com/a/g");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/b/.."), "https://example.com/a/");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/b/."), "https://example.com/a/b/");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/b/./"), "https://example.com/a/b/");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/.."), "https://example.com/");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/../../model.cellml"), "https://example.com/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a//b/../model.cellml"), "https://example.com/a/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/..b/.c/model.cellml"), "https://example.com/a/..b/.c/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com/a/../model.cellml?b=../c#d/../e"), "https://example.com/model.cellml?b=../c#d/../e");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com?a=/../b"), "https://example.com?a=/../b");
    EXPECT_EQ(libOpenCOR::canonicalUrl("https://example.com#a/../b"), "https://example.com#a/../b");
    EXPECT_EQ(libOpenCOR::canonicalUrl("http://user@example.com:8080/a/../model.cellml"), "http://user@example.com:8080/model.cellml");
    EXPECT_EQ(libOpenCOR::canonicalUrl("example.com/a/../model.cellml"), "example.com/model.cellml");
#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(libOpenCOR::canonicalUrl(R"(https://example.com\a\..\model.cellml)"), "https://example.com/model.cellml");
#endif
}

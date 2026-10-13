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
    EXPECT_EQ(libOpenCOR::encodeUrl("https://example.com/a%2Fb%3fc d.cellml"), "https://example.com/a%2Fb%3fc%20d.cellml");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://example.com/100%.cellml"), "https://example.com/100%25.cellml");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://example.com/%X%2X%"), "https://example.com/%25X%252X%25");

    // Keep the square brackets of an IP literal as is, but only in the authority of a URL.

    EXPECT_EQ(libOpenCOR::encodeUrl("https://[::1]/a[b].cellml"), "https://[::1]/a%5Bb%5D.cellml");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://[::1]:8080/a[b].cellml"), "https://[::1]:8080/a%5Bb%5D.cellml");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://[::1]?a[b]"), "https://[::1]?a%5Bb%5D");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://[::1]#a[b]"), "https://[::1]#a%5Bb%5D");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://[::1]"), "https://[::1]");
    EXPECT_EQ(libOpenCOR::encodeUrl("https://[fe80::1%en0]/a.cellml"), "https://[fe80::1%25en0]/a.cellml");
    EXPECT_EQ(libOpenCOR::encodeUrl("a[b]://[::1]"), "a%5Bb%5D://[::1]");
}

TEST(UtilsTest, decodeUrl)
{
    EXPECT_EQ(libOpenCOR::decodeUrl("%20!%22#$%25&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D~"),
              " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
    EXPECT_EQ(libOpenCOR::decodeUrl("%"), "%");
    EXPECT_EQ(libOpenCOR::decodeUrl("%X"), "%X");
    EXPECT_EQ(libOpenCOR::decodeUrl("%XX"), "%XX");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2X"), "%2X");

    // Don't decode the reserved characters, the percent sign, and the backslash, but uppercase their hexadecimal digits.

    EXPECT_EQ(libOpenCOR::decodeUrl("%20!%22#$%25&'()*+,-./0123456789:;%3C=%3E?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D%5E_%60abcdefghijklmnopqrstuvwxyz%7B%7C%7D~", false),
              " !\"#$%25&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ%5B%5C%5D^_`abcdefghijklmnopqrstuvwxyz{|}~");
    EXPECT_EQ(libOpenCOR::decodeUrl("%3A%2F%3F%23%5B%5D%40%21%24%26%27%28%29%2A%2B%2C%3B%3D%25%5C", false),
              "%3A%2F%3F%23%5B%5D%40%21%24%26%27%28%29%2A%2B%2C%3B%3D%25%5C");
    EXPECT_EQ(libOpenCOR::decodeUrl("%3a%2f%3f%23%5b%5d%2a%2b%2c%3b%3d%5c", false), "%3A%2F%3F%23%5B%5D%2A%2B%2C%3B%3D%5C");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2541%2e%7e", false), "%2541.~");

    // Percent-encode a percent sign that doesn't start a percent-encoded character, so that decoding a decoded URL
    // doesn't change it.

    EXPECT_EQ(libOpenCOR::decodeUrl("%", false), "%25");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2X", false), "%252X");
    EXPECT_EQ(libOpenCOR::decodeUrl("%%32%30", false), "%2520");
    EXPECT_EQ(libOpenCOR::decodeUrl(libOpenCOR::decodeUrl("%%32%30", false), false), "%2520");

    // Never decode the null character.

    EXPECT_EQ(libOpenCOR::decodeUrl("file.txt%00.cellml"), "file.txt%00.cellml");
    EXPECT_EQ(libOpenCOR::decodeUrl("file.txt%00.cellml", false), "file.txt%00.cellml");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2500"), "%00");
    EXPECT_EQ(libOpenCOR::decodeUrl("%2500", false), "%2500");
}

TEST(UtilsTest, retrieveFileInfo)
{
    // A local file name is never decoded.

    auto [isLocalFile, fileNameOrUrl] {libOpenCOR::retrieveFileInfo("some/path/my%20file%2Ftxt", false)};

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "some/path/my%20file%2Ftxt");

    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("a", false);

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "a");

    // The path of a "file" URL is fully decoded.

#ifdef BUILDING_ON_WINDOWS
    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("file:///C:/some/path/my%20file%25.txt", false);

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "C:/some/path/my file%.txt");

    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("FILE:///C:/some/path/my%20file%25.txt", false);

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "C:/some/path/my file%.txt");
#else
    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("file:///some/path/my%20file%25.txt", false);

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "/some/path/my file%.txt");

    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("FILE:///some/path/my%20file%25.txt", false);

    EXPECT_TRUE(isLocalFile);
    EXPECT_EQ(fileNameOrUrl, "/some/path/my file%.txt");
#endif

    // A remote URL is decoded, except for its reserved characters and percent signs, and then canonicalised. Its
    // scheme and host are also normalised to lowercase, but not its userinfo, path, query, fragment, percent-encoded
    // characters, and the zone identifier of an IPv6 address.

    static const std::vector<std::pair<std::string, std::string>> URLS {
        {"https://example.com/my%20file.cellml", "https://example.com/my file.cellml"},
        {"https://example.com/my file.cellml", "https://example.com/my file.cellml"},
        {"https://example.com/a%2Fb.cellml", "https://example.com/a%2Fb.cellml"},
        {"https://example.com/a%2fb.cellml", "https://example.com/a%2Fb.cellml"},
        {"https://example.com/a/b.cellml", "https://example.com/a/b.cellml"},
        {"https://example.com/x/..%2Fy.cellml", "https://example.com/x/..%2Fy.cellml"},
        {"https://example.com/x/%2E%2E/y.cellml", "https://example.com/y.cellml"},
        {"https://example.com/a%3Fb.cellml", "https://example.com/a%3Fb.cellml"},
        {"https://example.com/a%23b.cellml", "https://example.com/a%23b.cellml"},
        {"https://example.com/a%2541.cellml", "https://example.com/a%2541.cellml"},
        {"https://example.com/a%5Cb.cellml", "https://example.com/a%5Cb.cellml"},
        {"https://example.com/a%5cb.cellml", "https://example.com/a%5Cb.cellml"},
        {"https://example.com/100%.cellml", "https://example.com/100%25.cellml"},
        {"https://example.com/%%32%30.cellml", "https://example.com/%2520.cellml"},
        {"http://example.com/a/../my%20file.cellml?b=%2F%20c", "http://example.com/my file.cellml?b=%2F c"},
        {"HTTPS://example.com/my%20file.cellml", "https://example.com/my file.cellml"},
        {"Http://example.com/a/../my%20file.cellml", "http://example.com/my file.cellml"},
        {"https://EXAMPLE.com/My%20File.cellml", "https://example.com/My File.cellml"},
        {"https://EXAMPLE.com", "https://example.com"},
        {"HTTPS://User:Pass@EXAMPLE.COM:8080/A?B=C#D", "https://User:Pass@example.com:8080/A?B=C#D"},
        {"https://EXAMPLE.com?A=B", "https://example.com?A=B"},
        {"https://EXAMPLE.com#A", "https://example.com#A"},
        {"https://EX%2fAMPLE.com/A", "https://ex%2Fample.com/A"},
        {"https://EX%41MPLE.com/A", "https://example.com/A"},
        {"https://[FE80::1%25EN0]:8080/A", "https://[fe80::1%25EN0]:8080/A"},
    };

    for (const auto &[url, expectedUrl] : URLS) {
        std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo(url);

        EXPECT_FALSE(isLocalFile);
        EXPECT_EQ(fileNameOrUrl, expectedUrl);

        // Retrieving the information of a URL that has already been retrieved must not change it.

        std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo(fileNameOrUrl);

        EXPECT_FALSE(isLocalFile);
        EXPECT_EQ(fileNameOrUrl, expectedUrl);
    }

    // A backslash ends the host of a URL since it is considered to be a forward slash on Windows.

    std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo("https://EXAMPLE.com\\A");

    EXPECT_FALSE(isLocalFile);
#ifdef BUILDING_USING_MSVC
    EXPECT_EQ(fileNameOrUrl, "https://example.com/A");
#else
    EXPECT_EQ(fileNameOrUrl, "https://example.com\\A");
#endif
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

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

TEST(UtilsTest, percentEncodeNonXmlCharacters)
{
    // Valid UTF-8 is kept as is.

    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters(""), "");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("model.cellml"), "model.cellml");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("%20"), "%20");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xC3\xA9"), "\xC3\xA9"); // U+00E9.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xE0\xA0\x80"), "\xE0\xA0\x80"); // U+0800.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xE2\x82\xAC"), "\xE2\x82\xAC"); // U+20AC.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xED\x9F\xBF"), "\xED\x9F\xBF"); // U+D7FF.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF0\x9F\x98\x80"), "\xF0\x9F\x98\x80"); // U+1F600.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF1\x80\x80\x80"), "\xF1\x80\x80\x80"); // U+40000.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF4\x8F\xBF\xBF"), "\xF4\x8F\xBF\xBF"); // U+10FFFF.

    // A byte that is not part of a valid UTF-8 sequence is percent-encoded, i.e. a continuation byte without a lead
    // byte, a lead byte that is never valid, an overlong encoding, a surrogate, a code point above U+10FFFF, a
    // truncated sequence, or a sequence with an invalid continuation byte.

    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("/some/path/\xFF.cellml"), "/some/path/%FF.cellml");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\x80"), "%80");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xC0\xAF"), "%C0%AF");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF5\x80\x80\x80"), "%F5%80%80%80");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xE0\x80\xAF"), "%E0%80%AF");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF0\x80\x80\xAF"), "%F0%80%80%AF");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xED\xA0\x80"), "%ED%A0%80");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF4\x90\x80\x80"), "%F4%90%80%80");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xC3"), "%C3");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF0\x9F\x98"), "%F0%9F%98");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xE2\x82\x41"), "%E2%82A");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xE2\x82\xC3\xA9"), "%E2%82\xC3\xA9");

    // A control character that is not allowed in an XML document is percent-encoded, unlike the tab, line feed, and
    // carriage return control characters, and unlike the delete character.

    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("/some/path/" + std::string(1, '\0') + ".cellml"), "/some/path/%00.cellml");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\x01"), "%01");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\x08\x0B\x0C\x0E\x1F"), "%08%0B%0C%0E%1F");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\t\n\r\x7F"), "\t\n\r\x7F");

    // The U+FFFE and U+FFFF noncharacters are not allowed in an XML document, so they are percent-encoded, unlike the
    // code points next to them.

    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xEF\xBF\xBE"), "%EF%BF%BE");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xEF\xBF\xBF"), "%EF%BF%BF");
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xEF\xBF\xBD"), "\xEF\xBF\xBD"); // U+FFFD.
    EXPECT_EQ(libOpenCOR::percentEncodeNonXmlCharacters("\xF0\x90\x80\x80"), "\xF0\x90\x80\x80"); // U+10000.
}

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

    // A "file" URL refers to a local file if it has no host (with or without an authority) or the "localhost" host
    // (which is case-insensitive and may be percent-encoded), and to a remote file if it has another host (including
    // one with some userinfo since a "file" URL cannot have any). A "file" URL without a path refers to no file, while
    // a "file" URL which path is not absolute is not a valid "file" URL, so it is a local file name.

#ifdef BUILDING_ON_WINDOWS
    static const std::string PATH {"/C:/some/path/my%20file.txt"};
    static const std::string DECODED_PATH {"/C:/some/path/my file.txt"};
    static const std::string FILE_NAME {"C:/some/path/my file.txt"};
#else
    static const std::string PATH {"/some/path/my%20file.txt"};
    static const std::string DECODED_PATH {"/some/path/my file.txt"};
    static const std::string FILE_NAME {"/some/path/my file.txt"};
#endif
    static const std::vector<std::tuple<std::string, bool, std::string>> FILE_URLS {
        {"file://" + PATH, true, FILE_NAME},
        {"file:" + PATH, true, FILE_NAME},
        {"FILE:" + PATH, true, FILE_NAME},
        {"file://localhost" + PATH, true, FILE_NAME},
        {"file://LocalHost" + PATH, true, FILE_NAME},
        {"file://local%68ost" + PATH, true, FILE_NAME},
        {"file://example.com" + PATH, false, "file://example.com" + DECODED_PATH},
        {"file://EXAMPLE.com" + PATH, false, "file://example.com" + DECODED_PATH},
        {"file://localhose" + PATH, false, "file://localhose" + DECODED_PATH},
        {"file://user@localhost" + PATH, false, "file://user@localhost" + DECODED_PATH},
        {"file://", true, ""},
        {"file://localhost", true, ""},
        {"file:", true, "file:"},
        {"file:some/path/my%20file.txt", true, "file:some/path/my%20file.txt"},
    };

    for (const auto &[url, expectedIsLocalFile, expectedFileNameOrUrl] : FILE_URLS) {
        std::tie(isLocalFile, fileNameOrUrl) = libOpenCOR::retrieveFileInfo(url, false);

        EXPECT_EQ(isLocalFile, expectedIsLocalFile) << url;
        EXPECT_EQ(fileNameOrUrl, expectedFileNameOrUrl) << url;
    }

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

TEST(UtilsTest, stringToPathAndPathToString)
{
    // A file name can always be converted to a path and back without being altered, be it valid UTF-8 or not, and even
    // if it contains the three-byte encoding of a surrogate (e.g., "\xED\xA0\x80" for U+D800), possibly followed by a
    // byte that is not valid UTF-8 (in which case they must not be converted to a valid surrogate pair on Windows).

    for (const auto &fileName : std::vector<std::string> {"model.cellml", "\xC3\xA9t\xC3\xA9.cellml", "\xF0\x9F\x98\x80.cellml",
                                                          "\xFF.cellml", "\xC3.cellml", "\xED\xA0\x80.cellml", "\xED\xB0\x80.cellml",
                                                          "\xED\xA0\x80\xED\xB0\x80.cellml", "\xED\xA0\x80\xFF.cellml", "\xED\xB2\x80.cellml"}) {
        EXPECT_EQ(libOpenCOR::pathToString(libOpenCOR::stringToPath(fileName)), fileName);
    }

#ifdef BUILDING_USING_MSVC
    // On Windows, a path may contain lone surrogates, which can be converted to a file name and back without being
    // altered.

    static constexpr wchar_t HIGH_SURROGATE {0xD800};
    static constexpr wchar_t LOW_SURROGATE {0xDC00};
    static constexpr wchar_t LAST_LOW_SURROGATE {0xDFFF};
    static constexpr wchar_t ESCAPED_BYTE_SURROGATE {0xDCFF};

    for (const auto &path : std::vector<std::wstring> {std::wstring {HIGH_SURROGATE} + L".cellml",
                                                       std::wstring {LOW_SURROGATE} + L".cellml",
                                                       std::wstring {LAST_LOW_SURROGATE} + L".cellml",
                                                       std::wstring {HIGH_SURROGATE, HIGH_SURROGATE} + L".cellml",
                                                       std::wstring {LOW_SURROGATE, HIGH_SURROGATE} + L".cellml",
                                                       std::wstring {ESCAPED_BYTE_SURROGATE} + L".cellml",
                                                       std::wstring {HIGH_SURROGATE} + L"a" + std::wstring {ESCAPED_BYTE_SURROGATE} + L".cellml"}) {
        EXPECT_EQ(libOpenCOR::stringToPath(libOpenCOR::pathToString(path)).native(), path);
    }

    // A path with lone surrogates that stand for bytes that form a valid UTF-8 sequence cannot, however, be converted
    // to a file name and back without being altered (e.g., U+DCC3 U+DCA9 and U+00E9 are both converted to
    // "\xC3\xA9").

    static constexpr wchar_t ESCAPED_C3_SURROGATE {0xDCC3};
    static constexpr wchar_t ESCAPED_A9_SURROGATE {0xDCA9};
    static constexpr wchar_t E_ACUTE {0xE9};

    EXPECT_EQ(libOpenCOR::stringToPath(libOpenCOR::pathToString(std::wstring {ESCAPED_C3_SURROGATE, ESCAPED_A9_SURROGATE})).native(), std::wstring {E_ACUTE});
#endif
}

TEST(UtilsTest, hasScheme)
{
    EXPECT_TRUE(libOpenCOR::hasScheme("urn:miriam:biomodels.db:BIOMD0000000012"));
    EXPECT_TRUE(libOpenCOR::hasScheme("HTTPS://example.com/model.cellml"));
    EXPECT_TRUE(libOpenCOR::hasScheme("a1+b-c.d:model.cellml"));

    EXPECT_FALSE(libOpenCOR::hasScheme(""));
    EXPECT_FALSE(libOpenCOR::hasScheme("model.cellml"));
    EXPECT_FALSE(libOpenCOR::hasScheme(":model.cellml"));
    EXPECT_FALSE(libOpenCOR::hasScheme("C:/some/path/model.cellml"));
    EXPECT_FALSE(libOpenCOR::hasScheme("1a:model.cellml"));
    EXPECT_FALSE(libOpenCOR::hasScheme("./urn:model.cellml"));
    EXPECT_FALSE(libOpenCOR::hasScheme("some/path:model.cellml"));
}

TEST(UtilsTest, isSedmlId)
{
    EXPECT_TRUE(libOpenCOR::isSedmlId("a"));
    EXPECT_TRUE(libOpenCOR::isSedmlId("_"));
    EXPECT_TRUE(libOpenCOR::isSedmlId("_1"));
    EXPECT_TRUE(libOpenCOR::isSedmlId("model1"));
    EXPECT_TRUE(libOpenCOR::isSedmlId("My_Model_1"));

    EXPECT_FALSE(libOpenCOR::isSedmlId(""));
    EXPECT_FALSE(libOpenCOR::isSedmlId("1model"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("my model"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model-1"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model.1"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model\x01"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model" + std::string(1, '\0')));
    EXPECT_FALSE(libOpenCOR::isSedmlId("mod\xC3\xA9le"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("\xFF"));
}

TEST(UtilsTest, isCellmlIdentifier)
{
    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier("a"));
    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier("_a"));
    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier("_1a"));
    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier("a_1"));
    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier("My_Variable_1"));

    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier(""));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("_"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("_1"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("1a"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("my variable"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("variable-1"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("variable\x01"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("variable" + std::string(1, '\0')));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("\xC3\xA9t\xC3\xA9"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier("\xFF"));
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

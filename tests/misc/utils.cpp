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

#include <clocale>
#include <cmath>
#include <filesystem>
#include <limits>

#ifdef NAN
#    undef NAN
#endif

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
    // a "file" URL which path is not absolute is not a valid "file" URL, so it is a local file name. The query and
    // fragment of a local "file" URL are ignored, unless they are percent-encoded, in which case they are part of the
    // file name while those of a remote "file" URL are kept.

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
        {"file://" + PATH + "?a=b#c", true, FILE_NAME},
        {"file:" + PATH + "?a=b", true, FILE_NAME},
        {"file://localhost" + PATH + "#c", true, FILE_NAME},
        {"file://localhost?a=b#c", true, ""},
        {"file://localhost#c", true, ""},
        {"file://" + PATH + "%3Fa=b%23c", true, FILE_NAME + "?a=b#c"},
        {"file://example.com" + PATH + "?a=b#c", false, "file://example.com" + DECODED_PATH + "?a=b#c"},
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
    EXPECT_FALSE(libOpenCOR::isSedmlId("model{1}"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model\x01"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("model" + std::string(1, '\0')));
    EXPECT_FALSE(libOpenCOR::isSedmlId("mod\xC3\xA9le"));
    EXPECT_FALSE(libOpenCOR::isSedmlId("\xFF"));

    // A very long string can also be checked (e.g., without a stack overflow).

    static const std::string LONG_STRING(1000000, 'a');

    EXPECT_TRUE(libOpenCOR::isSedmlId(LONG_STRING));
    EXPECT_FALSE(libOpenCOR::isSedmlId(LONG_STRING + " "));
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

    // A very long string can also be checked (e.g., without a stack overflow).

    static const std::string LONG_STRING(1000000, '_');

    EXPECT_TRUE(libOpenCOR::isCellmlIdentifier(LONG_STRING + "a"));
    EXPECT_FALSE(libOpenCOR::isCellmlIdentifier(LONG_STRING));
}

TEST(UtilsTest, isCellmlFormatAndIsSedmlFormat)
{
    // A CellML or SED-ML format is one of the CellML or SED-ML specifications listed in the COMBINE specifications
    // registry (see https://github.com/combine-org/combine-specifications/tree/main/specifications), using either the
    // "http" or the "https" scheme.

    static const std::vector<std::string> SCHEMES {"http", "https"};
    static const std::vector<std::string> CELLML_SPECIFICATIONS {"cellml", "cellml.1", "cellml.1.0", "cellml.1.1", "cellml.2.0"};
    static const std::vector<std::string> SEDML_SPECIFICATIONS {"sed-ml", "sed-ml.level-1", "sed-ml.level-1.version-1", "sed-ml.level-1.version-2", "sed-ml.level-1.version-3", "sed-ml.level-1.version-4", "sed-ml.level-1.version-5"};

    for (const auto &scheme : SCHEMES) {
        auto prefix {scheme + "://identifiers.org/combine.specifications/"};

        for (const auto &specification : CELLML_SPECIFICATIONS) {
            EXPECT_TRUE(libOpenCOR::isCellmlFormat(prefix + specification));
            EXPECT_FALSE(libOpenCOR::isSedmlFormat(prefix + specification));
        }

        for (const auto &specification : SEDML_SPECIFICATIONS) {
            EXPECT_TRUE(libOpenCOR::isSedmlFormat(prefix + specification));
            EXPECT_FALSE(libOpenCOR::isCellmlFormat(prefix + specification));
        }

        // Specifications that are not listed in the COMBINE specifications registry.

        for (const auto &specification : {"cellml.2", "cellml.1.2", "cellml.2.0.1", "cellml.10.0", "cellml.", "cellml.x", "cellml.2.0 ", "cellml2.0", "cellml/2.0", "CellML"}) {
            EXPECT_FALSE(libOpenCOR::isCellmlFormat(prefix + specification));
        }

        for (const auto &specification : {"sedml", "sed-ml.level-2", "sed-ml.level-1.version-6", "sed-ml.level-1.version-", "sed-ml.1.0", "sed-ml.", "sed-ml ", "SED-ML"}) {
            EXPECT_FALSE(libOpenCOR::isSedmlFormat(prefix + specification));
        }
    }

    // Formats that don't use identifiers.org's COMBINE specifications.

    for (const auto &format : {"", "cellml", "sed-ml", "ftp://identifiers.org/combine.specifications/cellml", "HTTP://identifiers.org/combine.specifications/cellml", "http://identifiers.org/combine.specifications:cellml", "http://example.com/combine.specifications/cellml", "http://identifiers.org/combine.specifications/"}) {
        EXPECT_FALSE(libOpenCOR::isCellmlFormat(format));
    }

    for (const auto &format : {"", "cellml", "sed-ml", "ftp://identifiers.org/combine.specifications/sed-ml", "HTTP://identifiers.org/combine.specifications/sed-ml", "http://identifiers.org/combine.specifications:sed-ml", "http://example.com/combine.specifications/sed-ml", "http://identifiers.org/combine.specifications/"}) {
        EXPECT_FALSE(libOpenCOR::isSedmlFormat(format));
    }

    // A very long format can also be checked.

    static const std::string LONG_STRING(1000000, '0');

    EXPECT_FALSE(libOpenCOR::isCellmlFormat("http://identifiers.org/combine.specifications/cellml." + LONG_STRING));
    EXPECT_FALSE(libOpenCOR::isSedmlFormat("http://identifiers.org/combine.specifications/sed-ml." + LONG_STRING));
}

TEST(UtilsTest, isDoubleAndToDouble)
{
    // A double value is a decimal number with an optional sign, an optional fractional part, and an optional exponent,
    // which value must be finite once rounded to the nearest double value (so, a value that underflows is 0).

    static const std::vector<std::pair<std::string, double>> DOUBLES {
        {"1", 1.0},
        {"-1", -1.0},
        {"+1", 1.0},
        {"1.", 1.0},
        {".5", 0.5},
        {"-.5", -0.5},
        {"1.5e3", 1500.0},
        {"1.5E-3", 0.0015},
        {"1e+3", 1000.0},
        {"1.7976931348623157e308", std::numeric_limits<double>::max()},
        {"1e-310", 1e-310},
        {"1e-400", 0.0},
    };

    for (const auto &[string, value] : DOUBLES) {
        EXPECT_TRUE(libOpenCOR::isDouble(string)) << string;
        EXPECT_EQ(libOpenCOR::toDouble(string), value) << string;
    }

    // Anything else is not a double value, including a value that overflows, an infinity, a NaN, a hexadecimal number,
    // and a number with some whitespace.

    static const std::vector<std::string> NON_DOUBLES {
        "",
        ".",
        "+",
        "-",
        "e3",
        "1e",
        "1e+",
        "1.5.3",
        "1,5",
        "1.23abc",
        " 1",
        "1 ",
        "0x1",
        "inf",
        "-inf",
        "nan",
        "1e400",
        "-1e400",
        "1.7976931348623159e308",
    };

    for (const auto &string : NON_DOUBLES) {
        EXPECT_FALSE(libOpenCOR::isDouble(string)) << string;
        EXPECT_TRUE(std::isnan(libOpenCOR::toDouble(string))) << string;
    }

    // A very long string can also be checked (e.g., without a stack overflow).

    static const std::string LONG_STRING(1000000, '0');

    EXPECT_TRUE(libOpenCOR::isDouble("0." + LONG_STRING + "1e+" + LONG_STRING + "1"));
    EXPECT_EQ(libOpenCOR::toDouble(LONG_STRING + "1.5" + LONG_STRING), 1.5);
    EXPECT_FALSE(libOpenCOR::isDouble(LONG_STRING + "x"));

    // A double value doesn't depend on the current C locale, e.g., a French locale which decimal point is a comma.
    // Note: we check this only if such a locale is available.

    const std::string previousLocale {std::setlocale(LC_NUMERIC, nullptr)};

    for (const auto *locale : {"fr_FR.UTF-8", "fr_FR", "fr-FR"}) {
        if (std::setlocale(LC_NUMERIC, locale) != nullptr) {
            EXPECT_EQ(libOpenCOR::toDouble("1.5"), 1.5);
            EXPECT_FALSE(libOpenCOR::isDouble("1,5"));

            break;
        }
    }

    EXPECT_NE(std::setlocale(LC_NUMERIC, previousLocale.c_str()), nullptr);
}

TEST(UtilsTest, isIntAndToInt)
{
    // An int value is 0 or a non-zero decimal number with an optional sign, no leading zero, and an optional
    // non-negative exponent, which value must fit in an int.

    static const std::vector<std::pair<std::string, int>> INTS {
        {"0", 0},
        {"1", 1},
        {"+1", 1},
        {"-1", -1},
        {"123", 123},
        {"1e3", 1000},
        {"1E+3", 1000},
        {"-2e9", -2000000000},
        {"1e09", 1000000000},
        {"21474836e2", 2147483600},
        {"2147483647", std::numeric_limits<int>::max()},
        {"-2147483648", std::numeric_limits<int>::min()},
    };

    for (const auto &[string, value] : INTS) {
        EXPECT_TRUE(libOpenCOR::isInt(string)) << string;
        EXPECT_EQ(libOpenCOR::toInt(string), value) << string;
    }

    // Anything else is not an int value, including a value that doesn't fit in an int.

    static const std::vector<std::string> NON_INTS {
        "",
        "+",
        "-",
        "-0",
        "+0",
        "00",
        "01",
        "a",
        "12x",
        "1.0",
        " 1",
        "1 ",
        "1e",
        "1e+",
        "1ex",
        "1e3x",
        "1e-3",
        "1e10",
        "3e9",
        "2147483648",
        "-2147483649",
        "99999999999",
    };

    for (const auto &string : NON_INTS) {
        EXPECT_FALSE(libOpenCOR::isInt(string)) << string;
        EXPECT_EQ(libOpenCOR::toInt(string), 0) << string;
    }

    // A very long string can also be checked (e.g., without a stack overflow).

    static const std::string LONG_STRING(1000000, '0');

    EXPECT_FALSE(libOpenCOR::isInt("1" + LONG_STRING));
    EXPECT_EQ(libOpenCOR::toInt("1e" + LONG_STRING + "3"), 1000);
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

    // Existing relative file, which must be normalised lexically (i.e. without using the current working directory).

    auto origDir {std::filesystem::current_path()};

    std::filesystem::current_path(libOpenCOR::resourcePath("api"));

#ifdef BUILDING_ON_WINDOWS
    EXPECT_EQ(libOpenCOR::canonicalFileName("../api/./../cellml_2.cellml"), R"(..\cellml_2.cellml)");
#else
    EXPECT_EQ(libOpenCOR::canonicalFileName("../api/./../cellml_2.cellml"), "../cellml_2.cellml");
#endif

    std::filesystem::current_path(origDir);

#ifndef BUILDING_ON_WINDOWS
    // Relative files with a symbolic link followed by a ".." component, which must be kept since the ".." component
    // then refers to the parent directory of the target of the symbolic link (i.e. "real" rather than "cwd").
    // Note: a symbolic link usually requires some special privileges on Windows.

    auto dir {std::filesystem::temp_directory_path() / "libopencor_canonical_file_name_with_symbolic_link"};

    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir / "real/sub/inner");
    std::filesystem::create_directories(dir / "cwd");
    std::filesystem::create_directory_symlink("../real/sub", dir / "cwd/link");

    std::filesystem::current_path(dir / "cwd");

    EXPECT_EQ(libOpenCOR::canonicalFileName("link/../file.txt"), "link/../file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("link/./../file.txt"), "link/../file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("non_existing/../link/../file.txt"), "link/../file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("../cwd/link/../file.txt"), "../cwd/link/../file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("link/../../file.txt"), "link/../../file.txt");
    EXPECT_EQ(libOpenCOR::canonicalFileName("link/../dir/"), "link/../dir/");
    EXPECT_EQ(libOpenCOR::canonicalFileName("link/../dir/."), "link/../dir/");
    EXPECT_EQ(libOpenCOR::canonicalFileName("link/../dir/.."), "link/..");

    // A ".." component that doesn't follow a symbolic link is still normalised lexically.

    EXPECT_EQ(libOpenCOR::canonicalFileName("link/inner/../file.txt"), "link/file.txt");

    std::filesystem::current_path(origDir);
    std::filesystem::remove_all(dir);
#endif

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

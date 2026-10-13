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

#include "libopencor/issue.h"
#include "libopencor/logger.h"

#ifndef __EMSCRIPTEN__
#    include "curl/curl.h"
#endif

#ifdef BUILDING_USING_MSVC
#    include <process.h>
#else
#    include <unistd.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <regex>
#include <sstream>
#include <vector>

#ifdef ERROR
#    undef ERROR
#endif

#ifdef NAN
#    undef NAN
#endif

#ifdef min
#    undef min
#endif

namespace libOpenCOR {

#ifndef CODE_COVERAGE_ENABLED
void printIssues(const LoggerPtr &pLogger, const std::string &pHeader)
{
    std::cout << "---[ISSUES]---[BEGIN]\n";

    if (!pHeader.empty()) {
        std::cout << "---[" << pHeader << "]\n";
    }

    for (const auto &issue : pLogger->issues()) {
        std::cout << ((issue->type() == Issue::Type::ERROR) ? "ERROR" : "WARNING") << ": " << issue->description() << "\n";
    }

    std::cout << "---[ISSUES]---[END]\n";

    std::cout.flush();
}

void printHexDump(const UnsignedChars &pBytes)
{
    static constexpr auto BYTES_PER_LINE {16};
    static constexpr auto ADDRESS_WIDTH {8};
    static constexpr auto FIRST_ASCII_CHARACTER {32};
    static constexpr auto LAST_ASCII_CHARACTER {126};

    std::cout << "---[BYTES]---[BEGIN]\n";

    for (size_t i {0}; i < pBytes.size(); i += BYTES_PER_LINE) {
        // Print the offset.

        std::cout << std::hex << std::setfill('0') << std::setw(ADDRESS_WIDTH) << i << "  ";

        // Print hex bytes.

        for (size_t j {0}; j < BYTES_PER_LINE; ++j) {
            if (i + j < pBytes.size()) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(pBytes[i + j]);
            } else {
                std::cout << "  ";
            }

            // Add an extra space after 8 bytes.

            if (j == ADDRESS_WIDTH - 1) {
                std::cout << "  ";
            } else {
                std::cout << " ";
            }
        }

        std::cout << " |";

        // Print the ASCII representation.

        const size_t bytesOnThisLine {(pBytes.size() - i < BYTES_PER_LINE) ? (pBytes.size() - i) : BYTES_PER_LINE};

        for (size_t j {0}; j < bytesOnThisLine; ++j) {
            auto byte {pBytes[i + j]};

            if (byte >= FIRST_ASCII_CHARACTER && byte <= LAST_ASCII_CHARACTER) {
                std::cout << static_cast<char>(byte);
            } else {
                std::cout << ".";
            }
        }

        for (size_t j {bytesOnThisLine}; j < BYTES_PER_LINE; ++j) {
            std::cout << " ";
        }

        std::cout << "|\n";
    }

    std::cout << std::dec;

    std::cout << "---[BYTES]---[END]\n";

    std::cout.flush();
}

void printArray(const std::string &pName, const Doubles &pDoubles)
{
    std::cout << "---[ARRAY]---[" << pName << "]---[BEGIN]\n";

    if (!pDoubles.empty()) {
        const auto arraySize {pDoubles.size()};
        const auto indexWidth {static_cast<int>(log10(static_cast<double>(arraySize - 1))) + 1};

        for (size_t i {0}; i < arraySize; ++i) {
            std::cout << "[" << std::setfill('0') << std::setw(indexWidth) << i << "] " << pDoubles[i] << "\n";
        }
    }

    std::cout << "---[ARRAY]---[" << pName << "]---[END]\n";

    std::cout.flush();
}
#endif

bool fuzzyCompare(double pNb1, double pNb2)
{
    static constexpr double ONE_TRILLION {1000000000000.0};

    return std::fabs(pNb1 - pNb2) * ONE_TRILLION <= std::fmin(std::fabs(pNb1), std::fabs(pNb2));
}

namespace {

bool isPercentEncodedCharacter(const std::string &pUrl, size_t pIndex)
{
    // Check whether there is a percent-encoded character (e.g., "%20") at the given index of the given URL.

    return (pUrl[pIndex] == '%')
           && (pIndex + 2 < pUrl.size())
           && (std::isxdigit(static_cast<unsigned char>(pUrl[pIndex + 1])) != 0)
           && (std::isxdigit(static_cast<unsigned char>(pUrl[pIndex + 2])) != 0);
}

void appendPercentEncodedByte(std::string &pString, unsigned char pByte)
{
    // Append the percent-encoded version of the given byte (e.g., "%FF" for 0xFF) to the given string.

    static constexpr std::string_view HEX_DIGITS {"0123456789ABCDEF"};
    static constexpr unsigned int HEX_HIGH_NIBBLE_SHIFT {4U};
    static constexpr unsigned int HEX_LOW_NIBBLE_MASK {0x0FU};

    pString += '%';
    pString += HEX_DIGITS[pByte >> HEX_HIGH_NIBBLE_SHIFT];
    pString += HEX_DIGITS[pByte & HEX_LOW_NIBBLE_MASK];
}

constexpr unsigned char LAST_ASCII_BYTE {0x7F};
constexpr unsigned char FIRST_CONTINUATION_BYTE {0x80};
constexpr unsigned char LAST_CONTINUATION_BYTE {0xBF};
constexpr unsigned int CONTINUATION_BYTE_MASK {0x3FU};
constexpr unsigned int CONTINUATION_BYTE_SHIFT {6U};

std::tuple<size_t, char32_t> utf8Sequence(const std::string &pString, size_t pIndex)
{
    // Return the length of the valid UTF-8 sequence that starts at the given index of the given string (or 0 if there
    // is none) and the code point that it encodes.
    // Note: an overlong encoding, a surrogate, or a code point above U+10FFFF is not valid UTF-8 (see
    //       https://www.rfc-editor.org/rfc/rfc3629#section-4), hence we check the range of the first continuation byte.

    static constexpr unsigned int FIRST_TWO_BYTE_LEAD {0xC2};
    static constexpr unsigned int LAST_TWO_BYTE_LEAD {0xDF};
    static constexpr unsigned int FIRST_THREE_BYTE_LEAD {0xE0};
    static constexpr unsigned int SURROGATE_THREE_BYTE_LEAD {0xED};
    static constexpr unsigned int LAST_THREE_BYTE_LEAD {0xEF};
    static constexpr unsigned int FIRST_FOUR_BYTE_LEAD {0xF0};
    static constexpr unsigned int LAST_FOUR_BYTE_LEAD {0xF4};
    static constexpr unsigned int FIRST_THREE_BYTE_LEAD_LOW {0xA0};
    static constexpr unsigned int SURROGATE_THREE_BYTE_LEAD_HIGH {0x9F};
    static constexpr unsigned int FIRST_FOUR_BYTE_LEAD_LOW {0x90};
    static constexpr unsigned int LAST_FOUR_BYTE_LEAD_HIGH {0x8F};

    const auto byte {static_cast<unsigned char>(pString[pIndex])};

    if (byte <= LAST_ASCII_BYTE) {
        return {1, byte};
    }

    size_t continuationByteCount {0};
    unsigned int low {FIRST_CONTINUATION_BYTE};
    unsigned int high {LAST_CONTINUATION_BYTE};

    if ((byte >= FIRST_TWO_BYTE_LEAD) && (byte <= LAST_TWO_BYTE_LEAD)) {
        continuationByteCount = 1;
    } else if ((byte >= FIRST_THREE_BYTE_LEAD) && (byte <= LAST_THREE_BYTE_LEAD)) {
        continuationByteCount = 2;
        low = (byte == FIRST_THREE_BYTE_LEAD) ? FIRST_THREE_BYTE_LEAD_LOW : FIRST_CONTINUATION_BYTE;
        high = (byte == SURROGATE_THREE_BYTE_LEAD) ? SURROGATE_THREE_BYTE_LEAD_HIGH : LAST_CONTINUATION_BYTE;
    } else if ((byte >= FIRST_FOUR_BYTE_LEAD) && (byte <= LAST_FOUR_BYTE_LEAD)) {
        continuationByteCount = 3;
        low = (byte == FIRST_FOUR_BYTE_LEAD) ? FIRST_FOUR_BYTE_LEAD_LOW : FIRST_CONTINUATION_BYTE;
        high = (byte == LAST_FOUR_BYTE_LEAD) ? LAST_FOUR_BYTE_LEAD_HIGH : LAST_CONTINUATION_BYTE;
    } else {
        return {0, 0};
    }

    auto codePoint {static_cast<char32_t>(byte & (CONTINUATION_BYTE_MASK >> continuationByteCount))};

    for (size_t i {1}; i <= continuationByteCount; ++i) {
        if (pIndex + i >= pString.size()) {
            return {0, 0};
        }

        const auto continuationByte {static_cast<unsigned char>(pString[pIndex + i])};

        if ((continuationByte < ((i == 1) ? low : FIRST_CONTINUATION_BYTE))
            || (continuationByte > ((i == 1) ? high : LAST_CONTINUATION_BYTE))) {
            return {0, 0};
        }

        codePoint = (codePoint << CONTINUATION_BYTE_SHIFT) | (continuationByte & CONTINUATION_BYTE_MASK);
    }

    return {continuationByteCount + 1, codePoint};
}

} // namespace

std::string percentEncodeNonXmlCharacters(const std::string &pString)
{
    // Percent-encode the bytes of the given string that are not part of a valid UTF-8 sequence (e.g., "\xFF.cellml"
    // becomes "%FF.cellml") or that are part of a valid UTF-8 sequence for a code point that is not allowed in an XML
    // document (e.g., "\x01.cellml" becomes "%01.cellml" and "\xEF\xBF\xBF.cellml" becomes "%EF%BF%BF.cellml"), leaving
    // everything else as is.
    // Note #1: an XML document may only contain the tab, line feed, and carriage return control characters, and no
    //          U+FFFE and U+FFFF noncharacters (see https://www.w3.org/TR/xml/#charsets). A null character would, in
    //          addition, truncate the string when used as a C string (e.g., by libxml2).
    // Note #2: libxml2 escapes the tab, line feed, and carriage return control characters in an XML attribute (e.g.,
    //          "\t" becomes "&#9;"), so that they don't get normalised to a space when the XML document is parsed,
    //          hence we leave them as is.

    static constexpr char32_t TAB {0x09};
    static constexpr char32_t LINE_FEED {0x0A};
    static constexpr char32_t CARRIAGE_RETURN {0x0D};
    static constexpr char32_t FIRST_NON_CONTROL_CHARACTER {0x20};
    static constexpr char32_t FIRST_NONCHARACTER {0xFFFE};
    static constexpr char32_t LAST_NONCHARACTER {0xFFFF};

    std::string res;

    res.reserve(pString.size());

    for (size_t i {0}; i < pString.size();) {
        auto [length, codePoint] {utf8Sequence(pString, i)};

        if ((length == 0)
            || ((codePoint < FIRST_NON_CONTROL_CHARACTER) && (codePoint != TAB) && (codePoint != LINE_FEED)
                && (codePoint != CARRIAGE_RETURN))
            || ((codePoint >= FIRST_NONCHARACTER) && (codePoint <= LAST_NONCHARACTER))) {
            const auto byteCount {std::max<size_t>(length, 1)};

            for (size_t j {0}; j < byteCount; ++j) {
                appendPercentEncodedByte(res, static_cast<unsigned char>(pString[i + j]));
            }

            i += byteCount;
        } else {
            res.append(pString, i, length);

            i += length;
        }
    }

    return res;
}

std::string encodeUrl(const std::string &pUrl)
{
    static constexpr std::string_view URL_SAFE_CHARACTERS {"!#$&'()*+,-./:;=?@_~"};

    static constexpr std::string_view SCHEME_SEPARATOR {"://"};
    static constexpr std::string_view AUTHORITY_TERMINATORS {"/?#"};

    // Determine where the authority of the URL is (e.g., "example.com:8080" in "https://example.com:8080/a/b"), if any.

    auto authorityStart {pUrl.find(SCHEME_SEPARATOR)};
    auto authorityEnd {std::string::npos};

    if (authorityStart != std::string::npos) {
        authorityStart += SCHEME_SEPARATOR.size();
        authorityEnd = pUrl.find_first_of(AUTHORITY_TERMINATORS, authorityStart);
    }

    std::string res;

    res.reserve(pUrl.size() * 3); // NOLINT

    for (size_t i {0}; i < pUrl.size(); ++i) {
        const auto urlChar {pUrl[i]};
        const auto unsignedChar {static_cast<unsigned char>(urlChar)};

        // Keep a percent-encoded character as is (e.g., "%2F" in "https://example.com/a%2Fb"), so that it doesn't get
        // encoded twice.

        if (isPercentEncodedCharacter(pUrl, i)) {
            res += urlChar;

            continue;
        }

        // Keep the square brackets of an IP literal as is (e.g., "[::1]" in "https://[::1]:8080/a/b"), since they are
        // only allowed in the authority of a URL (see https://www.rfc-editor.org/rfc/rfc3986#section-3.2.2) and
        // encoding them would result in a host name that cannot be resolved.

        if (((urlChar == '[') || (urlChar == ']')) && (authorityStart != std::string::npos) && (i >= authorityStart)
            && (i < authorityEnd)) {
            res += urlChar;

            continue;
        }

        // Encode the character if it is not alphanumeric and not one of the URL safe characters (see
        // https://www.rfc-editor.org/rfc/rfc3986#section-2.3).

        if ((isalnum(unsignedChar) != 0) || (URL_SAFE_CHARACTERS.find(urlChar) != std::string::npos)) {
            res += urlChar;
        } else {
            appendPercentEncodedByte(res, unsignedChar);
        }
    }

    return res;
}

std::string decodeUrl(const std::string &pUrl, bool pDecodeReservedCharacters)
{
    // Decode the given URL.
    // Note #1: if pDecodeReservedCharacters is false, we don't decode the characters that have a meaning in a URL, i.e.
    //          the reserved characters (see https://www.rfc-editor.org/rfc/rfc3986#section-2.2) and the percent sign
    //          (see https://www.rfc-editor.org/rfc/rfc3986#section-2.4). Indeed, decoding them would result in a
    //          different URL (e.g., "https://example.com/a%2Fb" and "https://example.com/a/b" are two different URLs).
    //          We do, however, uppercase their hexadecimal digits (see
    //          https://www.rfc-editor.org/rfc/rfc3986#section-6.2.2.1), so that "https://example.com/a%2fb" and
    //          "https://example.com/a%2Fb" are considered to be the same URL. We also don't decode the backslash since
    //          canonicalUrl() considers it to be a forward slash on Windows (e.g., "https://example.com/a%5Cb" and
    //          "https://example.com/a/b" would otherwise be the same URL on Windows, but not on other platforms).
    // Note #2: if pDecodeReservedCharacters is false, we also percent-encode a percent sign that doesn't start a
    //          percent-encoded character (e.g., "https://example.com/100%" becomes "https://example.com/100%25"), so
    //          that decoding a decoded URL doesn't change it (e.g., "https://example.com/%%32%30" would otherwise
    //          become "https://example.com/%20" and then "https://example.com/ ").
    // Note #3: we never decode the null character since it cannot be part of a file name or URL, and since a file name
    //          with an embedded null character would get truncated when used by the operating system (e.g.,
    //          "file:///some/path/file.txt%00.cellml" would otherwise refer to "/some/path/file.txt").

    static constexpr auto BASE_16 {16};
    static constexpr std::string_view UNDECODED_CHARACTERS {":/?#[]@!$&'()*+,;=%\\"};

    std::string res;

    res.reserve(pUrl.size());

    for (size_t i {0}; i < pUrl.size(); ++i) {
        if (isPercentEncodedCharacter(pUrl, i)) {
            const auto decodedChar {static_cast<char>(std::stoi(pUrl.substr(i + 1, 2), nullptr, BASE_16))};

            if ((decodedChar != '\0')
                && (pDecodeReservedCharacters || (UNDECODED_CHARACTERS.find(decodedChar) == std::string::npos))) {
                res += decodedChar;
            } else {
                res += '%';
                res += static_cast<char>(std::toupper(static_cast<unsigned char>(pUrl[i + 1])));
                res += static_cast<char>(std::toupper(static_cast<unsigned char>(pUrl[i + 2])));
            }

            i += 2;
        } else if (!pDecodeReservedCharacters && (pUrl[i] == '%')) {
            res += "%25";
        } else {
            res += pUrl[i];
        }
    }

    return res;
}

#ifdef BUILDING_USING_MSVC
std::string forwardSlashPath(const std::string &pPath)
{
    static constexpr auto BACKSLASH {"\\\\"};
    static const auto BACKSLASH_REGEX {std::regex(BACKSLASH)};
    static constexpr auto FORWARD_SLASH {"/"};

    return std::regex_replace(pPath, BACKSLASH_REGEX, FORWARD_SLASH);
}
#endif

#ifdef BUILDING_USING_MSVC
namespace {

// Conversion between a UTF-8 std::string and a UTF-16 std::wstring, to be used instead of the one done by
// std::filesystem::path since a std::string may not be valid UTF-8 (e.g., "file:///some/path/%FF.txt" gets
// percent-decoded to "/some/path/\xFF.txt"), in which case std::filesystem::path would throw an exception. So, we map
// an invalid byte to a lone surrogate (e.g., 0xFF to U+DCFF), which a Windows file name may contain, and back, meaning
// that a file name can always be converted to a path and back without being altered.
// Note #1: a path may also contain some other lone surrogates (e.g., U+D800), which cannot be mapped to a byte, in
//          which case we encode them as if they were valid code points (i.e. using three bytes, e.g., "\xED\xA0\x80"
//          for U+D800) and decode them back (see stringToUtf16()), meaning that such a path can be converted to a file
//          name and back without being altered, unless it also has some lone surrogates as described in Note #2 (see
//          stringToUtf16()).
// Note #2: a path with lone surrogates that stand for bytes (i.e. U+DC80 to U+DCFF) that form a valid UTF-8 sequence
//          (e.g., U+DCC3 U+DCA9, i.e. "\xC3\xA9") or the three-byte encoding of a surrogate (e.g., U+DCED U+DCA0
//          U+DC80, i.e. "\xED\xA0\x80") cannot, however, be converted to a file name and back without being altered
//          since that file name is also the one for another path (e.g., U+00E9 or U+D800, respectively). This is
//          inherent to mapping an invalid byte to a lone surrogate, and only a path that was not created by us could
//          have such lone surrogates.

constexpr char32_t FIRST_TWO_BYTE_CODE_POINT {0x80};
constexpr char32_t FIRST_THREE_BYTE_CODE_POINT {0x800};
constexpr char32_t FIRST_FOUR_BYTE_CODE_POINT {0x10000};
constexpr char32_t FIRST_HIGH_SURROGATE {0xD800};
constexpr char32_t LAST_HIGH_SURROGATE {0xDBFF};
constexpr char32_t FIRST_LOW_SURROGATE {0xDC00};
constexpr char32_t LAST_LOW_SURROGATE {0xDFFF};
constexpr char32_t FIRST_ESCAPED_BYTE_SURROGATE {FIRST_LOW_SURROGATE + FIRST_CONTINUATION_BYTE};
constexpr char32_t LAST_ESCAPED_BYTE_SURROGATE {FIRST_LOW_SURROGATE + 0xFF};
constexpr unsigned int SURROGATE_SHIFT {10U};
constexpr char32_t SURROGATE_MASK {0x3FF};

std::wstring utf8ToUtf16(const std::string &pString, bool pDecodeSurrogates)
{
    // Convert the given string to UTF-16, mapping an invalid byte to a lone surrogate (e.g., 0xFF to U+DCFF) and, if
    // requested, decoding a three-byte encoding of a surrogate (e.g., "\xED\xA0\x80" for U+D800) to that surrogate.

    static constexpr unsigned char SURROGATE_LEAD {0xED};
    static constexpr unsigned char FIRST_SURROGATE_CONTINUATION_BYTE {0xA0};
    static constexpr unsigned int THREE_BYTE_LEAD_MASK {0x0FU};

    std::wstring res;

    res.reserve(pString.size());

    for (size_t i {0}; i < pString.size();) {
        auto [length, codePoint] {utf8Sequence(pString, i)};

        if ((length == 0) && pDecodeSurrogates && (i + 2 < pString.size())
            && (static_cast<unsigned char>(pString[i]) == SURROGATE_LEAD)
            && (static_cast<unsigned char>(pString[i + 1]) >= FIRST_SURROGATE_CONTINUATION_BYTE)
            && (static_cast<unsigned char>(pString[i + 1]) <= LAST_CONTINUATION_BYTE)
            && (static_cast<unsigned char>(pString[i + 2]) >= FIRST_CONTINUATION_BYTE)
            && (static_cast<unsigned char>(pString[i + 2]) <= LAST_CONTINUATION_BYTE)) {
            res += static_cast<wchar_t>(((SURROGATE_LEAD & THREE_BYTE_LEAD_MASK) << (2 * CONTINUATION_BYTE_SHIFT))
                                        | ((static_cast<unsigned char>(pString[i + 1]) & CONTINUATION_BYTE_MASK) << CONTINUATION_BYTE_SHIFT)
                                        | (static_cast<unsigned char>(pString[i + 2]) & CONTINUATION_BYTE_MASK));

            i += 3;
        } else if (length == 0) {
            res += static_cast<wchar_t>(FIRST_LOW_SURROGATE + static_cast<unsigned char>(pString[i]));

            ++i;
        } else {
            if (codePoint >= FIRST_FOUR_BYTE_CODE_POINT) {
                codePoint -= FIRST_FOUR_BYTE_CODE_POINT;

                res += static_cast<wchar_t>(FIRST_HIGH_SURROGATE + (codePoint >> SURROGATE_SHIFT));
                res += static_cast<wchar_t>(FIRST_LOW_SURROGATE + (codePoint & SURROGATE_MASK));
            } else {
                res += static_cast<wchar_t>(codePoint);
            }

            i += length;
        }
    }

    return res;
}

std::string utf16ToUtf8(const std::wstring &pString)
{
    static constexpr unsigned char TWO_BYTE_LEAD {0xC0};
    static constexpr unsigned char THREE_BYTE_LEAD {0xE0};
    static constexpr unsigned char FOUR_BYTE_LEAD {0xF0};

    std::string res;

    res.reserve(pString.size());

    for (size_t i {0}; i < pString.size(); ++i) {
        auto codePoint {static_cast<char32_t>(pString[i])};

        if ((codePoint >= FIRST_HIGH_SURROGATE) && (codePoint <= LAST_HIGH_SURROGATE) && (i + 1 < pString.size())
            && (static_cast<char32_t>(pString[i + 1]) >= FIRST_LOW_SURROGATE)
            && (static_cast<char32_t>(pString[i + 1]) <= LAST_LOW_SURROGATE)) {
            codePoint = FIRST_FOUR_BYTE_CODE_POINT + ((codePoint - FIRST_HIGH_SURROGATE) << SURROGATE_SHIFT)
                        + (static_cast<char32_t>(pString[i + 1]) - FIRST_LOW_SURROGATE);

            ++i;
        } else if ((codePoint >= FIRST_ESCAPED_BYTE_SURROGATE) && (codePoint <= LAST_ESCAPED_BYTE_SURROGATE)) {
            res += static_cast<char>(codePoint - FIRST_LOW_SURROGATE);

            continue;
        }

        if (codePoint < FIRST_TWO_BYTE_CODE_POINT) {
            res += static_cast<char>(codePoint);
        } else if (codePoint < FIRST_THREE_BYTE_CODE_POINT) {
            res += static_cast<char>(TWO_BYTE_LEAD | (codePoint >> CONTINUATION_BYTE_SHIFT));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | (codePoint & CONTINUATION_BYTE_MASK));
        } else if (codePoint < FIRST_FOUR_BYTE_CODE_POINT) {
            res += static_cast<char>(THREE_BYTE_LEAD | (codePoint >> (2 * CONTINUATION_BYTE_SHIFT)));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | ((codePoint >> CONTINUATION_BYTE_SHIFT) & CONTINUATION_BYTE_MASK));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | (codePoint & CONTINUATION_BYTE_MASK));
        } else {
            res += static_cast<char>(FOUR_BYTE_LEAD | (codePoint >> (3 * CONTINUATION_BYTE_SHIFT)));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | ((codePoint >> (2 * CONTINUATION_BYTE_SHIFT)) & CONTINUATION_BYTE_MASK));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | ((codePoint >> CONTINUATION_BYTE_SHIFT) & CONTINUATION_BYTE_MASK));
            res += static_cast<char>(FIRST_CONTINUATION_BYTE | (codePoint & CONTINUATION_BYTE_MASK));
        }
    }

    return res;
}

std::wstring stringToUtf16(const std::string &pString)
{
    // Convert the given string to UTF-16, decoding a three-byte encoding of a surrogate (e.g., "\xED\xA0\x80" for
    // U+D800) to that surrogate, as long as the result converts back to the given string. This may not be the case
    // (e.g., "\xED\xA0\x80\xFF" would be converted to U+D800 U+DCFF, which is a valid surrogate pair, i.e. U+100FF,
    // that converts back to "\xF4\x80\x83\xBF"), in which case we don't decode any such encoding (i.e. each of its
    // bytes is mapped to a lone surrogate), which always converts back to the given string.

    auto res {utf8ToUtf16(pString, true)};

    return (utf16ToUtf8(res) == pString) ? res : utf8ToUtf16(pString, false);
}

} // namespace
#endif

std::filesystem::path stringToPath(const std::string &pString)
{
#ifdef BUILDING_USING_MSVC
    return stringToUtf16(pString);
#else
    return std::u8string {pString.begin(), pString.end()};
#endif
}

std::string pathToString(const std::filesystem::path &pPath)
{
    auto path {pPath};

#ifdef BUILDING_USING_MSVC
    return utf16ToUtf8(path.make_preferred().native());
#else
    return path.make_preferred().string();
#endif
}

std::string canonicalFileName(const std::string &pFileName)
{
    // Determine the canonical version of the file name.
    // Note #1: we must never change the current working directory here since it is shared by all the threads of the
    //          process.
    // Note #2: we use the std::error_code versions of the std::filesystem functions so that nothing gets thrown (e.g.,
    //          if the file name is too long or if the file gets deleted while we are dealing with it).
    // Note #3: when building using Emscripten, the file system is virtual and std::filesystem::weakly_canonical()
    //          doesn't work as expected (e.g., it throws an exception rather than return "/" for "/some/path/../.."),
    //          so we only ever normalise the file name lexically.
    // Note #4: a file name with a null character (e.g., "/some/path/file.txt\0.cellml") cannot exist, so we only ever
    //          normalise it lexically (the std::filesystem functions would otherwise only consider the file name up to
    //          that null character, i.e. "/some/path/file.txt").

    // An empty file name stays empty (std::filesystem::canonical() would otherwise return the current working
    // directory).

    if (pFileName.empty()) {
        return {};
    }

    auto filePath {stringToPath(pFileName)};

#ifndef __EMSCRIPTEN__
    if (pFileName.find('\0') != std::string::npos) {
        return pathToString(filePath.lexically_normal());
    }

    // If the file exists then use std::filesystem::canonical() since it resolves symbolic links, etc.
    // Note: this fails if the file doesn't exist.

    std::error_code errorCode;
    auto res {std::filesystem::canonical(filePath, errorCode)};

    if (!errorCode) {
        return pathToString(res);
    }

    // The file doesn't exist, so if its file name has a root directory (i.e., it is an absolute file name or, on
    // Windows, a file name relative to the root of the current drive) then use std::filesystem::weakly_canonical()
    // since it returns a file name that is as close to the canonical version as possible.

    if (filePath.has_root_directory()) {
        res = std::filesystem::weakly_canonical(filePath, errorCode);

        if (!errorCode) {
            return pathToString(res);
        }
    }
#endif

    // The file name is relative (or something went wrong), so normalise it lexically. Unlike
    // std::filesystem::weakly_canonical(), this doesn't depend on the current working directory, and it keeps leading
    // ".." components (e.g., "a/../../b" becomes "../b").

    return pathToString(filePath.lexically_normal());
}

std::string canonicalUrl(const std::string &pUrl)
{
    // Determine the canonical version of the URL, i.e. remove the "." and ".." segments from its path, as described in
    // RFC 3986 (https://www.rfc-editor.org/rfc/rfc3986#section-5.2.4), as well as its empty segments (e.g., "/a//b"
    // becomes "/a/b").
    // Note: a URL is never a local file name, so we must not access the file system (a local file that happens to have
    //       the same name as the host and path of the URL would otherwise be used) or use std::filesystem::path (its
    //       separators are platform specific).

    static constexpr auto FORWARD_SLASH {'/'};
    static constexpr auto DOT {"."};
    static constexpr auto DOT_DOT {".."};

#ifdef BUILDING_USING_MSVC
    auto url {forwardSlashPath(pUrl)};
#else
    const auto &url {pUrl};
#endif

    // Retrieve the scheme and authority (e.g., "https://example.com"), the path (e.g., "/some/path/file.txt"), and the
    // query and/or fragment (e.g., "?a=b#c") of the URL.

    static constexpr auto SCHEME_SEPARATOR {"://"};
    static const auto SCHEME_SEPARATOR_LENGTH {strlen(SCHEME_SEPARATOR)};

    auto schemeSeparatorPos {url.find(SCHEME_SEPARATOR)};
    auto authorityPos {(schemeSeparatorPos == std::string::npos) ? 0 : schemeSeparatorPos + SCHEME_SEPARATOR_LENGTH};
    auto queryOrFragmentPos {url.find_first_of("?#", authorityPos)};
    auto pathPos {std::min(url.find(FORWARD_SLASH, authorityPos), queryOrFragmentPos)};

    if ((pathPos == std::string::npos) || (url[pathPos] != FORWARD_SLASH)) {
        return url;
    }

    auto path {url.substr(pathPos, (queryOrFragmentPos == std::string::npos) ? std::string::npos : queryOrFragmentPos - pathPos)};

    // Remove the ".", "..", and empty segments from the path, keeping a trailing forward slash if the last segment is a
    // ".", "..", or empty segment (e.g., "/a/b/.." and "/a//" become "/a/").
    // Note: path starts with a forward slash, so the first segment that we retrieve is always empty and we skip it.

    std::vector<std::string> segments;
    size_t segmentPos {1};

    while (true) {
        auto nextSegmentPos {path.find(FORWARD_SLASH, segmentPos)};
        auto isLastSegment {nextSegmentPos == std::string::npos};
        auto segment {path.substr(segmentPos, isLastSegment ? std::string::npos : nextSegmentPos - segmentPos)};

        if (segment == DOT_DOT) {
            if (!segments.empty()) {
                segments.pop_back();
            }
        } else if ((segment != DOT) && !segment.empty()) {
            segments.push_back(segment);
        }

        if (isLastSegment) {
            if ((segment == DOT) || (segment == DOT_DOT) || segment.empty()) {
                segments.emplace_back();
            }

            break;
        }

        segmentPos = nextSegmentPos + 1;
    }

    std::string res {url.substr(0, pathPos)};

    for (const auto &segment : segments) {
        res += FORWARD_SLASH;
        res += segment;
    }

    if (queryOrFragmentPos != std::string::npos) {
        res += url.substr(queryOrFragmentPos);
    }

    return res;
}

namespace {

bool startsWithScheme(const std::string &pFileNameOrUrl, std::string_view pScheme)
{
    // Check whether the given file name or URL starts with the given (lowercase) scheme, ignoring case since a scheme
    // is case-insensitive (see https://www.rfc-editor.org/rfc/rfc3986#section-3.1).

    return (pFileNameOrUrl.size() >= pScheme.size())
           && std::equal(pScheme.begin(), pScheme.end(), pFileNameOrUrl.begin(), [](char pSchemeChar, char pChar) {
                  return pSchemeChar == static_cast<char>(std::tolower(static_cast<unsigned char>(pChar)));
              });
}

std::string lowercaseHost(const std::string &pUrl, size_t pAuthorityPos)
{
    // Lowercase the host of the given URL (e.g., "EXAMPLE.com" in "https://user:PASS@EXAMPLE.com:8080/a/b") since it is
    // case-insensitive (see https://www.rfc-editor.org/rfc/rfc3986#section-3.2.2), but not its userinfo (e.g.,
    // "user:PASS"), which is case-sensitive.
    // Note #1: a backslash also ends the authority of the URL since canonicalUrl() considers it to be a forward slash
    //          on Windows (e.g., "https://EXAMPLE.com\A" would otherwise become "https://example.com/a" on Windows).
    // Note #2: we don't lowercase a percent-encoded character (e.g., "%2F") since its hexadecimal digits are uppercase
    //          (see decodeUrl()) nor the zone identifier of an IPv6 address (e.g., "en0" in "[fe80::1%25en0]") since it
    //          is the name of a network interface.
    // Note #3: we only lowercase ASCII letters since a non-ASCII character (e.g., in an internationalised domain name)
    //          is percent-decoded to UTF-8 bytes, which std::tolower() could alter depending on the current locale.

    static constexpr std::string_view AUTHORITY_TERMINATORS {"/?#\\"};

    auto res {pUrl};
    auto hostEnd {std::min(res.find_first_of(AUTHORITY_TERMINATORS, pAuthorityPos), res.size())};
    auto userInfoEnd {res.rfind('@', hostEnd)};
    auto hostStart {(userInfoEnd == std::string::npos) ? pAuthorityPos : userInfoEnd + 1};
    auto isIpLiteral {res[hostStart] == '['};

    for (auto i {hostStart}; i < hostEnd; ++i) {
        if (res[i] == '%') {
            if (isIpLiteral) {
                break;
            }

            i += 2;
        } else if ((res[i] >= 'A') && (res[i] <= 'Z')) {
            res[i] = static_cast<char>(res[i] - 'A' + 'a');
        }
    }

    return res;
}

} // namespace

std::tuple<bool, std::string> retrieveFileInfo(const std::string &pFileNameOrUrl, bool pCanonicalise)
{
    // Check whether the given file name or URL is a local file name or a URL.
    // Note #1: a URL represents a local file when used with the "file" scheme.
    // Note #2: a scheme and a host are case-insensitive (e.g., "HTTPS://EXAMPLE.com/file.txt" and
    //          "https://example.com/file.txt" are the same file), so we normalise them to lowercase (see
    //          https://www.rfc-editor.org/rfc/rfc3986#section-6.2.2.1 and lowercaseHost()).
    // Note #3: the percent-encoded characters of a URL are decoded (e.g., "https://example.com/my%20file.txt" and
    //          "https://example.com/my file.txt" are the same file), except for those that have a meaning in a URL (see
    //          decodeUrl()). The path of a "file" URL is, however, always fully decoded since it is a local file name.
    //          On the other hand, a local file name is never decoded since a percent sign is a valid character in a
    //          file name (e.g., "/some/path/my%20file.txt" and "/some/path/my file.txt" are two different files).

#ifdef BUILDING_ON_WINDOWS
    static constexpr std::string_view FILE_SCHEME {"file:///"};
#else
    static constexpr std::string_view FILE_SCHEME {"file://"};
#endif
    static constexpr std::string_view HTTP_SCHEME {"http://"};
    static constexpr std::string_view HTTPS_SCHEME {"https://"};

    for (const auto &scheme : {HTTP_SCHEME, HTTPS_SCHEME}) {
        if (startsWithScheme(pFileNameOrUrl, scheme)) {
            auto res {lowercaseHost(decodeUrl(std::string(scheme) + pFileNameOrUrl.substr(scheme.size()), false), scheme.size())};

            return {false, pCanonicalise ? canonicalUrl(res) : res};
        }
    }

    auto res {startsWithScheme(pFileNameOrUrl, FILE_SCHEME) ?
                  decodeUrl(pFileNameOrUrl.substr(FILE_SCHEME.size())) :
                  pFileNameOrUrl};

    return {true, pCanonicalise ? canonicalFileName(res) : res};
}

namespace {

std::filesystem::path canonicalPath(const std::string &pPath)
{
    auto [isLocalPath, path] {retrieveFileInfo(pPath)};

    return stringToPath(path);
}

} // namespace

std::string relativePath(const std::string &pPath, const std::string &pBasePath)
{
    return pathToString(std::filesystem::relative(canonicalPath(pPath), canonicalPath(pBasePath)));
}

std::string urlPath(const std::string &pPath)
{
    auto [isLocalPath, path] {retrieveFileInfo(pPath)};

    if (isLocalPath && stringToPath(path).is_absolute()) {
        static const std::string FILE_SCHEME {"file://"};
#ifdef BUILDING_USING_MSVC
        static constexpr auto FORWARD_SLASH {"/"};

        return FILE_SCHEME + FORWARD_SLASH + forwardSlashPath(path);
#else
        return FILE_SCHEME + path;
#endif
    }

    return path;
}

bool hasScheme(const std::string &pUriReference)
{
    // Check whether the given URI reference has a scheme (e.g., "urn" in "urn:miriam:biomodels.db:BIOMD0000000012"),
    // i.e. whether it starts with a letter followed by any number of letters, digits, plus signs, hyphens, and periods,
    // and then a colon (see https://www.rfc-editor.org/rfc/rfc3986#section-3.1).
    // Note: a single-letter scheme is not considered to be a scheme since it is the drive letter of a Windows file name
    //       (e.g., "C:/some/path/model.cellml").

    static constexpr size_t MINIMUM_SCHEME_LENGTH {2};

    auto colonPos {pUriReference.find(':')};

    if ((colonPos == std::string::npos) || (colonPos < MINIMUM_SCHEME_LENGTH)
        || (std::isalpha(static_cast<unsigned char>(pUriReference[0])) == 0)) {
        return false;
    }

    return std::all_of(pUriReference.begin() + 1, pUriReference.begin() + static_cast<std::ptrdiff_t>(colonPos), [](char pChar) {
        return (std::isalnum(static_cast<unsigned char>(pChar)) != 0) || (pChar == '+') || (pChar == '-') || (pChar == '.');
    });
}

#ifndef __EMSCRIPTEN__
namespace {

using TimeVal = struct
{
    uint64_t seconds;
    uint64_t microeconds;
};

void getTimeOfDay(TimeVal &pTimeVal)
{
    // Based off https://stackoverflow.com/a/58162122.

    const auto duration {std::chrono::system_clock::now().time_since_epoch()};
    const auto seconds {std::chrono::duration_cast<std::chrono::seconds>(duration)};

    pTimeVal.seconds = static_cast<uint64_t>(seconds.count());
    pTimeVal.microeconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds).count());
}

std::filesystem::path uniqueFilePath()
{
    // This is based off glibc's __gen_tempname() method, which is used in std::tmpfile(). The only reason we don't use
    // std::tmpfile() is that it creates the file for us and as soon as we would be closing (though an automatic call
    // to std::fclose()), the file would get automatically deleted, which is not what we want. See
    // https://code.woboq.org/userspace/glibc/sysdeps/posix/tempname.c.html#__gen_tempname.

    // The number of times to attempt to generate a temporary file name.
    // Note: ATTEMPTS_MIN is equal to 62x62x62 where 62 is the number of characters in LETTERS.

#    ifndef CODE_COVERAGE_ENABLED
    static constexpr uint64_t ATTEMPTS_MIN {238328U};
    static constexpr uint64_t MAX_ATTEMPTS {(ATTEMPTS_MIN < TMP_MAX) ? TMP_MAX : ATTEMPTS_MIN};
#    endif

    // Get some more or less random data.

    static const std::string LETTERS {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    static const uint64_t LETTERS_SIZE {LETTERS.size()};

    // Note: our caller must ensure that only one thread at a time calls us and creates the file (see downloadFile()),
    //       so that two threads cannot end up with the same file path.

    static const auto TEST_FILE {pathToString(std::filesystem::temp_directory_path() / "libOpenCOR_XXXXXX.tmp")};

    static const size_t XXXXXX_POS {TEST_FILE.size() - 6 - 4};
    static constexpr uint64_t MICROSECONDS_SHIFT {16U};
    static constexpr uint64_t PID_SHIFT {32U};
#    ifndef CODE_COVERAGE_ENABLED
    static constexpr uint64_t VALUE_SHIFT {7777U};
#    endif
    static constexpr uint64_t XXXXXX_POS_SHIFT {6U};

    auto testFile {TEST_FILE};
    TimeVal timeVal;

    getTimeOfDay(timeVal);

    auto value {(timeVal.microeconds << MICROSECONDS_SHIFT) ^ timeVal.seconds};

#    ifdef BUILDING_USING_MSVC
    value ^= static_cast<uint64_t>(_getpid()) << PID_SHIFT;
#    else
    value ^= static_cast<uint64_t>(getpid()) << PID_SHIFT;
#    endif

    std::string res;

#    ifndef CODE_COVERAGE_ENABLED
    for (uint64_t attempt {0}; attempt < MAX_ATTEMPTS; value += VALUE_SHIFT, ++attempt) {
#    endif
        uint64_t val {value};

        for (uint64_t i {0}; i < XXXXXX_POS_SHIFT; ++i) {
            testFile[XXXXXX_POS + i] = LETTERS[val % LETTERS_SIZE];
            val /= LETTERS_SIZE;
        }

#    ifndef CODE_COVERAGE_ENABLED
        if (!std::filesystem::exists(testFile)) {
#    endif
            res = testFile;

#    ifndef CODE_COVERAGE_ENABLED
            break;
        }
    }
#    endif

    return res;
}

// Initialise libcurl when constructed and clean it up when destroyed (see downloadFile()).

class CurlGlobal
{
public:
    CurlGlobal()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlGlobal()
    {
        curl_global_cleanup();
    }

    CurlGlobal(const CurlGlobal &pOther) = delete;
    CurlGlobal(CurlGlobal &&pOther) noexcept = delete;

    CurlGlobal &operator=(const CurlGlobal &pRhs) = delete;
    CurlGlobal &operator=(CurlGlobal &&pRhs) noexcept = delete;
};

size_t curlWriteFunction(char *pData, size_t pSize, size_t pDataSize, void *pUserData)
{
    const auto realDataSize {pSize * pDataSize};

    static_cast<std::ofstream *>(pUserData)->write(pData, static_cast<std::streamsize>(realDataSize));

    return realDataSize;
}

} // namespace

std::tuple<bool, std::filesystem::path> downloadFile(const std::string &pUrl)
{
    static const std::tuple<bool, std::filesystem::path> NO_TUPLE {false, std::filesystem::path()};

    // Create a file with a unique file path.
    // Note: we must generate the file path and create the file in one go, so that another thread cannot generate the
    //       same file path in between (in which case both threads would download to the same file and the local copy
    //       of one remote file would get deleted with the other remote file).

    static std::mutex uniqueFilePathMutex;

    std::filesystem::path filePath;
    std::ofstream file;

    {
        const std::scoped_lock<std::mutex> lock(uniqueFilePathMutex);

        filePath = uniqueFilePath();

        file.open(filePath, std::ios_base::binary);
    }

#    ifndef CODE_COVERAGE_ENABLED
    if (!file.is_open()) {
        return NO_TUPLE;
    }
#    endif

    // Make sure that libcurl is initialised.
    // Note: libcurl must only be initialised once (and only be cleaned up once no thread uses it anymore), hence we
    //       initialise it the first time a file is downloaded (in a thread-safe way, this being a function-local static
    //       variable) and clean it up when libOpenCOR is unloaded. After that, several threads can download a file at
    //       the same time since each of them uses its own libcurl handle and our OpenSSL is thread-safe.

    static const CurlGlobal curlGlobal;

    auto res {false};
    auto *curl {curl_easy_init()};

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_URL, encodeUrl(pUrl).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&file));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);

    if (curl_easy_perform(curl) == CURLE_OK) {
        static constexpr int64_t HTTP_OK {200};

        int64_t responseCode {0};

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        res = responseCode == HTTP_OK;
    }

    curl_easy_cleanup(curl);

    file.close();

    if (res) {
        return {true, filePath};
    }

    std::error_code errorCode;

    std::filesystem::remove(filePath, errorCode);

    return NO_TUPLE;
}

UnsignedChars fileContents(const std::filesystem::path &pFilePath)
{
    static const UnsignedChars NO_UNSIGNED_CHARS;

    // Retrieve and return the contents of the given file.
    // Note: a file path with a null character (e.g., "/some/path/file.txt\0.cellml") cannot exist, so we don't try to
    //       open it (std::ifstream would otherwise only consider the file path up to that null character, i.e.
    //       "/some/path/file.txt").

    if (pFilePath.native().find(std::filesystem::path::value_type {}) != std::filesystem::path::string_type::npos) {
        return NO_UNSIGNED_CHARS;
    }

    std::ifstream file(pFilePath, std::ios_base::binary);

    if (!file.is_open()) {
        return NO_UNSIGNED_CHARS;
    }

    // Note: we use the std::error_code version of std::filesystem::file_size() so that nothing gets thrown (e.g., if
    //       the file is actually a directory, which can be opened on some platforms).

    std::error_code errorCode;
    const auto fileSize {std::filesystem::file_size(pFilePath, errorCode)};

    if (errorCode) {
        return NO_UNSIGNED_CHARS;
    }

    UnsignedChars contents;

    contents.resize(fileSize);

    file.read(reinterpret_cast<char *>(contents.data()), static_cast<std::streamsize>(fileSize)); // NOLINT

    return contents;
}
#endif

bool isInfOrNan(double pNumber)
{
    return std::isinf(pNumber) || std::isnan(pNumber);
}

bool toBool(const std::string &pString)
{
    return pString == "true";
}

std::string toString(bool pBoolean)
{
    return pBoolean ? "true" : "false";
}

bool isInt(const std::string &pString)
{
    static const auto INT_REGEX {std::regex("^([-+]?[1-9][0-9]*([eE][+]?[0-9]+)?|0)$")};

    if (std::regex_match(pString, INT_REGEX)) {
        std::istringstream iss(pString);
        int res {0};

        iss >> res;

        return !iss.fail();
    }

    return false;
}

int toInt(const std::string &pString)
{
    std::istringstream iss(pString);
    int res {0};

    iss >> res;

    return res;
}

std::string toString(int pNumber)
{
    return std::format("{}", pNumber);
}

std::string toString(size_t pNumber)
{
    return std::format("{}", pNumber);
}

bool isDouble(const std::string &pString)
{
    static const auto DOUBLE_REGEX {std::regex("^[+-]?[0-9]*\\.?[0-9]+([eE][+-]?[0-9]+)?$")};

    return std::regex_match(pString, DOUBLE_REGEX);
}

double toDouble(const std::string &pString)
{
    if (!isDouble(pString)) {
        return NAN;
    }

    std::istringstream iss(pString);
    double res {NAN};

    iss >> res;

    return iss.fail() ? NAN : res;
}

std::string toString(double pNumber)
{
    std::ostringstream res;

    res << std::setprecision(std::numeric_limits<double>::digits10) << pNumber;

    return res.str();
}

std::string toString(const UnsignedChars &pBytes)
{
    return {reinterpret_cast<const char *>(pBytes.data()), pBytes.size()};
}

const xmlChar *toConstXmlCharPtr(const std::string &pString)
{
    return reinterpret_cast<const xmlChar *>(pString.c_str());
}

bool isSedmlId(const std::string &pString)
{
    // Check whether the given string is a valid SED-ML identifier, i.e. an SId, which starts with a letter or an
    // underscore, followed by any number of letters, digits, and underscores (see
    // https://sed-ml.org/specifications.html).

    static const auto SEDML_ID_REGEX {std::regex("^[A-Za-z_][A-Za-z0-9_]*$")};

    return std::regex_match(pString, SEDML_ID_REGEX);
}

bool isCellmlIdentifier(const std::string &pString)
{
    // Check whether the given string is a valid CellML identifier, i.e. it consists only of letters, digits, and
    // underscores, contains at least one letter, and doesn't start with a digit (see
    // https://www.cellml.org/specifications/cellml_2.0).

    static const auto CELLML_IDENTIFIER_REGEX {std::regex("^(?=[A-Za-z0-9_]*[A-Za-z])[A-Za-z_][A-Za-z0-9_]*$")};

    return std::regex_match(pString, CELLML_IDENTIFIER_REGEX);
}

libcellml::ComponentPtr owningComponent(const libcellml::VariablePtr &pVariable)
{
    return std::dynamic_pointer_cast<libcellml::Component>(pVariable->parent());
}

std::string name(const std::string &pComponentName, const std::string &pVariableName)
{
    return pComponentName + "/" + pVariableName;
}

std::string name(const libcellml::VariablePtr &pVariable)
{
    return name(owningComponent(pVariable)->name(), pVariable->name());
}

} // namespace libOpenCOR

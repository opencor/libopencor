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

#pragma once

#include <emscripten/bind.h>

#include <string>
#include <string_view>

// Conversion between a std::string and a JavaScript string, to be used instead of Embind's conversion for all our
// std::string values since a std::string may not be valid UTF-8 (e.g., the URL "https://example.com/%FF" gets
// percent-decoded to "https://example.com/\xFF"), in which case Embind would replace an invalid byte with U+FFFD and
// the original std::string couldn't be retrieved. So, we map an invalid byte to a lone surrogate (e.g., 0xFF to U+DCFF)
// and back, meaning that a string can always be retrieved from libOpenCOR and given back to it without being altered.
// We do, however, reject a string with any other lone surrogate (e.g., U+D800) since replacing it with U+FFFD would
// silently alter the string (e.g., a file name or URL would then refer to another file).
// Note #1: JsString must be registered as a "string" (see main.cpp), so that it is a string in our TypeScript
//          declarations.
// Note #2: the code within the EM_ASM() blocks is such that it doesn't rely on top-level commas since those can get
//          interpreted as macro argument separators by the C++ preprocessor (see main.cpp).
// Note #3: we build our JavaScript bindings with shared memory support (see CMakeLists.txt), meaning that HEAPU8 is
//          backed by a SharedArrayBuffer, which TextDecoder.decode() doesn't accept in a browser. So, we must copy the
//          bytes of a std::string (using slice() rather than subarray()) before decoding them.
// Note #4: a string that is rejected (e.g., because it has a lone surrogate) results in a JavaScript exception, which
//          we throw using emscripten::val::throw_() rather than from within an EM_ASM() block since the latter is
//          declared as not throwing. Also, a JavaScript exception that leaves WebAssembly doesn't restore the
//          WebAssembly stack pointer, meaning that some of the WebAssembly stack would be leaked every time a binding
//          throws. So, every binding that may throw (i.e. that uses fromJsString(), throwError(), or throwRangeError())
//          must be listed in restoreStackOnThrow() (see main.cpp).

EMSCRIPTEN_DECLARE_VAL_TYPE(JsString);

inline JsString toJsString(const std::string &pString)
{
    // clang-format off
    return JsString(emscripten::val::take_ownership(static_cast<emscripten::EM_VAL>(EM_ASM_PTR({
        let bytes = HEAPU8.slice($0, $0 + $1);

        try {
            return Emval.toHandle(new TextDecoder('utf-8', { fatal: true, ignoreBOM: true }).decode(bytes));
        } catch (e) {
        }

        let res = "";
        let i = 0;

        while (i < bytes.length) {
            let byte = bytes[i];
            let length = 0;
            let low = 0x80;
            let high = 0xBF;

            if (byte < 0x80) {
                res += String.fromCharCode(byte);

                ++i;

                continue;
            }

            if ((byte >= 0xC2) && (byte <= 0xDF)) {
                length = 1;
            } else if ((byte >= 0xE0) && (byte <= 0xEF)) {
                length = 2;
                low = (byte == 0xE0) ? 0xA0 : 0x80;
                high = (byte == 0xED) ? 0x9F : 0xBF;
            } else if ((byte >= 0xF0) && (byte <= 0xF4)) {
                length = 3;
                low = (byte == 0xF0) ? 0x90 : 0x80;
                high = (byte == 0xF4) ? 0x8F : 0xBF;
            }

            let codePoint = byte & (0x3F >> length);
            let j = 1;

            while ((j <= length) && (i + j < bytes.length)
                   && (bytes[i + j] >= ((j == 1) ? low : 0x80)) && (bytes[i + j] <= ((j == 1) ? high : 0xBF))) {
                codePoint = (codePoint << 6) | (bytes[i + j] & 0x3F);

                ++j;
            }

            if ((length > 0) && (j > length)) {
                res += String.fromCodePoint(codePoint);

                i += j;
            } else {
                res += String.fromCharCode(0xDC00 + byte);

                ++i;
            }
        }

        return Emval.toHandle(res);
    }, pString.data(), pString.size()))));
    // clang-format on
}

inline std::string fromJsString(const JsString &pString)
{
    // clang-format off
    auto bytes {emscripten::val::take_ownership(static_cast<emscripten::EM_VAL>(EM_ASM_PTR({
        let str = Emval.toValue($0);

        if (typeof str !== 'string') {
            return Emval.toHandle(new TypeError('Cannot pass non-string to std::string'));
        }

        let encoder = new TextEncoder();
        let chunks = [];
        let start = 0;

        for (let i = 0; i < str.length; ++i) {
            let codeUnit = str.charCodeAt(i);

            if ((codeUnit < 0xD800) || (codeUnit > 0xDFFF)) {
                continue;
            }

            if (codeUnit <= 0xDBFF) {
                let nextCodeUnit = (i + 1 < str.length) ? str.charCodeAt(i + 1) : 0;

                if ((nextCodeUnit >= 0xDC00) && (nextCodeUnit <= 0xDFFF)) {
                    ++i;

                    continue;
                }
            } else if ((codeUnit >= 0xDC80) && (codeUnit <= 0xDCFF)) {
                chunks.push(encoder.encode(str.substring(start, i)));
                chunks.push(Uint8Array.of(codeUnit - 0xDC00));

                start = i + 1;

                continue;
            }

            return Emval.toHandle(new TypeError('Cannot pass a string with a lone surrogate (other than U+DC80 to U+DCFF) to std::string'));
        }

        if (start == 0) {
            return Emval.toHandle(encoder.encode(str));
        }

        chunks.push(encoder.encode(str.substring(start)));

        let res = new Uint8Array(chunks.reduce((size, chunk) => size + chunk.length, 0));
        let offset = 0;

        for (let chunk of chunks) {
            res.set(chunk, offset);

            offset += chunk.length;
        }

        return Emval.toHandle(res);
    }, pString.as_handle())))};

    // Note: the above EM_ASM() block returns rather than throws a TypeError if the string is rejected (see Note #4).

    if (bytes.instanceof(emscripten::val::global("TypeError"))) {
        bytes.throw_();
    }

    auto size {bytes["length"].as<size_t>()};
    std::string res(size, '\0');

    if (size > 0) {
        EM_ASM({
            HEAPU8.set(Emval.toValue($0), $1);
        }, bytes.as_handle(), res.data());
    }

    return res;
    // clang-format on
}

[[noreturn]] inline void throwError(std::string_view pMessage)
{
    // Throw a JavaScript Error with the given message (see Note #4), e.g., when something cannot be done in the current
    // JavaScript environment.

    emscripten::val::global("Error").new_(std::string(pMessage)).throw_();
}

[[noreturn]] inline void throwRangeError(std::string_view pMessage)
{
    // Throw a JavaScript RangeError with the given message (see Note #4), e.g., when a value is rejected by libOpenCOR.

    emscripten::val::global("RangeError").new_(std::string(pMessage)).throw_();
}

// Wrappers for a method or a function that returns a std::string (e.g., libOpenCOR::Issue::description()) and for a
// method that takes a std::string and returns whether it accepted it (e.g., libOpenCOR::SedBase::setId()), so that they
// can be bound using a JsString rather than a std::string, e.g.:
//     .property("id", &JsStringResult<&libOpenCOR::SedBase::id>::call, &JsStringArgument<&libOpenCOR::SedBase::setId, ID_ERROR>::call)
// Note: a property setter cannot return whether the value was accepted, so we throw a RangeError with the given error
//       message if it wasn't.

template<auto Method, typename = decltype(Method)>
struct JsStringResult;

template<auto Method, typename Result, typename Class, typename... Args, bool IsNoexcept>
struct JsStringResult<Method, Result (Class::*)(Args...) const noexcept(IsNoexcept)>
{
    static JsString call(const Class &pThis, Args... pArgs)
    {
        return toJsString((pThis.*Method)(pArgs...));
    }
};

template<auto Function, typename Result, typename... Args, bool IsNoexcept>
struct JsStringResult<Function, Result (*)(Args...) noexcept(IsNoexcept)>
{
    static JsString call(Args... pArgs)
    {
        return toJsString(Function(pArgs...));
    }
};

template<auto Method, const std::string_view &ErrorMessage, typename = decltype(Method)>
struct JsStringArgument;

template<auto Method, const std::string_view &ErrorMessage, typename Class, bool IsNoexcept>
struct JsStringArgument<Method, ErrorMessage, bool (Class::*)(const std::string &) noexcept(IsNoexcept)>
{
    static void call(Class &pThis, const JsString &pValue)
    {
        // Note: the std::string returned by fromJsString() is destroyed before we (may) throw a RangeError.

        if (!(pThis.*Method)(fromJsString(pValue))) {
            throwRangeError(ErrorMessage);
        }
    }
};

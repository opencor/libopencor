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

#include <nanobind/nanobind.h>

#include <string>

// Type caster for std::string, to be used instead of the one in <nanobind/stl/string.h> (which is why it must be
// included by all our Python bindings source files that need to convert an std::string).
// Note: a std::string may not be valid UTF-8 (e.g., the URL "https://example.com/%FF" gets percent-decoded to
//       "https://example.com/\xFF"), in which case nanobind's type caster would raise a UnicodeDecodeError. So, like
//       Python does for file names (see os.fsdecode() and os.fsencode()), we use the "surrogateescape" error handler
//       to map an invalid byte to a lone surrogate (e.g., 0xFF to U+DCFF) and back, meaning that a string can always be
//       retrieved from libOpenCOR and given back to it without being altered. A string with any other lone surrogate
//       (e.g., U+D800) cannot be encoded, so it is rejected (i.e. a TypeError is raised) rather than silently altered.

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template<>
struct type_caster<std::string>
{
    NB_TYPE_CASTER(std::string, const_name("str"))

    bool from_python(handle pSrc, uint32_t, cleanup_list *) noexcept
    {
        if (!str_check(pSrc.ptr())) {
            return false;
        }

        // Use the cached UTF-8 representation of the string, if possible, i.e. if the string doesn't contain any lone
        // surrogate.

        Py_ssize_t size;
        const char *str {PyUnicode_AsUTF8AndSize(pSrc.ptr(), &size)};

        if (str != nullptr) {
            value = std::string(str, static_cast<size_t>(size));

            return true;
        }

        PyErr_Clear();

        // Map the lone surrogates back to the bytes they stand for.

        auto *bytes {PyUnicode_AsEncodedString(pSrc.ptr(), "utf-8", "surrogateescape")};

        if (bytes == nullptr) {
            PyErr_Clear();

            return false;
        }

        char *bytesStr {nullptr};
        Py_ssize_t bytesSize {0};
        auto res {PyBytes_AsStringAndSize(bytes, &bytesStr, &bytesSize) == 0};

        if (res) {
            value = std::string(bytesStr, static_cast<size_t>(bytesSize));
        } else {
            PyErr_Clear();
        }

        Py_DECREF(bytes);

        return res;
    }

    static handle from_cpp(const std::string &pValue, rv_policy, cleanup_list *) noexcept
    {
        return PyUnicode_DecodeUTF8(pValue.c_str(), static_cast<Py_ssize_t>(pValue.size()), "surrogateescape");
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)

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

#include "libopencor/logger.h"

#include <string>
#include <vector>

namespace libOpenCOR {

/**
 * @brief The File class.
 *
 * The File class is used to represent a local or a remote file, either on disk or in memory.
 */

class LIBOPENCOR_EXPORT File: public Logger
{
public:
    /**
     * @brief The type of a file.
     *
     * The type of a file, i.e. whether it is unknown, a CellML file, a SED-ML file, a COMBINE archive, or
     * irretrievable.
     */

    enum class Type
    {
        UNKNOWN_FILE, /**< The type of the file is unknown. */
        CELLML_FILE, /**< The file is a CellML file. */
        SEDML_FILE, /**< The file is a SED-ML file. */
#ifdef __EMSCRIPTEN__
        COMBINE_ARCHIVE /**< The file is a COMBINE archive. */
#else
        COMBINE_ARCHIVE, /**< The file is a COMBINE archive. */
        IRRETRIEVABLE_FILE /**< The file is irretrievable. */
#endif
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    File() = delete; /**< No default constructor allowed, @private. */
    ~File(); /**< Destructor, @private. */

    File(const File &pOther) = delete; /**< No copy constructor allowed, @private. */
    File(File &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    File &operator=(const File &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    File &operator=(File &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

#ifndef __EMSCRIPTEN__
    /**
     * @brief Create a @ref File object.
     *
     * Factory method to create a @ref File object:
     *
     * ```
     * auto localFile = libOpenCOR::File::create("/some/path/file.txt");
     * auto remoteFile = libOpenCOR::File::create("https://some.domain.com/file.txt");
     * ```
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     *
     * @return A smart pointer to a @ref File object.
     */

    static FilePtr create(const std::string &pFileNameOrUrl);
#endif

    /**
     * @brief Create a @ref File object.
     *
     * Factory method to create a @ref File object with some contents:
     *
     * ```
     * auto localVirtualFile = libOpenCOR::File::create("/some/path/file.txt", someContents);
     * auto remoteVirtualFile = libOpenCOR::File::create("https://some.domain.com/file.txt", someContents);
     * ```
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     * @param pContents The contents of the file as a @c std::vector of unsigned @c char.
     *
     * @return A smart pointer to a @ref File object.
     */

    static FilePtr create(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents);

    /**
     * @brief Get the type of this file.
     *
     * Return the type of this file.
     *
     * @return The type as a @ref Type.
     */

    Type type() const;

    /**
     * @brief Get the file name of this file.
     *
     * Return the file name of this file. If the file is remote then we return the file name of its local copy.
     *
     * @return The file name, as a @c std::string, of this file.
     */

    std::string fileName() const;

    /**
     * @brief Get the URL of this file.
     *
     * Return the URL of this file. If the file is local then we return an empty string.
     *
     * @return The URL, as a @c std::string, of this file.
     */

    std::string url() const;

    /**
     * @brief Get the contents of this file.
     *
     * Return the contents of this file.
     *
     * @return The contents, as an array of @c char, of this file.
     */

#ifdef __EMSCRIPTEN__
    emscripten::val jsContents();
#endif

    std::vector<unsigned char> contents();

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit File(const std::string &pFileNameOrUrl, const std::vector<unsigned char> &pContents = {}); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

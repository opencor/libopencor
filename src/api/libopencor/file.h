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

namespace libOpenCOR {

/**
 * @brief The File class.
 *
 * The File class is used to represent a local or a remote file, either on disk or in memory.
 */

class LIBOPENCOR_EXPORT File: public Logger
    , public std::enable_shared_from_this<File>
{
    friend class SedDocument;
    friend class SedInstanceTask;
    friend class SedModel;
    friend class SedSimulation;

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
    ~File() override; /**< Destructor, @private. */

    File(const File &pOther) = delete; /**< No copy constructor allowed, @private. */
    File(File &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    File &operator=(const File &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    File &operator=(File &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

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
     * Note: if there is already a @ref File object for the given file name or URL then we return it after having reset
     *       its contents and type.
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     * @param pRetrieveContents Whether the contents of the file should be retrieved.
     *
     * @return A smart pointer to a @ref File object.
     */

#ifdef __EMSCRIPTEN__
    static FilePtr create(const std::string &pFileNameOrUrl);
#else
    static FilePtr create(const std::string &pFileNameOrUrl, bool pRetrieveContents = true);
#endif

    /**
     * @brief Get the type of this file.
     *
     * Return the type of this file.
     *
     * @return The type, as a @ref Type, of this file.
     */

    Type type() const;

    /**
     * @brief Get the file name of this file.
     *
     * Return the file name of this file. If the file is remote then we return the file name of its local copy.
     *
     * @sa url()
     * @sa path()
     *
     * @return The file name, as a @c std::string, of this file.
     */

    std::string fileName() const;

    /**
     * @brief Get the URL of this file.
     *
     * Return the URL of this file. If the file is local then we return an empty string.
     *
     * @sa fileName()
     * @sa path()
     *
     * @return The URL, as a @c std::string, of this file.
     */

    std::string url() const;

    /**
     * @brief Get the path of this file.
     *
     * Return the path of this file. If the file is local then we return its file name otherwise its URL.
     *
     * @sa fileName()
     * @sa url()
     *
     * @return The path, as a @c std::string, of this file.
     */

    std::string path() const;

    /**
     * @brief Get the contents of this file.
     *
     * Return the contents of this file.
     *
     * @return The contents, as an @ref UnsignedChars, of this file.
     */

    UnsignedChars contents();

    /**
     * @brief Set the contents of this file.
     *
     * Set the contents of this file.
     *
     * @param pContents The contents, as an @ref UnsignedChars, of this file.
     */

    void setContents(const UnsignedChars &pContents);

    /**
     * @brief Return whether this file has child files.
     *
     * Return whether this file has child files. This method is only relevant for COMBINE archives.
     *
     * @return @c true if this file has child files, @c false otherwise.
     */

    bool hasChildFiles() const;

    /**
     * @brief Return the number of child files.
     *
     * Return the number of child files. This method is only relevant for COMBINE archives.
     *
     * @return The number of child files.
     */

    size_t childFileCount() const;

    /**
     * @brief Return the child file names.
     *
     * Return the child file names. This method is only relevant for COMBINE archives.
     *
     * @return The child file names, as a @ref Strings.
     */

    Strings childFileNames() const;

    /**
     * @brief Return the child files.
     *
     * Return the child files. This method is only relevant for COMBINE archives.
     *
     * @return The child files, as a @ref FilePtrs.
     */

    FilePtrs childFiles() const;

    /**
     * @brief Return a child file.
     *
     * Return a child file. This method is only relevant for COMBINE archives.
     *
     * @param pIndex The index of the child file.
     *
     * @return The file, as a @ref FilePtr, if it is a child file of this file, @c nullptr otherwise.
     */

    FilePtr childFile(size_t pIndex) const;

    /**
     * @brief Return a child file.
     *
     * Return a child file. This method is only relevant for COMBINE archives.
     *
     * @param pFileName The name of the child file.
     *
     * @return The file, as a @ref FilePtr, if it is a child file of this file, @c nullptr otherwise.
     */

#ifdef __EMSCRIPTEN__
    FilePtr childFileFromFileName(const std::string &pFileName) const;
#else
    FilePtr childFile(const std::string &pFileName) const;
#endif

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit File(const std::string &pFileNameOrUrl, bool pRetrieveContents); /**< Constructor, @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

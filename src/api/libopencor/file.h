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

#include "libopencor/export.h"
#include "libopencor/types.h"

#include <string>

namespace libOpenCOR {

/**
 * @brief The File class.
 *
 * The File class is for representing a local or a remote file.
 */

class LIBOPENCOR_EXPORT File
{
public:
    /**
     * @brief The type of a file.
     *
     * A file can be of one of the following types:
     *  - UNDEFINED: the file has yet to be resolved;
     *  - CELLML_FILE: the file is a CellML file;
     *  - SEDML_FILE: the file is a SED-ML file;
     *  - COMBINE_ARCHIVE: the file is a COMBINE archive; or
     *  - UNKNOWN: the type of the file is unknown.
     *
     * @sa resolve
     */

    enum class Type
    {
        UNDEFINED,
        CELLML_FILE,
        SEDML_FILE,
        COMBINE_ARCHIVE,
        UNKNOWN_FILE
    };

    /**
     * Constructors, destructor and assignment operators.
     */

    File() = delete; /**< No default constructor allowed, @private. */
    ~File() = default; /**< Default destructor, @private. */

    File(const File &pOther) = delete; /**< No copy constructor allowed, @private. */
    File(File &&) noexcept = delete; /**< No move constructor allowed, @private. */

    File &operator=(const File &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    File &operator=(File &&) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @c File object.
     *
     * Factory method to create a @c File:
     *
     * ```
     * auto localFile = libOpenCOR::File::create("/some/path/file.txt");
     * auto remoteFile = libOpenCOR::File::create("https://domain.com/file.txt");
     * ```
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     *
     * @return A smart pointer to a @c File object.
     */

    static FilePtr create(const std::string &pFileNameOrUrl);

    /**
     * @brief Get the type of this @c File.
     *
     * Return the type of this @c File.
     *
     * @return The type as a @c Type.
     */

    Type type() const;

    /**
     * @brief Get the file name for this @c File.
     *
     * Return the file name for this @c File. If @c File is remote then we return
     * the file name for its local copy.
     *
     * @return The file name of this @c File as a @c std::string.
     */

    std::string fileName() const;

    /**
     * @brief Get the URL for this @c File.
     *
     * Return the URL for this @c File. If @c File is remote then we return an
     * empty string.
     *
     * @return The URL of this @c File as a @c std::string.
     */

    std::string url() const;

    /**
     * @brief Resolve this @c File.
     *
     * Retrieve the contents of this @c File and determine its type, i.e. a
     * CellML file, a SED-ML file, a COMBINE archive, or an unknown file.
     */

    void resolve();

private:
    explicit File(const std::string &pFileNameOrUrl); /**< Constructor @private*/

    struct Impl; /**< Forward declaration of the implementation class, @private. */

    std::unique_ptr<Impl> mPimpl;
};

} // namespace libOpenCOR

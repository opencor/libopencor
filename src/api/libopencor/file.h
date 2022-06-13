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
     *  - LOCAL: a file that is locally stored; or
     *  - REMOTE: a profile that is remotely stored.
     */

    enum class Type
    {
        LOCAL,
        REMOTE
    };

    /**
     * Constructors and destructor.
     */

    File(const File &pRhs) = delete;
    File(File &&pRhs) noexcept = delete;
    File &operator=(File pRhs) = delete;
    ~File();

    /**
     * @brief Create a @c File object.
     *
     * Factory method to create a @c File:
     *
     * ```
     * FilePtr localFile = libOpenCOR::File::create("/some/path/file.txt");
     * FilePtr remoteFile = libOpenCOR::File::create("https://domain.com/file.txt");
     * ```
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     *
     * @return A smart pointer to a @c File object.
     */

    static FilePtr create(const std::string &pFileNameOrUrl);

    /**
     * @brief Get the file name for this @c File.
     *
     * Return the file name for this @c File. If @c File is remote then we return
     * the file name for its local copy.
     *
     * @return The file name as a @c std::string.
     */

    std::string fileName() const;

    /**
     * @brief Get the URL for this @c File.
     *
     * Return the URL for this @c File. If @c File is remote then we return an
     * empty string.
     *
     * @return The URL as a @c std::string.
     */

    std::string url() const;

    /**
     * @brief Get the type for this @c File.
     *
     * Return the type for this @c File.
     *
     * @return The type as a @c Type.
     */

    Type type() const;

private:
    explicit File(const std::string &pFileNameOrUrl);

    struct FileImpl;
    FileImpl *mPimpl;
};

} // namespace libOpenCOR

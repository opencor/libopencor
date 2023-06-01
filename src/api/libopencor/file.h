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
     * The type of a file, i.e. whether it is unresolved, a CellML file, a SED-ML file, a COMBINE archive, or unknown.
     *
     * @sa resolve()
     */

    enum class Type
    {
        UNRESOLVED, /**< The file has yet to be resolved. */
        CELLML_FILE, /**< The file is a CellML file. */
        SEDML_FILE, /**< The file is a SED-ML file. */
        COMBINE_ARCHIVE, /**< The file is a COMBINE archive. */
        UNKNOWN_FILE /**< The type of the file is unknown. */
    };

    /**
     * @brief The status of an action on a file.
     *
     * The status of an action on a file, i.e. whether it was successful, unable to retrieve the contents of a file, or
     * unable to instantiate a file.
     */

    enum class Status
    {
        OK, /**< The action completed successfully. */
        NON_RETRIEVABLE_FILE, /**< The action could not retrieve the contents of a file. */
        NON_INSTANTIABLE_FILE /**< The action could not instantiate a file. */
    };

    /**
     * Constructors, destructor, and assignment operators.
     */

    File() = delete; /**< No default constructor allowed, @private. */
    ~File(); /**< Destructor, @private. */

    File(const File &pOther) = delete; /**< No copy constructor allowed, @private. */
    File(File &&) noexcept = delete; /**< No move constructor allowed, @private. */

    File &operator=(const File &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    File &operator=(File &&) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref File object.
     *
     * Factory method to create a @ref File object:
     *
     * ```
     * auto localFile = libOpenCOR::File::create("/some/path/file.txt");
     * auto remoteFile = libOpenCOR::File::create("https://domain.com/file.txt");
     * ```
     *
     * @param pFileNameOrUrl The @c std::string file name or URL.
     *
     * @return A smart pointer to a @ref File object.
     */

    static FilePtr create(const std::string &pFileNameOrUrl);

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
     * @brief Resolve this @ref File.
     *
     * Retrieve the contents of this @ref File and determine its @ref Type, i.e. a CellML file, a SED-ML file, a COMBINE
     * archive, or an unknown file.
     *
     * @sa Status, type()
     *
     * @return The status as a @ref Status.
     */

    Status resolve();

    /**
     * @brief Instantiate this @ref File.
     *
     * Instantiate this @ref File and generate the runtime associated with the CellML file that is associated with this
     * @ref File, be it directly or indirectly.
     *
     * @sa Status, runtime()
     *
     * @return The status as a @ref Status.
     */

    Status instantiate();

private:
    explicit File(const std::string &pFileNameOrUrl); /**< Constructor @private*/

    struct Impl; /**< Forward declaration of the implementation class, @private. */

    Impl *mPimpl;
};

} // namespace libOpenCOR

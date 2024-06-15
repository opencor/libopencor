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

#include "libopencor/file.h"

#include <vector>

namespace libOpenCOR {

/**
 * @brief The FileManager class.
 *
 * The FileManager class is used to keep track of files.
 */

class LIBOPENCOR_EXPORT FileManager
{
    friend class File;

public:
    /**
     * @brief Get the file manager instance.
     *
     * Get the file manager instance.
     *
     * @return The file manager instance, as a reference to a @ref FileManager object.
     */

    static FileManager &instance();

    /**
     * @brief Return whether there are some files.
     *
     * Return whether there are some files.
     *
     * @return @c true if there are some files, @c false otherwise.
     */

    bool hasFiles() const;

    /**
     * @brief Return the files.
     *
     * Return the files.
     *
     * @return The files, as an @ref FilePtrs.
     */

    FilePtrs files() const;

    /**
     * @brief Get a given file.
     *
     * Get a given file.
     *
     * @param pFileNameOrUrl The name of the file or its URL.
     *
     * @return The file, as a @ref FilePtr, if it is managed by the file manager, @c nullptr otherwise.
     */

    FilePtr file(const std::string &pFileNameOrUrl) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl &mPimpl; /**< Private implementation reference, @private. */

    explicit FileManager(); /**< Constructor, @private. */
};

} // namespace libOpenCOR
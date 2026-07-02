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
     * @brief Return the file manager instance.
     *
     * Return the file manager instance.
     *
     * @return The file manager instance, as a reference to a @ref FileManager object.
     */

    static FileManager &instance();

    /**
     * @brief Manage the given file.
     *
     * Manage the given file.
     *
     * @param pFile The file to be managed.
     *
     * @sa unmanage()
     */

    void manage(const FilePtr &pFile);

    /**
     * @brief Unmanage the given file.
     *
     * Unmanage the given file.
     *
     * @param pFile The file to be unmanaged.
     *
     * @sa manage()
     */

    void unmanage(const FilePtr &pFile);

    /**
     * @brief Reset the file manager.
     *
     * Reset the file manager by unmanaging all the files.
     *
     * @sa unmanage()
     */

    void reset();

    /**
     * @brief Return whether there are some managed files.
     *
     * Return whether there are some managed files.
     *
     * @return @c true if there are some managed files, @c false otherwise.
     */

    bool hasFiles() const;

    /**
     * @brief Return the number of managed files.
     *
     * Return the number of managed files.
     *
     * @return The number of managed files.
     */

    size_t fileCount() const;

    /**
     * @brief Return the managed files.
     *
     * Return the managed files.
     *
     * @return The managed files, as an @ref FilePtrs.
     */

    FilePtrs files() const;

    /**
     * @brief Return the managed file at the given index.
     *
     * Return the managed file at the given index.
     *
     * @param pIndex The index of the managed file.
     *
     * @return The managed file, as a @ref FilePtr, if it is managed by the file manager, @c nullptr otherwise.
     */

    FilePtr file(size_t pIndex) const;

    /**
     * @brief Return the managed file with the given name or URL.
     *
     * Return the managed file with the given name or URL.
     *
     * @param pFileNameOrUrl The name of the managed file or its URL.
     *
     * @return The managed file, as a @ref FilePtr, if it is managed by the file manager, @c nullptr otherwise.
     */

#ifdef __EMSCRIPTEN__
    FilePtr fileFromFileNameOrUrl(const std::string &pFileNameOrUrl) const;
#else
    FilePtr file(const std::string &pFileNameOrUrl) const;
#endif

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    Impl &mPimpl; /**< Private implementation reference, @private. */

    explicit FileManager(); /**< Constructor, @private. */
};

} // namespace libOpenCOR

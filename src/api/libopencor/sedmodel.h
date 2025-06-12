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

#include "libopencor/sedbase.h"

namespace libOpenCOR {

/**
 * @brief The SedModel class.
 *
 * The SedModel class is used to describe a model in the context of a simulation experiment description.
 */

class LIBOPENCOR_EXPORT SedModel: public SedBase
{
    friend class SedDocument;
    friend class SedInstanceTask;
    friend class SedSimulation;
    friend class SedTask;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    SedModel() = delete; /**< No default constructor allowed, @private. */
    ~SedModel() override; /**< Destructor, @private. */

    SedModel(const SedModel &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedModel(SedModel &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedModel &operator=(const SedModel &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedModel &operator=(SedModel &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedModel object.
     *
     * Factory method to create a @ref SedModel object:
     *
     * ```
     * auto document = libOpenCOR::SedDocument::create();
     * auto file = libOpenCOR::File::create("/some/path/file.txt");
     * auto model = libOpenCOR::SedModel::create(document, file);
     * ```
     *
     * @param pDocument The @ref SedDocument object to which the @ref SedModel object is to belong.
     * @param pFile The @ref File object for which to create the @ref SedModel object.
     *
     * @return A smart pointer to a @ref SedModel object.
     */

    static SedModelPtr create(const SedDocumentPtr &pDocument, const FilePtr &pFile);

    /**
     * @brief Get the file.
     *
     * Get the file for the @ref SedModel object.
     *
     * @return The file, as a smart pointer to a @ref File object.
     */

    FilePtr file() const;

    /**
     * @brief Return whether there are some changes.
     *
     * Return whether there are some changes.
     *
     * @return @c true if there are some changes, @c false otherwise.
     */

    bool hasChanges() const;

    /**
     * @brief Return the number of changes.
     *
     * Return the number of changes.
     *
     * @return The number of changes.
     */

    size_t changeCount() const;

    /**
     * @brief Return the changes.
     *
     * Return the changes.
     *
     * @return The changes, as a @ref SedChangePtrs.
     */

    SedChangePtrs changes() const;

    /**
     * @brief Return the change at the given index.
     *
     * Return the change at the given index.
     *
     * @param pIndex The index of the change to return.
     *
     * @return The change as a @ref SedChangePtr, if the index is valid, @c nullptr otherwise.
     */

    SedChangePtr change(size_t pIndex) const;

    /**
     * @brief Add the change to this model.
     *
     * Add the change to this model.
     *
     * @param pChange The @ref SedChange object to be added.
     *
     * @return @c true if the change was added, @c false otherwise.
     */

    bool addChange(const SedChangePtr &pChange);

    /**
     * @brief Remove the change from this model.
     *
     * Remove the change from this model.
     *
     * @param pChange The @ref SedChange object to be removed.
     *
     * @return @c true if the change was removed, @c false otherwise.
     */

    bool removeChange(const SedChangePtr &pChange);

    /**
     * @brief Remove all changes from this model.
     *
     * Remove all changes from this model.
     *
     * @return @c true if all changes were removed, @c false otherwise.
     */
    bool removeAllChanges();

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedModel(const SedDocumentPtr &pDocument, const FilePtr &pFile); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

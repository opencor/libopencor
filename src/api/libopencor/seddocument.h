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
 * @brief The SedDocument class.
 *
 * The SedDocument class is used to describe a simulation experiment that adheres to the SED-ML specification.
 */

class LIBOPENCOR_EXPORT SedDocument: public Logger
    , public std::enable_shared_from_this<SedDocument>
{
    friend class SedModel;
    friend class SedSimulation;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedDocument() override; /**< Destructor, @private. */

    SedDocument(const SedDocument &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedDocument(SedDocument &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedDocument &operator=(const SedDocument &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedDocument &operator=(SedDocument &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief Create a @ref SedDocument object.
     *
     * Factory method to create a @ref SedDocument object:
     *
     * ```
     * auto sedDocument = libOpenCOR::SedDocument::create();
     * ```
     *
     * @return A smart pointer to a @ref SedDocument object.
     */

    static SedDocumentPtr create();

    /**
     * @brief Initialise this simulation experiment description using the given @ref File.
     *
     * Initialise this simulation experiment description using the given @ref File.
     *
     * @param pFile The @ref File used to initialise this simulation experiment description.
     */

    void initialise(const FilePtr &pFile);

    /**
     * @brief Get the serialised version of this simulation experiment description.
     *
     * Return the @c std::string serialised version of this simulation experiment description.
     *
     * @return The @c std::string serialised version of this simulation experiment description.
     */

    std::string serialise() const;

    /**
     * @brief Get the serialised version of this simulation experiment description.
     *
     * Return the @c std::string serialised version of this simulation experiment description.
     *
     * @param pBasePath The @c std::string base path to use to determine the relative path, if possible, of a model
     * source.
     *
     * @return The @c std::string serialised version of this simulation experiment description.
     */

    std::string serialise(const std::string &pBasePath) const;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedDocument(); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR

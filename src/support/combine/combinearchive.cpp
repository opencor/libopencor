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

#include "combinearchive_p.h"

#include "utils.h"

#include "combine/combinearchive.h"

#include "libopencor/file.h"

#include "omex/CaContent.h"

namespace libOpenCOR {

CombineArchive::Impl::Impl(const FilePtr &pFile, libcombine::CombineArchive *pArchive)
    : mArchive(pArchive)
    , mArchiveLocation(libOpenCOR::pathToString(libOpenCOR::stringToPath(pFile->fileName() + ".contents/")))
    , mArchiveLocationSize(mArchiveLocation.size())
{
    // Extract all the files contained in the COMBINE archive.

    for (int i = 0; i < mArchive->getNumEntries(); ++i) {
        const auto *entry = mArchive->getEntry(i);
        auto location = entry->getLocation();
        auto file = File::create(mArchiveLocation + location);

        file->setContents(mArchive->extractEntryToBuffer(location));

        mFiles.push_back(file); // So that the files (besides the master file) don't get automatically deleted when we
                                // get out of scope.

        if (entry->getMaster()) {
            mMasterFile = file;
        }
    }
}

CombineArchive::Impl::~Impl()
{
    delete mArchive;
}

FilePtr CombineArchive::Impl::masterFile() const
{
    return mMasterFile;
}

bool CombineArchive::Impl::hasFiles() const
{
    return mFiles.empty();
}

size_t CombineArchive::Impl::fileCount() const
{
    return mFiles.size();
}

Strings CombineArchive::Impl::fileNames() const
{
    Strings res;

    for (const auto &file : mFiles) {
        res.push_back(file->fileName().substr(mArchiveLocationSize));
    }

    return res;
}

FilePtrs CombineArchive::Impl::files() const
{
    return mFiles;
}

FilePtr CombineArchive::Impl::file(size_t pIndex) const
{
    if (pIndex >= mFiles.size()) {
        return {};
    }

    return mFiles[pIndex];
}

FilePtr CombineArchive::Impl::file(const std::string &pFileName) const
{
    for (const auto &file : mFiles) {
        if (file->fileName() == mArchiveLocation + pFileName) {
            return file;
        }
    }

    return {};
}

CombineArchive::CombineArchive(const FilePtr &pFile, libcombine::CombineArchive *pArchive)
    : Logger(new Impl {pFile, pArchive})
{
}

CombineArchive::~CombineArchive()
{
    delete pimpl();
}

CombineArchive::Impl *CombineArchive::pimpl()
{
    return static_cast<Impl *>(Logger::pimpl());
}

const CombineArchive::Impl *CombineArchive::pimpl() const
{
    return static_cast<const Impl *>(Logger::pimpl());
}

CombineArchivePtr CombineArchive::create(const FilePtr &pFile)
{
    // Try to retrieve a COMBINE archive.
    // Note: a COMBINE archive is a ZIP file, so we make sure that it starts with 0x04034b50, which is the magic number
    //       used by a ZIP file (we ignore empty and spanned ZIP files). Indeed, libCOMBINE may crash depending on the
    //       file contents that is given to it.

    static const auto LS8 = 8U;
    static const auto LS16 = 16U;
    static const auto LS24 = 24U;
    static const auto ZIP_MAGIC_NUMBER = 0x04034b50;

    auto fileContents = pFile->contents();

    if ((fileContents.size() > 4)
        && (fileContents[0] + (fileContents[1] << LS8) + (fileContents[2] << LS16) + (fileContents[3] << LS24) == ZIP_MAGIC_NUMBER)) {
        auto *archive = new libcombine::CombineArchive {};

        archive->initializeFromBuffer(fileContents);

        return CombineArchivePtr {new CombineArchive {pFile, archive}};
    }

    return {};
}

FilePtr CombineArchive::masterFile() const
{
    return pimpl()->masterFile();
}

bool CombineArchive::hasFiles() const
{
    return !pimpl()->hasFiles();
}

size_t CombineArchive::fileCount() const
{
    return pimpl()->fileCount();
}

Strings CombineArchive::fileNames() const
{
    return pimpl()->fileNames();
}

FilePtrs CombineArchive::files() const
{
    return pimpl()->files();
}

FilePtr CombineArchive::file(size_t pIndex) const
{
    return pimpl()->file(pIndex);
}

FilePtr CombineArchive::file(const std::string &pFileName) const
{
    return pimpl()->file(pFileName);
}

} // namespace libOpenCOR

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

#include "omexfile_p.h"
#include "file_p.h"

#include "utils.h"

#include "combine/combinearchive.h"
#include "omex/CaContent.h"

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <unordered_map>

namespace libOpenCOR {

namespace {

std::string archiveLocation(const FilePtr &pFile)
{
    // Determine the location of the OMEX file, i.e. the location where its files are "virtually" extracted (e.g.,
    // "/some/path/archive.omex.contents/").
    // Note #1: the files of an OMEX file are managed by the file manager using their file name, so each OMEX file
    //          must have its own location. This means that a remote OMEX file that has no local copy (i.e. it was not
    //          downloaded), and therefore no file name, must use a virtual file name that is unique to it.
    // Note #2: we canonicalise the location since the files of the OMEX file have a canonical file name (see
    //          File::create()), which we need to be able to compare against (see OmexFile::Impl::file()). For
    //          instance, the temporary directory on macOS is a symbolic link (i.e. "/var/..." vs. "/private/var/...").

    static std::atomic<uint64_t> virtualFileNameCounter {0};

    auto fileName {pFile->fileName()};

    if (fileName.empty()) {
        std::error_code errorCode;

        fileName = pathToString(std::filesystem::temp_directory_path(errorCode) / ("libOpenCOR_" + std::to_string(++virtualFileNameCounter) + ".virtual"));
    }

    return pathToString(stringToPath(canonicalFileName(fileName + ".contents") + "/"));
}

} // namespace

OmexFile::Impl::Impl(const FilePtr &pFile, libcombine::CombineArchive *pArchive, UnsignedChars &&pArchiveContents)
    : mArchiveContents(std::move(pArchiveContents))
    , mArchive(pArchive)
    , mArchiveLocation(archiveLocation(pFile))
    , mArchiveLocationSize(mArchiveLocation.size())
{
    // Extract all the files contained in the OMEX file.

    auto fileCount {static_cast<size_t>(mArchive->getNumEntries())};

    mFiles.reserve(fileCount);
    mFileNames.reserve(fileCount);

    FilePtr cellmlFile;
    FilePtr sedmlFile;
    std::unordered_map<std::string, size_t> fileIndexes;
    Strings fileFormats;

    fileFormats.reserve(fileCount);

    for (int i {0}; i < mArchive->getNumEntries(); ++i) {
        const auto *entry {mArchive->getEntry(i)};
        auto location {entry->getLocation()};

        // Make sure that the file is located within the OMEX file.
        // Note #1: a location is a URI reference (see https://www.rfc-editor.org/rfc/rfc3986#section-4.1), so it may be
        //          percent-encoded (e.g., "my%20file.cellml"). Since mArchiveLocation is a local file name, we must
        //          decode the location to get the file name of the file.
        // Note #2: a location may have some ".." segments (e.g., "../file.cellml"), possibly percent-encoded (e.g.,
        //          "%2E%2E/file.cellml"), in which case the file would be located outside of the OMEX file. It
        //          could then be confused with another managed file (which contents would get replaced) or even with
        //          the OMEX file itself (resulting in an infinite recursion), so we ignore such a file. To
        //          determine where the file is located, we use its file name as determined by File::create().
        // Note #3: the file name of a file that is located within the OMEX file has no ".." component after the
        //          location of the OMEX file, unless it follows a symbolic link (see canonicalFileName()), in
        //          which case the file may be located outside of the OMEX file (e.g.,
        //          "archive.omex.contents/../file.cellml" if "archive.omex.contents" is a symbolic link), so we also
        //          ignore such a file.

        auto decodedLocation {decodeUrl(location)};
        auto fileNameOrUrl {mArchiveLocation + decodedLocation};
        auto [isLocalFile, fileName] {retrieveFileInfo(fileNameOrUrl)};
        auto isLocatedWithinArchive {fileName.starts_with(mArchiveLocation)};

        if (isLocatedWithinArchive) {
            auto filePathWithinArchive {stringToPath(fileName.substr(mArchiveLocationSize))};

            isLocatedWithinArchive = std::ranges::none_of(filePathWithinArchive, [](const auto &pComponent) {
                return pComponent == "..";
            });
        }

        if (!isLocatedWithinArchive) {
            std::string warning;

            warning.reserve(location.size() + 64); // NOLINT

            warning += "The file '";
            warning += location;
            warning += "' is not located within the OMEX file. It has been ignored.";

            addWarning(warning);

            continue;
        }

        // Skip a location that refers to the OMEX file itself.
        // Note: libCOMBINE skips the "." location, which is how an OMEX file normally refers to itself, but not
        //       the other locations that refer to it (e.g., "./" or "/"), which would otherwise result in a file for
        //       the directory in which we extract the OMEX file.

        if (fileName == mArchiveLocation) {
            continue;
        }

        // Make sure that the file is not the same as a file that we have already extracted.
        // Note: different locations may refer to the same file (e.g., "./my%20file.cellml" and "./my file.cellml", or
        //       "./file.cellml" and "file.cellml"). This is an ambiguity in the OMEX file itself, so we keep the
        //       first file and ignore the other ones (File::create() would otherwise return the same file, which
        //       contents would get replaced and which would be counted several times). We do, however, honour the
        //       master attribute of an ignored entry since it still refers to the file that we kept, and we keep track
        //       of its location so that it can still be used to retrieve that file (see file()).

        if (auto fileIndex {fileIndexes.find(fileName)}; fileIndex != fileIndexes.end()) {
            const auto &fileLocation {mFileNames[fileIndex->second]};
            std::string warning;

            warning.reserve(location.size() + fileLocation.size() + 64); // NOLINT

            warning += "The file '";
            warning += location;
            warning += "' is the same as the file '";
            warning += fileLocation;
            warning += "'. It has been ignored.";

            addWarning(warning);

            mIgnoredFileNameIndexes.emplace(location, fileIndex->second);

            if (entry->getMaster()) {
                mMasterFile = mFiles[fileIndex->second];
            }

            continue;
        }

        fileIndexes.emplace(fileName, mFiles.size());

        // Retrieve the contents of the file.
        // Note #1: a location is a URI reference while the name of an entry in the ZIP file is a file name, so the
        //          entry for a percent-encoded location (e.g., "my%20file.cellml") should be named after its decoded
        //          location (e.g., "my file.cellml"). However, libCOMBINE uses a location as is, so some tools name the
        //          entry after the location itself. So, we first look for an entry named after the decoded location
        //          and, if there is none, for an entry named after the location itself.
        // Note #2: we must use libcombine::CombineArchive::extractEntryToBuffer() since it extracts an entry in memory
        //          while libcombine::CombineArchive::extractEntryToStream() extracts it to a temporary file (which name
        //          is generated in a non thread-safe way) and returns empty contents if that file cannot be created.
        //          However, it returns empty contents for an entry that doesn't exist, so we cannot distinguish it from
        //          an empty entry, in which case we also look for an entry named after the location itself.

        auto contents {mArchive->extractEntryToBuffer(decodedLocation)};

        if (contents.empty()) {
            contents = mArchive->extractEntryToBuffer(location);
        }

        // Note: we only set the contents of the file for now, i.e. we don't check its type yet (see below).

        auto file {File::create(fileNameOrUrl)};

        file->pimpl()->setContents(contents);

        mFiles.push_back(file);
        mFileNames.push_back(location);
        fileFormats.push_back(entry->getFormat());

        if (entry->getMaster()) {
            mMasterFile = file;
        }
    }

    // Determine the type of each file using its contents or, if they are not recognised (e.g., malformed XML), using
    // the format specified in the manifest. This means that a broken SED-ML file is still considered to be a SED-ML
    // file and, as such, cannot be silently ignored in favour of a CellML file.
    // Note: we do this only once the contents of all the files have been set since a CellML file may import another
    //       CellML file from the OMEX file, and that file may come after it in the manifest.

    for (size_t i {0}; i < mFiles.size(); ++i) {
        const auto &file {mFiles[i]};

        file->pimpl()->checkType(file, true);

        auto fileType {file->type()};

        if (fileType == File::Type::UNKNOWN_FILE) {
            const auto &format {fileFormats[i]};

            if (isCellmlFormat(format)) {
                fileType = File::Type::CELLML_FILE;
            } else if (isSedmlFormat(format)) {
                fileType = File::Type::SEDML_FILE;
            }
        }

        if (fileType == File::Type::CELLML_FILE) {
            if (mCellmlFileCount == 0) {
                cellmlFile = file;
            }

            ++mCellmlFileCount;
        } else if (fileType == File::Type::SEDML_FILE) {
            if (mSedmlFileCount == 0) {
                sedmlFile = file;
            }

            ++mSedmlFileCount;
        }
    }

    // Determine the master file, if none was specified.
    // Note: the master attribute is optional, so if no master file was specified then we consider the SED-ML file to be
    //       the master file, if there is only one SED-ML file, or the CellML file to be the master file, if there is no
    //       SED-ML file and only one CellML file. Otherwise, it is up to the user (or a tool like Web OpenCOR) to
    //       decide which file should be used.

    if (mMasterFile == nullptr) {
        if (mSedmlFileCount == 1) {
            mMasterFile = sedmlFile;
        } else if ((mSedmlFileCount == 0) && (mCellmlFileCount == 1)) {
            mMasterFile = cellmlFile;
        }
    }
}

OmexFile::Impl::~Impl()
{
    delete mArchive;
}

const FilePtr &OmexFile::Impl::masterFile() const
{
    return mMasterFile;
}

size_t OmexFile::Impl::cellmlFileCount() const
{
    return mCellmlFileCount;
}

size_t OmexFile::Impl::sedmlFileCount() const
{
    return mSedmlFileCount;
}

bool OmexFile::Impl::hasFiles() const
{
    return !mFiles.empty();
}

size_t OmexFile::Impl::fileCount() const
{
    return mFiles.size();
}

const Strings &OmexFile::Impl::fileNames() const
{
    return mFileNames;
}

const FilePtrs &OmexFile::Impl::files() const
{
    return mFiles;
}

const FilePtr &OmexFile::Impl::file(size_t pIndex) const
{
    static const FilePtr NO_FILE_PTR;

    if (pIndex >= mFiles.size()) {
        return NO_FILE_PTR;
    }

    return mFiles[pIndex];
}

const FilePtr &OmexFile::Impl::file(const std::string &pFileName) const
{
    static const FilePtr NO_FILE_PTR;

    // Look for the file using its location, as returned by fileNames() (e.g., "./my%20file.cellml"), the location of an
    // entry that was ignored because it refers to the same file (e.g., "my%20file.cellml"), or its file name relative
    // to the OMEX file (e.g., "my file.cellml").
    // Note: a file name is never decoded (see retrieveFileInfo()), so we must look for the location of an ignored entry
    //       explicitly since it may be percent-encoded.

    for (size_t i {0}; i < mFileNames.size(); ++i) {
        if (mFileNames[i] == pFileName) {
            return mFiles[i];
        }
    }

    auto ignoredFileNameIndex {mIgnoredFileNameIndexes.find(pFileName)};

    if (ignoredFileNameIndex != mIgnoredFileNameIndexes.end()) {
        return mFiles[ignoredFileNameIndex->second];
    }

    auto [isLocalFile, fileName] {retrieveFileInfo(mArchiveLocation + pFileName)};

    for (const auto &file : mFiles) {
        if (file->fileName() == fileName) {
            return file;
        }
    }

    return NO_FILE_PTR;
}

OmexFile::OmexFile(const FilePtr &pFile, libcombine::CombineArchive *pArchive, UnsignedChars &&pArchiveContents)
    : Logger(std::make_unique<Impl>(pFile, pArchive, std::move(pArchiveContents)))
{
#ifdef CODE_COVERAGE_ENABLED
    (void)pimpl();
#endif
}

OmexFile::~OmexFile() = default;

OmexFile::Impl *OmexFile::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const OmexFile::Impl *OmexFile::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
}

OmexFilePtr OmexFile::create(const FilePtr &pFile)
{
    static const OmexFilePtr NO_OMEX_FILE_PTR;

    // Check whether the file is an OMEX file and if so then return its OmexFile object.

    if (pFile->pimpl()->type() == File::Type::OMEX_FILE) {
        return pFile->pimpl()->mOmexFile;
    }

    // Check whether the type of the file is already known and if so then return since there is no need to parse the
    // file contents again.

    if (pFile->pimpl()->mTypeChecked) {
        return NO_OMEX_FILE_PTR;
    }

    // Try to retrieve an OMEX file.
    // Note: an OMEX file is a ZIP file, so we make sure that it starts with 0x04034b50, which is the magic number
    //       used by a ZIP file (we ignore empty and spanned ZIP files). Indeed, libCOMBINE may crash depending on the
    //       file contents that is given to it.

    static const auto LS8 {8U};
    static const auto LS16 {16U};
    static const auto LS24 {24U};
    static const auto ZIP_MAGIC_NUMBER {0x04034b50};

    auto fileContents {pFile->contents()};

    if ((fileContents.size() > 4)
        && (fileContents[0] + (fileContents[1] << LS8) + (fileContents[2] << LS16) + (fileContents[3] << LS24) == ZIP_MAGIC_NUMBER)) {
        auto *archive {new libcombine::CombineArchive {}};

        archive->initializeFromBuffer(fileContents);

        return OmexFilePtr {new OmexFile {pFile, archive, std::move(fileContents)}};
        // Note: libCOMBINE retains pointers into the supplied buffer, so we must not destroy it until the archive is
        //       gone hence we keep the original ZIP buffer alive for the lifetime of the archive by moving it into the
        //       OmexFile::Impl object.
    }

    return NO_OMEX_FILE_PTR;
}

const FilePtr &OmexFile::masterFile() const
{
    return pimpl()->masterFile();
}

size_t OmexFile::cellmlFileCount() const
{
    return pimpl()->cellmlFileCount();
}

size_t OmexFile::sedmlFileCount() const
{
    return pimpl()->sedmlFileCount();
}

bool OmexFile::hasFiles() const
{
    return pimpl()->hasFiles();
}

size_t OmexFile::fileCount() const
{
    return pimpl()->fileCount();
}

const Strings &OmexFile::fileNames() const
{
    return pimpl()->fileNames();
}

const FilePtrs &OmexFile::files() const
{
    return pimpl()->files();
}

const FilePtr &OmexFile::file(size_t pIndex) const
{
    return pimpl()->file(pIndex);
}

const FilePtr &OmexFile::file(const std::string &pFileName) const
{
    return pimpl()->file(pFileName);
}

} // namespace libOpenCOR

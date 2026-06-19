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

// Extract all object files from a static archive (.a) file, ensuring that object files with duplicate names are all
// preserved by prepending a unique index to each output filename.
//
// Handle the following ar formats:
// - Standard POSIX ar (short names in the header field);
// - BSD ar format with #1/longname convention (used by Apple's ar); and
// - GNU/Linux ar format with // string table and /N name references.

#include <cstdint>
#include <cstring>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef _WIN32
#    include <fcntl.h>
#    include <sys/mman.h>
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#pragma pack(push, 1)
struct ArHeader
{
    char name[16];
    char date[12];
    char uid[6];
    char gid[6];
    char mode[8];
    char size[10];
    char magic[2];
};
#pragma pack(pop)

constexpr auto ArHeaderSize = sizeof(ArHeader);

static_assert(ArHeaderSize == 60, "The ar header must be exactly 60 bytes long.");

int main(int pArgC, char *pArgV[])
{
    // Validate command-line arguments.

    if (pArgC != 2) {
        std::cerr << "Usage: " << pArgV[0] << " <archive>\n";

        return 1;
    }

    // Validate the archive path.

    std::filesystem::path archivePath(pArgV[1]);

    if (!std::filesystem::is_regular_file(archivePath)) {
        std::cerr << "Error: " << archivePath.string() << " is not a file.\n";

        return 1;
    }

    // Read the entire archive into memory using the fastest available method for the platform:
    //  - Windows: single bulk read;
    //  - Linux/macOS: memory-mapped I/O for zero-copy access.

    const char *data = nullptr;
    size_t dataSize = 0;

#ifdef _WIN32
    std::ifstream file(archivePath, std::ios::binary);

    if (!file) {
        std::cerr << "Error: could not open " << archivePath.string() << ".\n";

        return 1;
    }

    file.seekg(0, std::ios::end);

    dataSize = static_cast<size_t>(file.tellg());

    file.seekg(0, std::ios::beg);

    std::vector<char> fileBuffer(dataSize);

    file.read(fileBuffer.data(), static_cast<std::streamsize>(dataSize));
    file.close();

    data = fileBuffer.data();
#else
    struct stat st;

    if (::stat(archivePath.c_str(), &st) != 0) {
        std::cerr << "Error: could not access " << archivePath.string() << ".\n";

        return 1;
    }

    dataSize = static_cast<size_t>(st.st_size);

    auto fd {::open(archivePath.c_str(), O_RDONLY)};

    if (fd == -1) {
        std::cerr << "Error: could not open " << archivePath.string() << ".\n";

        return 1;
    }

    auto mapped {::mmap(nullptr, dataSize, PROT_READ, MAP_PRIVATE, fd, 0)};

    ::close(fd);

    if (mapped == MAP_FAILED) {
        std::cerr << "Error: could not mmap " << archivePath.string() << ".\n";

        return 1;
    }

    data = static_cast<const char *>(mapped);
#endif

    // Check the archive magic.

    if ((dataSize < 8) || (std::memcmp(data, "!<arch>\n", 8) != 0)) {
        std::cerr << "Error: " << archivePath.string() << " is not a valid ar archive.\n";

#ifndef _WIN32
        ::munmap(const_cast<char *>(data), dataSize);
#endif

        return 1;
    }

    // GNU/Linux ar stores long names in a string table ('//' member). Member headers reference string table entries via
    // /N notation.

    size_t pos {8};
    int memberIndex {0};
    std::string stringTable;
    auto outputDir {std::filesystem::current_path()};

    // Pre-allocate a large output buffer (64 KB) shared across all member writes to reduce write syscalls.

    constexpr size_t OutputBufferSize = 65536;
    std::vector<char> outputBuffer(OutputBufferSize);

    while (pos + ArHeaderSize <= dataSize) {
        // Parse the ar header for the current member.

        ArHeader header;

        std::memcpy(&header, data + pos, ArHeaderSize);

        // Validate the header magic bytes (0x60 0x0A).

        if ((header.magic[0] != '\x60') || (header.magic[1] != '\x0A')) {
            break;
        }

        // Parse the size field (right-justified decimal, 10 characters).

        long memberSize {};

        if (auto [ptr, ec] = std::from_chars(header.size, header.size + 10, memberSize);
            ec != std::errc {}) {
            break;
        }

        pos += ArHeaderSize;

        if (pos + memberSize > dataSize) {
            break;
        }

        // Decode the member name from the 16-byte header field.

        char nameBuf[17] {};

        std::memcpy(nameBuf, header.name, 16);

        auto nameEnd = 16;

        while ((nameEnd > 0) && (nameBuf[nameEnd - 1] == ' ')) {
            --nameEnd;
        }

        nameBuf[nameEnd] = '\0';

        std::string memberName;
        const char *memberData = nullptr;
        long memberDataSize = 0;

        if ((nameEnd >= 3) && (std::memcmp(nameBuf, "#1/", 3) == 0)) {
            // BSD ar format (#1/N): the first N bytes of the data area contain the member name.

            long nameSize {};

            if (auto [ptr, ec] = std::from_chars(nameBuf + 3, nameBuf + nameEnd, nameSize);
                ec != std::errc {}) {
                nameSize = 0;
            }

            if (nameSize > memberSize) {
                nameSize = static_cast<long>(memberSize);
            }

            // Strip trailing nulls from the name.

            if (nameSize > 0) {
                auto rawEnd = static_cast<size_t>(nameSize);

                while ((rawEnd > 0) && (data[pos + rawEnd - 1] == '\0')) {
                    --rawEnd;
                }

                memberName.assign(data + pos, rawEnd);
            }

            // The actual member data starts immediately after the name.

            memberData = data + pos + nameSize;
            memberDataSize = memberSize - nameSize;
        } else {
            // Standard POSIX or GNU/Linux ar format.

            memberName.assign(nameBuf, static_cast<size_t>(nameEnd));

            // POSIX ar appends a '/' to mark the end of the name.

            if (!memberName.empty() && (memberName.back() == '/')) {
                memberName.pop_back();
            }

            // The member data starts immediately after the header.

            memberData = data + pos;
            memberDataSize = memberSize;
        }

        // Get the GNU/Linux ar string table content ('//' member).

        if ((nameEnd == 2) && (nameBuf[0] == '/') && (nameBuf[1] == '/')) {
            stringTable.assign(memberData, static_cast<size_t>(memberDataSize));
        }

        // Resolve GNU/Linux ar extended names (/N format).

        if (!memberName.empty() && (memberName[0] == '/') && (memberName.size() > 1) && !stringTable.empty()) {
            auto allDigits {true};

            for (size_t i = 1; i < memberName.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(memberName[i]))) {
                    allDigits = false;

                    break;
                }
            }

            if (allDigits) {
                long offset {};

                if (auto [ptr, ec] = std::from_chars(memberName.data() + 1, memberName.data() + memberName.size(), offset);
                    (ec == std::errc {}) && (offset >= 0) && (static_cast<size_t>(offset) < stringTable.size())) {
                    auto end {static_cast<size_t>(offset)};

                    while ((end < stringTable.size()) && (stringTable[end] != '/') && (stringTable[end] != '\n') && (stringTable[end] != '\0')) {
                        ++end;
                    }

                    memberName = stringTable.substr(static_cast<size_t>(offset), end - static_cast<size_t>(offset));
                }
            }
        }

        // Skip special / header-only members (symbol table, string table, etc.).

        if (memberName.empty() || (memberName == " ") || (memberName == "__.SYMDEF") || (memberName == "__.SYMDEF SORTED") || (memberName == "/")) {
            pos += memberSize;

            if (memberSize % 2 != 0) {
                ++pos; // Odd-sized members are padded with a newline.
            }

            continue;
        }

        // Write the member with an index prefix to guarantee unique filenames, preventing member name collisions within
        // the archive.

        char prefix[16] {};

        std::snprintf(prefix, sizeof(prefix), "%04d", memberIndex);

        auto outputPath {outputDir / (std::string(prefix) + "_" + std::filesystem::path(memberName).filename().string())};
        std::ofstream outputFile(outputPath, std::ios::binary);

        if (!outputFile) {
            std::cerr << "Error: could not write " << outputPath.string() << "\n";

#ifndef _WIN32
            ::munmap(const_cast<char *>(data), dataSize);
#endif

            return 1;
        }

        outputFile.rdbuf()->pubsetbuf(outputBuffer.data(), OutputBufferSize);
        outputFile.write(memberData, memberDataSize);
        outputFile.close();

        pos += memberSize;

        if (memberSize % 2 != 0) {
            ++pos; // Odd-sized members are padded with a newline.
        }

        ++memberIndex;
    }

#ifndef _WIN32
    ::munmap(const_cast<char *>(data), dataSize);
#endif

    return 0;
}

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

#include "../extern/modp_b64/modp_b64.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(int pArgC, char *pArgV[])
{
    // Convert the given file to a base64-encoded string.

    if (pArgC != 2) {
        std::cerr << "Usage: " << pArgV[0] << " <file>" << std::endl;

        return 1;
    }

    std::uintmax_t fileSize = std::filesystem::file_size(pArgV[1]);
    char *buffer = new char[fileSize];
    std::ifstream file(pArgV[1], std::ios::binary);

    file.read(buffer, fileSize);

    if (!file) {
        std::cerr << "Error: the file could not be read." << std::endl;

        return 1;
    }

    file.close();

    char *base64 = new char[modp_b64_encode_len(fileSize)];

    modp_b64_encode(base64, buffer, fileSize);

    std::cout << base64 << std::endl;

    delete[] base64;
    delete[] buffer;

    return 0;
}

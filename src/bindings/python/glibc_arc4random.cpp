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

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#if defined(__linux__) && defined(__GNUC__)
// arc4random(), arc4random_buf(), and arc4random_uniform() were added in glibc 2.36, so we need to provide a
// compatibility implementation on older glibc versions.

extern "C" {
__attribute__((weak)) uint32_t arc4random()
{
    int fd;

    do {
        fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    } while ((fd < 0) && (errno == EINTR));

    if (fd < 0) {
        __builtin_abort();
    }

    uint32_t res = 0;
    size_t offset = 0;

    while (offset < sizeof(res)) {
        ssize_t bytesRead = read(fd, reinterpret_cast<char *>(&res) + offset, sizeof(res) - offset);

        if (bytesRead > 0) {
            offset += static_cast<size_t>(bytesRead);
        } else if ((bytesRead < 0) && (errno == EINTR)) {
            continue;
        } else {
            // EOF or unrecoverable error.

            break;
        }
    }

    close(fd);

    if (offset != sizeof(res)) {
        __builtin_abort();
    }

    return res;
}

__attribute__((weak)) void arc4random_buf(void *pBuf, size_t pSize)
{
    if (pSize == 0) {
        return;
    }

    int fd;

    do {
        fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    } while ((fd < 0) && (errno == EINTR));

    if (fd < 0) {
        __builtin_abort();
    }

    size_t offset = 0;

    while (offset < pSize) {
        ssize_t bytesRead = read(fd, static_cast<char *>(pBuf) + offset, pSize - offset);

        if (bytesRead > 0) {
            offset += static_cast<size_t>(bytesRead);
        } else if ((bytesRead < 0) && (errno == EINTR)) {
            continue;
        } else {
            // EOF or unrecoverable error.

            break;
        }
    }

    close(fd);

    if (offset != pSize) {
        __builtin_abort();
    }
}

__attribute__((weak)) uint32_t arc4random_uniform(uint32_t pUpperBound)
{
    if (pUpperBound < 2) {
        return 0;
    }

    uint32_t rejectionThreshold = -pUpperBound % pUpperBound;
    uint32_t number;

    do {
        number = arc4random();
    } while (number < rejectionThreshold);

    return number % pUpperBound;
}
}

#endif

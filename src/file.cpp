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

#include "libopencor/file.h"

namespace libOpenCOR {

/**
 * Private implementation.
 */

struct File::FileImpl
{
    std::string mFileNameOrUrl;
};

/**
 * Public implementation.
 */

File::File(const std::string &pFileNameOrUrl)
    : mPimpl(new FileImpl())
{
    mPimpl->mFileNameOrUrl = pFileNameOrUrl;
}

File::~File()
{
    delete mPimpl;
}

FilePtr File::create(const std::string &pFileNameOrUrl)
{
    return std::shared_ptr<File> {new File {pFileNameOrUrl}};
}

} // namespace libOpenCOR

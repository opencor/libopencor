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

#include "version_p.h"

#include "curl/curlver.h"

#include "libcellml/version.h"

#include "libopencor/version.h"

#include "libsedmlbegin.h"
#include "sedml/common/libsedml-version.h"
#include "libsedmlend.h"

#include "omex/common/libcombine-version.h"

namespace libOpenCOR {

unsigned int version()
{
    return LIBOPENCOR_VERSION;
}

std::string versionString()
{
    return LIBOPENCOR_VERSION_STRING;
}

unsigned int libcellmlVersion()
{
    return libcellml::version();
}

std::string libcellmlVersionString()
{
    return libcellml::versionString();
}

unsigned int libcombineVersion()
{
    return static_cast<unsigned int>(libcombine::getLibCombineVersion());
}

std::string libcombineVersionString()
{
    return libcombine::getLibCombineDottedVersion();
}

unsigned int libcurlVersion()
{
    return LIBCURL_VERSION_NUM;
}

std::string libcurlVersionString()
{
    return LIBCURL_VERSION;
}

unsigned int libsedmlVersion()
{
    return static_cast<unsigned int>(libsedml::getLibSEDMLVersion());
}

std::string libsedmlVersionString()
{
    return libsedml::getLibSEDMLDottedVersion();
}

} // namespace libOpenCOR

# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import datetime
import libopencor as loc


version_major = 0
version_patch = 0

now = datetime.datetime.now()
year = now.year
month = now.month
day = now.day


def test_version():
    version = 0
    number = (
        10000000000 * version_major
        + 100 * (10000 * year + 100 * month + day)
        + version_patch
    )
    i = 0

    while number != 0:
        version |= (number % 10) << i
        number //= 10
        i += 4

    assert isinstance(loc.version(), int)
    assert loc.version() == version


def test_version_string():
    version = f"{version_major}.{year}{month:02}{day:02}.{version_patch}"

    assert loc.__version__ == version

    assert isinstance(loc.version_string(), str)
    assert loc.version_string() == version


def test_clang_version():
    assert isinstance(loc.clang_version(), int)
    assert loc.clang_version() == 0x160006


def test_clang_version_string():
    assert isinstance(loc.clang_version_string(), str)
    assert loc.clang_version_string() == "16.0.6"


def test_libcellml_version():
    assert isinstance(loc.libcellml_version(), int)
    assert loc.libcellml_version() == 0x000500


def test_libcellml_version_string():
    assert isinstance(loc.libcellml_version_string(), str)
    assert loc.libcellml_version_string() == "0.5.0"


def test_libcombine_version():
    assert isinstance(loc.libcombine_version(), int)
    assert loc.libcombine_version() == 220


def test_libcombine_version_string():
    assert isinstance(loc.libcombine_version_string(), str)
    assert loc.libcombine_version_string() == "0.2.20"


def test_libcurl_version():
    assert isinstance(loc.libcurl_version(), int)
    assert loc.libcurl_version() == 0x080D00


def test_libcurl_version_string():
    assert isinstance(loc.libcurl_version_string(), str)
    assert loc.libcurl_version_string() == "8.13.0-DEV"


def test_libsedml_version():
    assert isinstance(loc.libsedml_version(), int)
    assert loc.libsedml_version() == 20033


def test_libsedml_version_string():
    assert isinstance(loc.libsedml_version_string(), str)
    assert loc.libsedml_version_string() == "2.0.33"


def test_llvm_version():
    assert isinstance(loc.llvm_version(), int)
    assert loc.llvm_version() == 0x160006


def test_llvm_version_string():
    assert isinstance(loc.llvm_version_string(), str)
    assert loc.llvm_version_string() == "16.0.6"


def test_sundials_version():
    assert isinstance(loc.sundials_version(), int)
    assert loc.sundials_version() == 0x070300


def test_sundials_version_string():
    assert isinstance(loc.sundials_version_string(), str)
    assert loc.sundials_version_string() == "7.3.0"

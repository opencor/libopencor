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
import libopencor as oc


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

    assert isinstance(oc.version(), int)
    assert oc.version() == version


def test_version_string():
    version = f"{version_major}.{year}{month:02}{day:02}.{version_patch}"

    assert oc.__version__ == version

    assert isinstance(oc.version_string(), str)
    assert oc.version_string() == version


def test_clang_version():
    assert isinstance(oc.clang_version(), int)
    assert oc.clang_version() == 0x160006


def test_clang_version_string():
    assert isinstance(oc.clang_version_string(), str)
    assert oc.clang_version_string() == "16.0.6"


def test_libcellml_version():
    assert isinstance(oc.libcellml_version(), int)
    assert oc.libcellml_version() == 0x000500


def test_libcellml_version_string():
    assert isinstance(oc.libcellml_version_string(), str)
    assert oc.libcellml_version_string() == "0.5.0"


def test_libcombine_version():
    assert isinstance(oc.libcombine_version(), int)
    assert oc.libcombine_version() == 220


def test_libcombine_version_string():
    assert isinstance(oc.libcombine_version_string(), str)
    assert oc.libcombine_version_string() == "0.2.20"


def test_libcurl_version():
    assert isinstance(oc.libcurl_version(), int)
    assert oc.libcurl_version() == 0x080D00


def test_libcurl_version_string():
    assert isinstance(oc.libcurl_version_string(), str)
    assert oc.libcurl_version_string() == "8.13.0-DEV"


def test_libsedml_version():
    assert isinstance(oc.libsedml_version(), int)
    assert oc.libsedml_version() == 20033


def test_libsedml_version_string():
    assert isinstance(oc.libsedml_version_string(), str)
    assert oc.libsedml_version_string() == "2.0.33"


def test_llvm_version():
    assert isinstance(oc.llvm_version(), int)
    assert oc.llvm_version() == 0x160006


def test_llvm_version_string():
    assert isinstance(oc.llvm_version_string(), str)
    assert oc.llvm_version_string() == "16.0.6"


def test_sundials_version():
    assert isinstance(oc.sundials_version(), int)
    assert oc.sundials_version() == 0x070300


def test_sundials_version_string():
    assert isinstance(oc.sundials_version_string(), str)
    assert oc.sundials_version_string() == "7.3.0"

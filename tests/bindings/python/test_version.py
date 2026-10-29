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


import libopencor as loc
import pathlib


def test_libopencor():
    with open(pathlib.Path(__file__).parent.parent.parent / "VERSION.txt") as file:
        version_string = file.read().strip()

    major_version, date_string, patch_version = version_string.split(".")
    major_version = int(major_version)
    year = int(date_string[:4])
    month = int(date_string[4:6])
    day = int(date_string[6:8])
    patch_version = int(patch_version)
    version = 0
    number = (
        10000000000 * major_version
        + 100 * (10000 * year + 100 * month + day)
        + patch_version
    )
    i = 0

    while number != 0:
        version |= (number % 10) << i
        number //= 10
        i += 4

    assert loc.version() == version
    assert loc.version_string() == version_string

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.version(), int)
    assert isinstance(loc.version_string(), str)

    # The version string is also available as the version of our Python module.
    # Note: this has no C++ or JavaScript equivalent since __version__ is a Python convention.

    assert loc.__version__ == version_string


def test_libcellml():
    assert loc.libcellml_version() == 0x000702
    assert loc.libcellml_version_string() == "0.7.2"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.libcellml_version(), int)
    assert isinstance(loc.libcellml_version_string(), str)


def test_libcombine():
    assert loc.libcombine_version() == 220
    assert loc.libcombine_version_string() == "0.2.20"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.libcombine_version(), int)
    assert isinstance(loc.libcombine_version_string(), str)


def test_libcurl():
    # Note: this has no JavaScript equivalent since our JavaScript bindings are built without libcurl.

    assert loc.libcurl_version() == 0x081500
    assert loc.libcurl_version_string() == "8.21.0-DEV"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.libcurl_version(), int)
    assert isinstance(loc.libcurl_version_string(), str)


def test_libsedml():
    assert loc.libsedml_version() == 20033
    assert loc.libsedml_version_string() == "2.0.33"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.libsedml_version(), int)
    assert isinstance(loc.libsedml_version_string(), str)


def test_llvm():
    assert loc.llvm_version() == 0x220108
    assert loc.llvm_version_string() == "22.1.8"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.llvm_version(), int)
    assert isinstance(loc.llvm_version_string(), str)


def test_sundials():
    assert loc.sundials_version() == 0x070700
    assert loc.sundials_version_string() == "7.7.0"

    # The version is an integer and the version string is a string.
    # Note: this has no C++ equivalent since C++ is statically typed.

    assert isinstance(loc.sundials_version(), int)
    assert isinstance(loc.sundials_version_string(), str)

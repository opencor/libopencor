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


import libopencor


def test_version():
    assert isinstance(libopencor.version(), int)
    assert libopencor.version() > 0


def test_version_string():
    assert isinstance(libopencor.version_string(), str)
    assert libopencor.version_string() != ""


def test_libcellml_version():
    assert isinstance(libopencor.libcellml_version(), int)
    assert libopencor.libcellml_version() > 0


def test_libcellml_version_string():
    assert isinstance(libopencor.libcellml_version_string(), str)
    assert libopencor.libcellml_version_string() != ""


def test_libcombine_version():
    assert isinstance(libopencor.libcombine_version(), int)
    assert libopencor.libcombine_version() > 0


def test_libcombine_version_string():
    assert isinstance(libopencor.libcombine_version_string(), str)
    assert libopencor.libcombine_version_string() != ""


def test_libcurl_version():
    assert isinstance(libopencor.libcurl_version(), int)
    assert libopencor.libcurl_version() > 0


def test_libcurl_version_string():
    assert isinstance(libopencor.libcurl_version_string(), str)
    assert libopencor.libcurl_version_string() != ""


def test_libsedml_version():
    assert isinstance(libopencor.libsedml_version(), int)
    assert libopencor.libsedml_version() > 0


def test_libsedml_version_string():
    assert isinstance(libopencor.libsedml_version_string(), str)
    assert libopencor.libsedml_version_string() != ""


def test_sundials_version():
    assert isinstance(libopencor.sundials_version(), int)
    assert libopencor.sundials_version() > 0


def test_sundials_version_string():
    assert isinstance(libopencor.sundials_version_string(), str)
    assert libopencor.sundials_version_string() != ""

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
from skbuild import setup

year = datetime.datetime.now().year
month = datetime.datetime.now().month
day = datetime.datetime.now().day

setup(
    name="libopencor",
    version=f"{year}.{month:02}.{day:02}",
    description="libOpenCOR is the backend library to OpenCOR, an open source cross-platform modelling environment.",
    author="libOpenCOR contributors",
    url="https://opencor.ws/libopencor",
    license="Apache 2.0",
    packages=["libopencor"],
    package_dir={"": "src/bindings/python"},
    cmake_args=["-DBUILD_TYPE=Release", "-DPYTHON_BINDINGS=ON", "-DSHARED_LIBS=OFF"],
    cmake_install_dir="src/bindings/python/libopencor",
    exclude_package_data={"": ["bin/*", "cmake/*", "include/*", "lib/*"]},
)

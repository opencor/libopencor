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


from skbuild import setup

with open("VERSION.txt", "r", encoding="utf-8") as version_file:
    version = version_file.read().strip()

with open("README.md", "r", encoding="utf-8") as readme_file:
    long_description = readme_file.read()

setup(
    name="libopencor",
    version=version,
    description="libOpenCOR is the backend library to OpenCOR, a cross-platform modelling environment, which can be used to organise, edit, simulate and analyse CellML files.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="libOpenCOR contributors",
    url="https://opencor.ws/libopencor/",
    license="Apache 2.0",
    packages=["libopencor"],
    package_dir={"": "src/bindings/python"},
    cmake_args=[
        "-DBUILD_TYPE=Release",
        "-DPYTHON_BINDINGS=ON",
        "-DSHARED_LIBS=OFF",
        "-DUNIT_TESTING=OFF",
    ],
    cmake_install_dir="src/bindings/python/libopencor",
    exclude_package_data={"": ["bin/*", "cmake/*", "include/*", "lib/*"]},
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Education",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++",
        "Programming Language :: JavaScript",
        "Programming Language :: Python",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
)

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

setup(
    name='libopencor',
    version='0.0.0',
    description='libOpenCOR is the backend library to OpenCOR, an open source cross-platform modelling environment.',
    license='Apache 2.0',
    packages=['libopencor'],
    package_dir={'': 'src/bindings/python'},
    cmake_install_dir='src/bindings/python/libopencor',
    include_package_data=True,
)

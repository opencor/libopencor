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


import coverage
import os
import sys
import unittest

os.chdir('@PYTHON_COVERAGE_WORKING_DIRECTORY@')

cov = coverage.Coverage(source=['libopencor'])

cov.start()

unittest.TextTestRunner().run(unittest.TestLoader().discover('.'))

cov.stop()
cov.save()

if len(sys.argv) > 1:
    report_format = sys.argv[1]
else:
    report_format = ''

if report_format == 'html':
    cov.html_report(directory='python_coverage_report')
else:
    cov.report()

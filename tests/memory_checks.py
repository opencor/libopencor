#!/usr/bin/env python

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

# Do memory checks using Memcheck from Valgrind (on Linux) or Leaks (on macOS).
# Note: this script is based on the runMemcheck.py script that used to be part
#       of KDevelop (see
#       https://invent.kde.org/kdevelop/kdevelop/-/blob/3973/veritas/tests/runMemcheck.py).

import math
import os
import platform
import shutil
import sys
import xml.dom.minidom


def child_node_data(dom, tag):
    res = None
    element = dom.getElementsByTagName(tag)

    if len(element) != 0:
        res = element[0].firstChild.data

    return res


class Frame:
    def __init__(self, frame_node):
        self.function = child_node_data(frame_node, "fn")
        self.file = child_node_data(frame_node, "file")
        self.line = child_node_data(frame_node, "line")

    def __str__(self):
        res = ""

        if self.function:
            res += "     " + self.function

        if self.file and self.line:
            res += " (" + self.file + ":" + self.line + ")"

        res += "\n"

        return res


class BackTrace:
    def __init__(self, error_node):
        self.kind = error_node.getElementsByTagName("kind")[0].firstChild.data
        self.stack = []

        for frame in error_node.getElementsByTagName("frame"):
            if child_node_data(frame, "fn"):
                self.stack.append(Frame(frame))

    def is_leak(self):
        if self.kind != "Leak_DefinitelyLost":
            return False

        for frame in self.stack:
            if "libOpenCOR::" in frame.function:
                return True

        return False

    def __str__(self):
        out = "   Traceback (most recent call first):\n"

        for frame in self.stack:
            out += str(frame)

        return out


def parse_errors(output):
    res = []

    if platform.system() == "Linux":
        dom = xml.dom.minidom.parseString(output)
        error = dom.getElementsByTagName("error")

        for stack in error:
            back_trace = BackTrace(stack)

            if back_trace.is_leak():
                res.append(back_trace)
    else:
        leaks_report_line = 0
        lines = ""

        for line in output.split("\n"):
            if leaks_report_line == 0:
                if line.startswith("leaks Report Version"):
                    leaks_report_line = 1
            else:
                lines += line + "\n"

                leaks_report_line += 1

                if leaks_report_line == 3:
                    if "0 leaks for 0 total leaked bytes." in line:
                        return []

        res.append(lines)

    return res


def garbage(line):
    return not line.startswith("<unknown program name>") and not line.startswith(
        "profiling:"
    )


def check_memory(tool, test, test_path):
    if platform.system() == "Linux":
        os.system(
            f"{tool} --tool=memcheck --child-silent-after-fork=yes --leak-check=full --xml=yes --xml-fd=3 --num-callers=50 {test_path} 1>{test}.txt 2>{test}.err 3>{test}.xml"
        )

        return "".join(list(filter(garbage, open(f"{test}.xml").readlines())))
    else:
        os.system(f"{tool} --quiet -atExit -- {test_path} 1>{test}.txt 2>{test}.err")

        return open(f"{test}.txt").read()


def run_test(tool, test, test_path):
    sys.stdout.write(f"-- Checking memory in {test} - ")

    if not os.access(test_path, os.X_OK):
        sys.stdout.write("Not found\n")

        return False

    errors = parse_errors(check_memory(tool, test, test_path))

    if len(errors) == 0:
        sys.stdout.write("Success\n")

        return True

    sys.stdout.write("Failed\n")
    sys.stderr.write("---------------------------------------\n")

    for error in errors:
        sys.stderr.write(str(error))
        sys.stderr.write("---------------------------------------\n")

    return False


if __name__ == "__main__":
    if len(sys.argv) > 2:
        if platform.system() == "Linux":
            tool = shutil.which("valgrind")

            if tool == None:
                sys.stderr.write("-- Valgrind could not be found.\n")

                sys.exit(3)
        else:
            tool = shutil.which("leaks")

            if tool == None:
                sys.stderr.write("-- The leaks tool could not be found.\n")

                sys.exit(3)

        exit_code = 0
        tests_dir = sys.argv[1]
        tests = sys.argv[2:]
        results = []

        for test in tests:
            results.append(run_test(tool, test, os.path.join(tests_dir, test)))

        successes = []
        failures = []

        for index, result in enumerate(results):
            if result:
                successes.append(tests[index])
            else:
                failures.append(tests[index])

                exit_code = 2

        total = len(successes) + len(failures)

        sys.stdout.write("-- Summary:\n")
        sys.stdout.write(
            f"     {math.ceil(100.0 * len(successes) / total)}% tests passed, {len(failures)} tests failed out of {total}.\n"
        )

        if len(failures):
            sys.stdout.write("\n")
            sys.stdout.write("     The failed tests are:\n")

            for failure in failures:
                sys.stdout.write(f"      - {failure}\n")

            sys.stdout.write("\n")

        sys.exit(exit_code)
    else:
        sys.stderr.write(
            f"Usage: python3 {os.path.basename(sys.argv[0])} test_exectable_dir test_executable_1 [test_exectuable_2 ...]"
        )

        sys.exit(1)

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
import threading
import utils


def test_parallel_independent_instances():
    INSTANCE_COUNT = 3
    SIMULATION_PROPERTY = 10000

    instances = []

    for _ in range(INSTANCE_COUNT):
        file = loc.File(utils.resource_path("cellml_2.cellml"))
        document = loc.SedDocument(file)
        simulation = document.simulations[0]

        simulation.number_of_steps = SIMULATION_PROPERTY
        simulation.output_end_time = float(SIMULATION_PROPERTY)

        instances.append(document.instantiate())

    completed_count = 0
    mutex = threading.Lock()

    def run_instance(instance):
        nonlocal completed_count

        instance.run()

        with mutex:
            completed_count += 1

    threads = []

    for instance in instances:
        thread = threading.Thread(target=run_instance, args=(instance,))

        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

    assert completed_count == INSTANCE_COUNT

    for instance in instances:
        assert instance.progress == 1.0
        assert not instance.has_issues


def test_parallel_shared_document():
    SIMULATION_PROPERTY = 10000

    file = loc.File(utils.resource_path("cellml_2.cellml"))
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.number_of_steps = SIMULATION_PROPERTY
    simulation.output_end_time = float(SIMULATION_PROPERTY)

    instance1 = document.instantiate()
    instance2 = document.instantiate()
    instance3 = document.instantiate()

    done_flags = [False, False, False]
    mutex = threading.Lock()

    def run_instance(instance, index):
        instance.run()

        with mutex:
            done_flags[index] = True

    threads = [
        threading.Thread(target=run_instance, args=(instance1, 0)),
        threading.Thread(target=run_instance, args=(instance2, 1)),
        threading.Thread(target=run_instance, args=(instance3, 2)),
    ]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    assert done_flags[0]
    assert done_flags[1]
    assert done_flags[2]

    assert instance1.progress == 1.0
    assert instance2.progress == 1.0
    assert instance3.progress == 1.0

    assert not instance1.has_issues
    assert not instance2.has_issues
    assert not instance3.has_issues


def test_parallel_async_lifecycle():
    INSTANCE_COUNT = 3
    SIMULATION_PROPERTY = 10000

    instances = []

    for _ in range(INSTANCE_COUNT):
        file = loc.File(utils.resource_path("cellml_2.cellml"))
        document = loc.SedDocument(file)
        simulation = document.simulations[0]

        simulation.number_of_steps = SIMULATION_PROPERTY
        simulation.output_end_time = float(SIMULATION_PROPERTY)

        instances.append(document.instantiate())

    for instance in instances:
        assert instance.start_run() is True

    for instance in instances:
        assert instance.wait_for_run() > 0.0

    for instance in instances:
        assert instance.is_running is False
        assert not instance.has_issues

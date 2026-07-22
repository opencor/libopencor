/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "tests/utils.h"

#include <libopencor>

#include <atomic>
#include <thread>
#include <vector>

TEST(ConcurrentSedTest, parallelIndependentInstances)
{
    static const auto INSTANCE_COUNT {3U};
    static const auto SIMULATION_PROPERTY {10000};

    std::vector<libOpenCOR::SedInstancePtr> instances;

    for (size_t i {0}; i < INSTANCE_COUNT; ++i) {
        auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
        auto document {libOpenCOR::SedDocument::create(file)};
        const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

        simulation->setNumberOfSteps(SIMULATION_PROPERTY);
        simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

        instances.push_back(document->instantiate());
    }

    std::vector<std::thread> threads;
    std::atomic<size_t> completedCount {0};

    threads.reserve(instances.size());

    for (auto &instance : instances) {
        threads.emplace_back([&instance, &completedCount]() {
            instance->run();

            ++completedCount;
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_EQ(completedCount.load(), INSTANCE_COUNT);

    for (const auto &instance : instances) {
        EXPECT_DOUBLE_EQ(instance->progress(), 1.0);
        EXPECT_FALSE(instance->hasIssues());
    }
}

TEST(ConcurrentSedTest, parallelSharedDocument)
{
    static const auto SIMULATION_PROPERTY {10000};

    auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    simulation->setNumberOfSteps(SIMULATION_PROPERTY);
    simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

    auto instance1 {document->instantiate()};
    auto instance2 {document->instantiate()};
    auto instance3 {document->instantiate()};

    std::atomic<bool> instance1Done {false};
    std::atomic<bool> instance2Done {false};
    std::atomic<bool> instance3Done {false};

    std::thread thread1([&instance1, &instance1Done]() {
        instance1->run();

        instance1Done.store(true);
    });

    std::thread thread2([&instance2, &instance2Done]() {
        instance2->run();

        instance2Done.store(true);
    });

    std::thread thread3([&instance3, &instance3Done]() {
        instance3->run();

        instance3Done.store(true);
    });

    thread1.join();
    thread2.join();
    thread3.join();

    EXPECT_TRUE(instance1Done.load());
    EXPECT_TRUE(instance2Done.load());
    EXPECT_TRUE(instance3Done.load());

    EXPECT_DOUBLE_EQ(instance1->progress(), 1.0);
    EXPECT_DOUBLE_EQ(instance2->progress(), 1.0);
    EXPECT_DOUBLE_EQ(instance3->progress(), 1.0);

    EXPECT_FALSE(instance1->hasIssues());
    EXPECT_FALSE(instance2->hasIssues());
    EXPECT_FALSE(instance3->hasIssues());
}

TEST(ConcurrentSedTest, parallelAsyncLifecycle)
{
    static const auto INSTANCE_COUNT {3U};
    static const auto SIMULATION_PROPERTY {10000};

    std::vector<libOpenCOR::SedInstancePtr> instances;

    for (size_t i {0}; i < INSTANCE_COUNT; ++i) {
        auto file {libOpenCOR::File::create(libOpenCOR::resourcePath("cellml_2.cellml"))};
        auto document {libOpenCOR::SedDocument::create(file)};
        const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

        simulation->setNumberOfSteps(SIMULATION_PROPERTY);
        simulation->setOutputEndTime(static_cast<double>(SIMULATION_PROPERTY));

        instances.push_back(document->instantiate());
    }

    for (auto &instance : instances) {
        EXPECT_TRUE(instance->startRun());
    }

    for (auto &instance : instances) {
        EXPECT_GT(instance->waitForRun(), 0.0);
    }

    for (const auto &instance : instances) {
        EXPECT_EQ(instance->status(), libOpenCOR::SedInstance::Status::IDLE);
        EXPECT_FALSE(instance->hasIssues());
    }
}

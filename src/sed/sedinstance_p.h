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

#pragma once

#include "logger_p.h"
#include "sedinstancetask_p.h"

#include "libopencor/sedinstance.h"

#include <condition_variable>
#include <future>
#include <mutex>

namespace libOpenCOR {

class SedInstance::Impl: public Logger::Impl
{
public:
    SedInstanceTaskPtrs mTasks;
    IssuePtrs mTasksIssues;

    mutable std::atomic<bool> mRunning {false};
    mutable std::mutex mRunMutex;
    mutable std::future<double> mRunFuture;
    mutable std::atomic<double> mLastRunElapsedTime {0.0};

    std::atomic<unsigned> mRunControl {INSTANCE_RUN_CONTROL_NONE};

    std::condition_variable mPauseConditionVariable;
    std::mutex mPauseMutex;

    static SedInstancePtr create(const SedDocumentPtr &pDocument);

    explicit Impl(const SedDocumentPtr &pDocument);

    double run();
    bool startRun();
    bool isRunning() const;
    double waitForRun();
    void pauseRun();
    void resumeRun();
    void stopRun();
    double progress() const;

    bool hasTasks() const;
    size_t taskCount() const;
    const SedInstanceTaskPtrs &tasks() const;
    const SedInstanceTaskPtr &task(size_t pIndex) const;
};

} // namespace libOpenCOR

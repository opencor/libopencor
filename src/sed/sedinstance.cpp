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

#include "sedabstracttask_p.h"
#include "sedinstance_p.h"
#include "sedinstancetask_p.h"

#include "libopencor/seddocument.h"

#include <chrono>

namespace libOpenCOR {

namespace {

constexpr auto ZERO_WAIT {std::chrono::milliseconds {0}};

} // namespace

SedInstancePtr SedInstance::Impl::create(const SedDocumentPtr &pDocument)
{
    return SedInstancePtr {new SedInstance(pDocument)};
}

SedInstance::Impl::Impl(const SedDocumentPtr &pDocument)
    : Logger::Impl()
{
    // Check whether there are some outputs that should be generated or, failing that, whether there are some tasks that
    // could be run.
    //---GRY--- WE DON'T CURRENTLY SUPPORT OUTPUTS, SO WE JUST CHECK FOR TASKS FOR NOW.

    if (pDocument->hasTasks()) {
        // Make sure that all the tasks are valid.

        const auto &tasks {pDocument->tasks()};
        auto tasksValid {true};

        for (const auto &task : tasks) {
            auto *taskPimpl {task->pimpl()};

            taskPimpl->removeAllIssues();

            // Make sure that the task is valid.

            if (!taskPimpl->isValid()) {
                addIssues(task, "Task");

                tasksValid = false;
            }
        }

        // Create an instance of all the tasks, if they are all valid.

        if (tasksValid) {
            mTasks.reserve(tasks.size());

            for (const auto &task : tasks) {
                auto taskInstance {SedInstanceTask::Impl::create(task)};

                mTasks.push_back(taskInstance);

                if (taskInstance->hasIssues()) {
                    addIssues(taskInstance, "Task instance");
                }
            }
        }

        // Keep track of the issues of all the tasks.

        mTasksIssues = mIssues;
    } else {
        addError("The simulation experiment description does not contain any tasks to run.");
    }
}

double SedInstance::Impl::run()
{
    // Reset ourselves.

    removeAllIssues();

    mIssues = mTasksIssues;

    // Reset our control flags and make sure that they are passed to each task so that they can be used by them.

    mRunControl.store(INSTANCE_RUN_CONTROL_NONE, std::memory_order_relaxed);

    for (const auto &task : mTasks) {
        task->pimpl()->mRunControl = &mRunControl;

        task->pimpl()->mPauseMutex = &mPauseMutex;
        task->pimpl()->mPauseConditionVariable = &mPauseConditionVariable;
    }

    // Run all the tasks associated with this instance unless they have some issues.

    auto res {0.0};

    for (const auto &task : mTasks) {
        if (!task->hasIssues()) {
            res += task->pimpl()->run();

            if (task->hasIssues()) {
                addIssues(task, "Task");

                // Reset the issues of the task so that they are not reported again should the instance be run again.

                task->pimpl()->removeAllIssues();
            }
        }
    }

    // Reset and make sure that our control flags are no longer passed to each task.

    for (const auto &task : mTasks) {
        task->pimpl()->mRunControl = nullptr;

        task->pimpl()->mPauseMutex = nullptr;
        task->pimpl()->mPauseConditionVariable = nullptr;
    }

    return res;
}

bool SedInstance::Impl::startRun()
{
    const std::scoped_lock<std::mutex> runLock(mRunMutex);

    if (mRunFuture.valid()) {
        if (mRunFuture.wait_for(ZERO_WAIT) != std::future_status::ready) {
            return false;
        }

        mLastRunElapsedTime.store(mRunFuture.get(), std::memory_order_relaxed);
    }

    mRunning.store(true, std::memory_order_release);

    mRunFuture = std::async(std::launch::async, [this]() {
        const auto result = run();

#ifdef __EMSCRIPTEN__
        // Clean up our per-worker WASM runtime data.
        // Note: indeed, each pthread worker maintains its own globalThis.runtime, so without cleanup, compiled
        //       WebAssembly.Module objects would accumulate on reused workers and therefore leak native memory.

        for (const auto &task : mTasks) {
            task->pimpl()->mRuntime->cleanupWorkerWasm();
        }
#endif

        mRunning.store(false, std::memory_order_release);

        return result;
    });

    return true;
}

bool SedInstance::Impl::isRunning() const
{
    return mRunning.load(std::memory_order_acquire);
}

double SedInstance::Impl::waitForRun()
{
    const std::scoped_lock<std::mutex> runLock(mRunMutex);

    if (!mRunFuture.valid()) {
        return mLastRunElapsedTime.load(std::memory_order_relaxed);
    }

    mLastRunElapsedTime.store(mRunFuture.get(), std::memory_order_relaxed);
    mRunning.store(false, std::memory_order_release);

    return mLastRunElapsedTime.load(std::memory_order_relaxed);
}

void SedInstance::Impl::pauseRun()
{
    mRunControl.fetch_or(INSTANCE_RUN_CONTROL_PAUSE, std::memory_order_relaxed);
}

void SedInstance::Impl::resumeRun()
{
    mRunControl.fetch_and(~INSTANCE_RUN_CONTROL_PAUSE, std::memory_order_relaxed);

    mPauseConditionVariable.notify_all();
}

void SedInstance::Impl::stopRun()
{
    mRunControl.fetch_or(INSTANCE_RUN_CONTROL_STOP, std::memory_order_relaxed);

    mPauseConditionVariable.notify_all();
}

double SedInstance::Impl::progress() const
{
    if (mTasks.empty()) {
        return 0.0;
    }

    auto total {0.0};

    for (const auto &task : mTasks) {
        total += task->pimpl()->progress();
    }

    return total / static_cast<double>(mTasks.size());
}

bool SedInstance::Impl::hasTasks() const
{
    return !mTasks.empty();
}

size_t SedInstance::Impl::taskCount() const
{
    return mTasks.size();
}

const SedInstanceTaskPtrs &SedInstance::Impl::tasks() const
{
    return mTasks;
}

const SedInstanceTaskPtr &SedInstance::Impl::task(size_t pIndex) const
{
    static const SedInstanceTaskPtr NO_SED_INSTANCE_TASK_PTR;

    if (pIndex >= mTasks.size()) {
        return NO_SED_INSTANCE_TASK_PTR;
    }

    return mTasks[pIndex];
}

SedInstance::SedInstance(const SedDocumentPtr &pDocument)
    : Logger(new Impl(pDocument))
{
}

SedInstance::~SedInstance()
{
    pimpl()->waitForRun(); // To ensure that the instance is not running before we delete it.

    delete pimpl();
}

SedInstance::Impl *SedInstance::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl);
}

const SedInstance::Impl *SedInstance::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl);
}

double SedInstance::run()
{
    return pimpl()->run();
}

bool SedInstance::startRun()
{
    return pimpl()->startRun();
}

bool SedInstance::isRunning() const
{
    return pimpl()->isRunning();
}

double SedInstance::waitForRun()
{
    return pimpl()->waitForRun();
}

void SedInstance::pauseRun()
{
    pimpl()->pauseRun();
}

void SedInstance::resumeRun()
{
    pimpl()->resumeRun();
}

void SedInstance::stopRun()
{
    pimpl()->stopRun();
}

double SedInstance::progress() const
{
    return pimpl()->progress();
}

bool SedInstance::hasTasks() const
{
    return pimpl()->hasTasks();
}

size_t SedInstance::taskCount() const
{
    return pimpl()->taskCount();
}

const SedInstanceTaskPtrs &SedInstance::tasks() const
{
    return pimpl()->tasks();
}

const SedInstanceTaskPtr &SedInstance::task(size_t pIndex) const
{
    return pimpl()->task(pIndex);
}

} // namespace libOpenCOR

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

#include <atomic>
#include <chrono>
#include <exception>
#include <memory>

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
    } else {
        addError("The simulation experiment description does not contain any tasks to run.");
    }

    // Keep track of the issues so that they can be restored should the instance be run.
    // Note: this includes the issues generated above (e.g., when there are no tasks to run).

    mTasksIssues = mIssues;
    mTasksErrors = mErrors;
    mTasksWarnings = mWarnings;
}

SedInstance::Status SedInstance::Impl::status() const
{
    if (!mRunning.load(std::memory_order_acquire)) {
        return Status::IDLE;
    }

    if ((mRunControl.load(std::memory_order_relaxed) & INSTANCE_RUN_CONTROL_PAUSE) != 0U) {
        return Status::PAUSED;
    }

    return Status::RUNNING;
}

double SedInstance::Impl::run()
{
    // Reset ourselves by restoring the issues of all the tasks.
    // Note: the clearing of mIssues, mErrors, and mWarnings could be done using removeAllIssues(), but this would
    //       result in transiently-empty issues, which could be seen by a reader. So, instead, we just clear and restore
    //       the issues in one go.
    // Note: the issue counts are published with std::memory_order_release so that readers using
    //       std::memory_order_acquire never see an updated count before the corresponding issue vectors are visible.

    {
        const std::scoped_lock<std::mutex> lock(mMutex);

        mIssues.clear();
        mErrors.clear();
        mWarnings.clear();

        mIssues = mTasksIssues;
        mErrors = mTasksErrors;
        mWarnings = mTasksWarnings;

        mIssueCount.store(mIssues.size(), std::memory_order_release);
        mErrorCount.store(mErrors.size(), std::memory_order_release);
        mWarningCount.store(mWarnings.size(), std::memory_order_release);
    }

    // Make sure that our control flags are passed to each task so that they can be used by them.
    // Note: our control flags are reset by our callers (see SedInstance::run() and startRun()) rather than here.
    //       Indeed, when called from startRun(), we are run on a separate thread, i.e. some time after startRun() has
    //       returned. So, if we were to reset our control flags here, a stop or pause requested in between would be
    //       lost.

    for (const auto &task : mTasks) {
        task->pimpl()->mRunControl = &mRunControl;

        task->pimpl()->mPauseMutex = &mPauseMutex;
        task->pimpl()->mPauseConditionVariable = &mPauseConditionVariable;
    }

    // Run all the tasks associated with this instance unless they have some issues.
    // Note: a task may throw an exception (e.g., std::bad_alloc if its results cannot be allocated), in which case we
    //       report it as an error rather than let it escape. Indeed, run() may be called from startRun(), i.e. on a
    //       separate thread, and an escaping exception would leave us in a state from which we cannot recover. Also,
    //       we have no way to trigger such an exception in our tests, hence we ignore our try...catch statement during
    //       code coverage.

    auto res {0.0};

#ifndef CODE_COVERAGE_ENABLED
    try {
#endif
        for (const auto &task : mTasks) {
            if (!task->hasIssues()) {
                res += task->pimpl()->run();

                if (task->hasIssues()) {
                    addIssues(task, "Task");

                    // Reset the issues of the task so that they are not reported again should the instance be run
                    // again.

                    task->pimpl()->removeAllIssues();
                }
            }
        }
#ifndef CODE_COVERAGE_ENABLED
    } catch (const std::exception &exception) {
        addError(std::string("The simulation failed: ") + exception.what() + ".");
    } catch (...) {
        addError("The simulation failed.");
    }
#endif

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

    // Reset our control flags (see the note in run()).

    mRunControl.store(INSTANCE_RUN_CONTROL_NONE, std::memory_order_relaxed);

    mRunning.store(true, std::memory_order_release);

    // Start our run in a separate thread.
    // Note #1: we must be flagged as not running anymore once our run is done, even if run() throws an exception (it
    //          reports a failure as an issue, but it might still throw, e.g., std::bad_alloc when restoring our
    //          issues), hence we use a guard to do so. Otherwise, we would be stuck in RUNNING.
    // Note #2: std::async() may throw an exception (e.g., std::system_error if no thread could be created), in which
    //          case we must also be flagged as not running anymore. We have no way to trigger such an exception in our
    //          tests, hence we ignore our try...catch statement during code coverage.

#ifndef CODE_COVERAGE_ENABLED
    try {
#endif
        mRunFuture = std::async(std::launch::async, [this]() {
            auto resetRunning = [](std::atomic<bool> *pRunning) {
                pRunning->store(false, std::memory_order_release);
            };
            const std::unique_ptr<std::atomic<bool>, decltype(resetRunning)> runningGuard {&mRunning, resetRunning};

            return run();
        });
#ifndef CODE_COVERAGE_ENABLED
    } catch (...) {
        mRunning.store(false, std::memory_order_release);

        throw;
    }
#endif

    return true;
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
    // Note: our control flags must be updated while holding our pause mutex. Otherwise, a paused task could check them
    //       (and find that it is still paused), we could then update them and notify our pause condition variable, and
    //       only then would the task start waiting on our pause condition variable, i.e. it would never be woken up.

    {
        const std::scoped_lock<std::mutex> pauseLock(mPauseMutex);

        mRunControl.fetch_and(~INSTANCE_RUN_CONTROL_PAUSE, std::memory_order_relaxed);
    }

    mPauseConditionVariable.notify_all();
}

void SedInstance::Impl::stopRun()
{
    // Note: see the note in resumeRun().

    {
        const std::scoped_lock<std::mutex> pauseLock(mPauseMutex);

        mRunControl.fetch_or(INSTANCE_RUN_CONTROL_STOP, std::memory_order_relaxed);
    }

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
    : Logger(std::make_unique<Impl>(pDocument))
{
}

SedInstance::~SedInstance()
{
    // Make sure that the instance is not running before we delete it.
    // Note #1: we stop any run before waiting for it since a paused run would otherwise never complete, i.e. we would
    //          wait for it forever.
    // Note #2: run() reports a failure as an issue rather than throw an exception, so waitForRun() should never throw,
    //          but an exception must never escape a destructor (it would result in std::terminate() being called),
    //          hence we make sure of it.

#ifndef CODE_COVERAGE_ENABLED
    try {
#endif
        pimpl()->stopRun();
        pimpl()->waitForRun();
#ifndef CODE_COVERAGE_ENABLED
    } catch (...) { // NOLINT(bugprone-empty-catch)
        // There is nothing more that we can do since we are being deleted.
    }
#endif
}

SedInstance::Impl *SedInstance::pimpl()
{
    return static_cast<Impl *>(Logger::mPimpl.get());
}

const SedInstance::Impl *SedInstance::pimpl() const
{
    return static_cast<const Impl *>(Logger::mPimpl.get());
}

SedInstance::Status SedInstance::status() const noexcept
{
    return pimpl()->status();
}

double SedInstance::run()
{
    // Reset our control flags (see the note in SedInstance::Impl::run()).

    pimpl()->mRunControl.store(INSTANCE_RUN_CONTROL_NONE, std::memory_order_relaxed);

    return pimpl()->run();
}

bool SedInstance::startRun()
{
    return pimpl()->startRun();
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

double SedInstance::progress() const noexcept
{
    return pimpl()->progress();
}

bool SedInstance::hasTasks() const noexcept
{
    return pimpl()->hasTasks();
}

size_t SedInstance::taskCount() const noexcept
{
    return pimpl()->taskCount();
}

const SedInstanceTaskPtrs &SedInstance::tasks() const noexcept
{
    return pimpl()->tasks();
}

const SedInstanceTaskPtr &SedInstance::task(size_t pIndex) const noexcept
{
    return pimpl()->task(pIndex);
}

} // namespace libOpenCOR

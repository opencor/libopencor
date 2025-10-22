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

namespace libOpenCOR {

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

        auto tasks = pDocument->tasks();
        auto tasksValid = true;

        for (const auto &task : tasks) {
            auto *taskPimpl = task->pimpl();

            taskPimpl->removeAllIssues();

            // Make sure that the task is valid.

            if (!taskPimpl->isValid()) {
                addIssues(task);

                tasksValid = false;
            }
        }

        // Create an instance of all the tasks, if they are all valid.

        if (tasksValid) {
            for (const auto &task : tasks) {
                auto taskInstance = SedInstanceTask::Impl::create(task);

                mTasks.push_back(taskInstance);

                if (taskInstance->hasIssues()) {
                    addIssues(taskInstance);
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
    // Reset iourselves.

    removeAllIssues();

    mIssues = mTasksIssues;

    // Run all the tasks associated with this instance unless they have some issues.

    auto res = 0.0;

    for (const auto &task : mTasks) {
        if (!task->hasIssues()) {
            res += task->pimpl()->run();

            if (task->hasIssues()) {
                addIssues(task);

                // Reset the issues of the task so that they are not reported again should the instance be run again.

                task->pimpl()->removeAllIssues();
            }
        }
    }

    return res;
}

bool SedInstance::Impl::hasTasks() const
{
    return !mTasks.empty();
}

size_t SedInstance::Impl::taskCount() const
{
    return mTasks.size();
}

SedInstanceTaskPtrs SedInstance::Impl::tasks() const
{
    return mTasks;
}

SedInstanceTaskPtr SedInstance::Impl::task(size_t pIndex) const
{
    if (pIndex >= mTasks.size()) {
        return {};
    }

    return mTasks[pIndex];
}

SedInstance::SedInstance(const SedDocumentPtr &pDocument)
    : Logger(new Impl(pDocument))
{
}

SedInstance::~SedInstance()
{
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

bool SedInstance::hasTasks() const
{
    return pimpl()->hasTasks();
}

size_t SedInstance::taskCount() const
{
    return pimpl()->taskCount();
}

SedInstanceTaskPtrs SedInstance::tasks() const
{
    return pimpl()->tasks();
}

SedInstanceTaskPtr SedInstance::task(size_t pIndex) const
{
    return pimpl()->task(pIndex);
}

} // namespace libOpenCOR

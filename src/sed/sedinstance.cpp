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
    } else {
        addError("The simulation experiment description does not contain any tasks to run.");
    }
}

void SedInstance::Impl::run()
{
    // Run all the tasks associated with this instance unless they have some issues.

    for (const auto &task : mTasks) {
        if (!task->hasIssues()) {
            task->pimpl()->run();

            if (task->hasIssues()) {
                addIssues(task);
            }
        }
    }
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
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

/*---GRY---
const SedInstance::Impl *SedInstance::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
*/

void SedInstance::run()
{
    pimpl()->run();
}

} // namespace libOpenCOR

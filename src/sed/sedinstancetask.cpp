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

#include "sedinstancetask_p.h"

namespace libOpenCOR {

SedInstanceTaskPtr SedInstanceTask::Impl::create()
{
    return SedInstanceTaskPtr {new SedInstanceTask()};
}

SedInstanceTask::SedInstanceTask()
    : Logger(new Impl())
{
}

SedInstanceTask::~SedInstanceTask()
{
    delete pimpl();
}

SedInstanceTask::Impl *SedInstanceTask::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

const SedInstanceTask::Impl *SedInstanceTask::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}

} // namespace libOpenCOR

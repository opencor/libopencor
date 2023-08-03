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

#include "libopencor/logger.h"

namespace libOpenCOR {

class SimulationWorker;
using SimulationWorkerPtr = std::shared_ptr<SimulationWorker>;

class SimulationWorker: public Logger
{
public:
    ~SimulationWorker();

    SimulationWorker(const SimulationWorker &pOther) = delete;
    SimulationWorker(SimulationWorker &&pOther) noexcept = delete;

    SimulationWorker &operator=(const SimulationWorker &pRhs) = delete;
    SimulationWorker &operator=(SimulationWorker &&pRhs) noexcept = delete;

    static SimulationWorkerPtr create();

private:
    class Impl;

    explicit SimulationWorker();

    Impl *pimpl();
    const Impl *pimpl() const;
};

} // namespace libOpenCOR

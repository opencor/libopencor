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

#include "libopencor/simulationdata.h"

#include "simulationdata_p.h"

namespace libOpenCOR {

SimulationDataPtr SimulationData::Impl::create()
{
    return std::shared_ptr<SimulationData> {new SimulationData()};
}

//---GRY---
#ifndef COVERAGE_ENABLED
bool SimulationData::Impl::isValid()
{
    return true;
}
#endif

SimulationData::SimulationData()
    : Logger(new Impl())
{
}

SimulationData::~SimulationData()
{
    delete pimpl();
}

SimulationData::Impl *SimulationData::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

//---GRY---
#ifndef COVERAGE_ENABLED
const SimulationData::Impl *SimulationData::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}
#endif

//---GRY---
#ifndef COVERAGE_ENABLED
bool SimulationData::isValid() const
{
    return pimpl()->isValid();
}
#endif

} // namespace libOpenCOR

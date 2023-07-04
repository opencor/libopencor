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

#include "libopencor/simulation.h"

#include "simulation_p.h"

#include "libopencor/file.h"

namespace libOpenCOR {

Simulation::Impl::Impl(const FilePtr &pFile)
    : mFile(pFile)
{
    switch (mFile->type()) {
    case File::Type::UNKNOWN_FILE:
        addError("A simulation cannot be created using an unknown file.");

        break;
    case File::Type::CELLML_FILE:
        break;
    case File::Type::SEDML_FILE:
        addError("A simulation cannot currently be created using a SED-ML file.");

        break;
    case File::Type::COMBINE_ARCHIVE:
        addError("A simulation cannot currently be created using a COMBINE archive.");

        break;
#ifndef __EMSCRIPTEN__
    case File::Type::IRRETRIEVABLE_FILE:
        addError("A simulation cannot be created using an irretrievable file.");

        break;
#endif
    }
}

bool Simulation::Impl::supportedFile() const
{
    return mFile->type() == File::Type::CELLML_FILE;
}

Simulation::Simulation(const FilePtr &pFile)
    : Logger(new Impl(pFile))
{
}

Simulation::~Simulation()
{
    delete pimpl();
}

Simulation::Impl *Simulation::pimpl()
{
    return reinterpret_cast<Impl *>(Logger::pimpl());
}

const Simulation::Impl *Simulation::pimpl() const
{
    return reinterpret_cast<const Impl *>(Logger::pimpl());
}

SimulationPtr Simulation::create(const FilePtr &pFile)
{
    return std::shared_ptr<Simulation> {new Simulation {pFile}};
}

} // namespace libOpenCOR

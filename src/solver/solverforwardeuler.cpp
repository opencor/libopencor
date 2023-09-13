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

#include "solverforwardeuler_p.h"

namespace libOpenCOR {

SolverPtr SolverForwardEuler::Impl::create()
{
    return std::shared_ptr<SolverForwardEuler> {new SolverForwardEuler {}};
}

SolverForwardEuler::SolverForwardEuler()
    : Solver(new Impl())
{
    pimpl()->mIsValid = true;
}

SolverForwardEuler::~SolverForwardEuler()
{
    delete pimpl();
}

SolverForwardEuler::Impl *SolverForwardEuler::pimpl()
{
    return reinterpret_cast<Impl *>(Solver::pimpl());
}

/*---GRY---
const SolverForwardEuler::Impl *SolverForwardEuler::pimpl() const
{
    return reinterpret_cast<const Impl *>(Solver::pimpl());
}
*/

} // namespace libOpenCOR

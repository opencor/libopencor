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

#include "utils.h"

#include "libopencor/solver.h"

#include "libxml/tree.h"

namespace libsedml {

class SedAlgorithm;

} // namespace libsedml

namespace libOpenCOR {

class Solver::Impl: public Logger::Impl
{
public:
    std::string mId;
    std::string mName;

    explicit Impl(const std::string &pId, const std::string &pName);

    virtual SolverPtr duplicate() = 0;

    virtual void populate(libsedml::SedAlgorithm *pAlgorithm) = 0;

    std::string id() const;
    std::string name() const;

    void serialise(xmlNodePtr pNode, bool pNlaAlgorithm = false) const;

    virtual StringStringMap properties() const = 0;
};

} // namespace libOpenCOR

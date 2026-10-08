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

#include "seduniformtimecourse_p.h"

#include "utils.h"

#include "libopencor/sedmodel.h"

#include <cmath>
#include <memory>

namespace libOpenCOR {

SedUniformTimeCourse::Impl::Impl(const SedDocumentPtr &pDocument)
    : SedSimulation::Impl(pDocument)
{
}

Strings SedUniformTimeCourse::Impl::validationErrors(const SedModelPtr &pModel) const
{
    // Make sure that our times are finite and such that initialTime <= outputStartTime < outputEndTime, and that our
    // number of steps is strictly positive.
    // Note: we are validated both when instantiating a document (see SedSimulation::Impl::isValid()) and when running
    //       an instance task (see SedInstanceTask::Impl::run()) since our times and number of steps may have been
    //       changed in between.

    const auto &modelId = pModel->id();
    Strings res;

    if (!std::isfinite(mInitialTime) || !std::isfinite(mOutputStartTime) || !std::isfinite(mOutputEndTime)
        || (mInitialTime > mOutputStartTime) || (mOutputStartTime >= mOutputEndTime)) {
        const auto initialTime {toString(mInitialTime)};
        const auto outputStartTime {toString(mOutputStartTime)};
        const auto outputEndTime {toString(mOutputEndTime)};
        std::string error;

        error.reserve(mId.size() + modelId.size() + initialTime.size() + outputStartTime.size() + outputEndTime.size() + 152); // NOLINT

        error += "Simulation '";
        error += mId;
        error += "' is to be used with model '";
        error += modelId;
        error += "' which requires finite times such that initialTime <= outputStartTime < outputEndTime but ";
        error += initialTime;
        error += ", ";
        error += outputStartTime;
        error += ", and ";
        error += outputEndTime;
        error += " are provided.";

        res.push_back(error);
    }

    if (mNumberOfSteps <= 0) {
        const auto numberOfSteps {toString(mNumberOfSteps)};
        std::string error;

        error.reserve(mId.size() + modelId.size() + numberOfSteps.size() + 112); // NOLINT

        error += "Simulation '";
        error += mId;
        error += "' is to be used with model '";
        error += modelId;
        error += "' which requires a strictly positive number of steps but ";
        error += numberOfSteps;
        error += " is provided.";

        res.push_back(error);
    }

    return res;
}

double SedUniformTimeCourse::Impl::initialTime() const
{
    return mInitialTime;
}

void SedUniformTimeCourse::Impl::setInitialTime(double pInitialTime)
{
    mInitialTime = pInitialTime;
}

double SedUniformTimeCourse::Impl::outputStartTime() const
{
    return mOutputStartTime;
}

void SedUniformTimeCourse::Impl::setOutputStartTime(double pOutputStartTime)
{
    mOutputStartTime = pOutputStartTime;
}

double SedUniformTimeCourse::Impl::outputEndTime() const
{
    return mOutputEndTime;
}

void SedUniformTimeCourse::Impl::setOutputEndTime(double pOutputEndTime)
{
    mOutputEndTime = pOutputEndTime;
}

int SedUniformTimeCourse::Impl::numberOfSteps() const
{
    return mNumberOfSteps;
}

void SedUniformTimeCourse::Impl::setNumberOfSteps(int pNumberOfSteps)
{
    mNumberOfSteps = pNumberOfSteps;
}

void SedUniformTimeCourse::Impl::serialise(xmlNodePtr pNode) const
{
    auto *node {xmlNewNode(nullptr, toConstXmlCharPtr("uniformTimeCourse"))};

    SedSimulation::Impl::serialise(node);

    xmlNewProp(node, toConstXmlCharPtr("initialTime"), toConstXmlCharPtr(toString(mInitialTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputStartTime"), toConstXmlCharPtr(toString(mOutputStartTime)));
    xmlNewProp(node, toConstXmlCharPtr("outputEndTime"), toConstXmlCharPtr(toString(mOutputEndTime)));
    xmlNewProp(node, toConstXmlCharPtr("numberOfSteps"), toConstXmlCharPtr(toString(mNumberOfSteps)));

    xmlAddChild(pNode, node);
}

SedUniformTimeCourse::SedUniformTimeCourse(const SedDocumentPtr &pDocument)
    : SedSimulation(std::make_unique<Impl>(pDocument))
{
}

SedUniformTimeCourse::~SedUniformTimeCourse() = default;

SedUniformTimeCourse::Impl *SedUniformTimeCourse::pimpl()
{
    return reinterpret_cast<SedUniformTimeCourse::Impl *>(SedSimulation::pimpl());
}

const SedUniformTimeCourse::Impl *SedUniformTimeCourse::pimpl() const
{
    return static_cast<const Impl *>(SedSimulation::pimpl());
}

SedUniformTimeCoursePtr SedUniformTimeCourse::create(const SedDocumentPtr &pDocument)
{
    return SedUniformTimeCoursePtr {new SedUniformTimeCourse {pDocument}};
}

double SedUniformTimeCourse::initialTime() const
{
    return pimpl()->initialTime();
}

void SedUniformTimeCourse::setInitialTime(double pInitialTime)
{
    pimpl()->setInitialTime(pInitialTime);
}

double SedUniformTimeCourse::outputStartTime() const
{
    return pimpl()->outputStartTime();
}

void SedUniformTimeCourse::setOutputStartTime(double pOutputStartTime)
{
    pimpl()->setOutputStartTime(pOutputStartTime);
}

double SedUniformTimeCourse::outputEndTime() const
{
    return pimpl()->outputEndTime();
}

void SedUniformTimeCourse::setOutputEndTime(double pOutputEndTime)
{
    pimpl()->setOutputEndTime(pOutputEndTime);
}

int SedUniformTimeCourse::numberOfSteps() const
{
    return pimpl()->numberOfSteps();
}

void SedUniformTimeCourse::setNumberOfSteps(int pNumberOfSteps)
{
    pimpl()->setNumberOfSteps(pNumberOfSteps);
}

} // namespace libOpenCOR

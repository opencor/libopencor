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

#include "odemodel.h"

namespace OdeModel {

void run(const libOpenCOR::SedDocumentPtr &pDocument, const libOpenCOR::Doubles &pStateValues,
         const libOpenCOR::Doubles &pStateAbsTols, const libOpenCOR::Doubles &pRateValues,
         const libOpenCOR::Doubles &pRateAbsTols, const libOpenCOR::Doubles &pVariableValues,
         const libOpenCOR::Doubles &pVariableAbsTols, bool pCompiled)
{
    static const auto OUTPUT_END_TIME = 50.0;
    static const auto NUMBER_OF_STEPS = 50000;

    auto simulation = dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(pDocument->simulations()[0]);

    simulation->setOutputEndTime(OUTPUT_END_TIME);
    simulation->setNumberOfSteps(NUMBER_OF_STEPS);

    auto instance = pDocument->instantiate(pCompiled);

    instance->run();

    auto instanceTask = instance->tasks()[0];

    EXPECT_EQ_VALUES(instanceTask, 13000, pStateValues, pStateAbsTols, pRateValues, pRateAbsTols, pVariableValues, pVariableAbsTols);
}

} // namespace OdeModel

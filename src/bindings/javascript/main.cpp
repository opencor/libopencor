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

#include <libopencor>

void fileApi();
void loggerApi();
void sedApi();
void solverApi();
void versionApi();

EMSCRIPTEN_BINDINGS(libOpenCOR)
{
    // Vectors.

    emscripten::register_vector<double>("Doubles");
    emscripten::register_vector<libOpenCOR::FilePtr>("FilePtrs");
    emscripten::register_vector<libOpenCOR::IssuePtr>("IssuePtrs");
    emscripten::register_vector<libOpenCOR::SedAbstractTaskPtr>("SedAbstractTaskPtrs");
    emscripten::register_vector<libOpenCOR::SedChangePtr>("SedChangePtrs");
    emscripten::register_vector<libOpenCOR::SedDataDescriptionPtr>("SedDataDescriptionPtrs");
    emscripten::register_vector<libOpenCOR::SedDataGeneratorPtr>("SedDataGeneratorPtrs");
    emscripten::register_vector<libOpenCOR::SedInstanceTaskPtr>("SedInstanceTaskPtrs");
    emscripten::register_vector<libOpenCOR::SedModelPtr>("SedModelPtrs");
    emscripten::register_vector<libOpenCOR::SedOutputPtr>("SedOutputPtrs");
    emscripten::register_vector<libOpenCOR::SedSimulationPtr>("SedSimulationPtrs");
    emscripten::register_vector<libOpenCOR::SedStylePtr>("SedStylePtrs");
    emscripten::register_vector<std::string>("Strings");

    // APIs.

    loggerApi(); // Note: it needs to be first since it is used by some other APIs.

    fileApi();
    sedApi();
    solverApi();
    versionApi();

    // Make all registered vectors behave like native JavaScript arrays: expose a .length property and [Symbol.iterator]
    // so that for-of loops, spread syntax, and destructuring work without requiring callers to use .size()/.get().
    // Note: the code within the EM_ASM() block is such that it doesn't rely on top-level comma-heavy literals since
    //       those commas can get interpreted as macro argument separators by the C++ preprocessor, which causes
    //       compilation errors.

    // clang-format off
    EM_ASM({
        let vectorNames = 'Doubles|FilePtrs|IssuePtrs|SedAbstractTaskPtrs|SedChangePtrs|SedDataDescriptionPtrs|SedDataGeneratorPtrs|SedInstanceTaskPtrs|SedModelPtrs|SedOutputPtrs|SedSimulationPtrs|SedStylePtrs|Strings'.split('|');

        vectorNames.forEach(function(name) {
            let prototype = Module[name].prototype;

            Object.defineProperty(prototype, 'length', {
                get: function() {
                    return this.size();
                }
            });

            prototype[Symbol.iterator] = function() {
                let i = 0;
                let n = this.size();
                let iterator = {};

                iterator.next = () => {
                    let result = {};

                    if (i < n) {
                        result.value = this.get(i++);
                        result.done = false;
                    } else {
                        result.value = undefined;
                        result.done = true;
                    }

                    return result;
                };

                return iterator;
            };

            Object.setPrototypeOf(prototype, new Proxy(Object.getPrototypeOf(prototype), {
                get: function(target, prop, receiver) {
                    if (typeof prop === 'string' && /^[0-9]+$/.test(prop)) {
                        return receiver.get(parseInt(prop));
                    }

                    return Reflect.get(target, prop, receiver);
                },
                set: function(target, prop, value, receiver) {
                    if (typeof prop === 'string' && /^[0-9]+$/.test(prop)) {
                        receiver.set(parseInt(prop), value);

                        return true;
                    }

                    return Reflect.set(target, prop, value, receiver);
                }
            }));
        });
    }); // clang-format on
}

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

#include "stringconverter.h"

#include <libopencor>

#include <optional>

void fileApi();
void loggerApi();
void sedApi();
void solverApi();
void versionApi();

// Make sure that addOnPostCtor(), stackSave(), and stackRestore() are available to our EM_ASM() blocks (see below).

EM_JS_DEPS(libOpenCOR, "$addOnPostCtor,$stackSave,$stackRestore");

namespace {

void restoreStackOnThrow()
{
    // Make sure that the WebAssembly stack pointer gets restored when a binding throws a JavaScript exception (see Note
    // #4 in stringconverter.h), i.e. a binding that takes a string (see fromJsString()), that may reject a value (see
    // throwRangeError()), or that may fail in the current JavaScript environment (see throwError()), as Embind itself
    // does for a binding that allocates its arguments on the WebAssembly stack.
    // Note #1: a binding is listed as "<class name>:<member name>|<member name>|...", where a member name is that of a
    //          method, a static method, a property (in which case its setter is wrapped), or "constructor" (in which
    //          case the class is wrapped using a Proxy, so that it otherwise behaves as before, e.g., for instanceof).
    // Note #2: this must be done once all the static constructors have been run, i.e. once all the types (including
    //          Embind's builtin ones, e.g., void and bool) have been registered. Indeed, Embind only installs a method
    //          once all its types have been registered, so it would otherwise replace a method that we have wrapped.

    // clang-format off
    EM_ASM({
        let restoringStack = (call) => {
            let sp = stackSave();

            try {
                return call();
            } finally {
                stackRestore(sp);
            }
        };
        let bindings = 'File:constructor|childFileFromFileName FileManager:fileFromFileNameOrUrl SedBase:id|isDuplicateId|isValidId SedChangeAttribute:constructor|componentName|variableName|newValue SedDocument:serialise SedInstance:startRun Strings:push_back|resize|set'.split(' ');

        addOnPostCtor(() => bindings.forEach((binding) => {
            let parts = binding.split(':');
            let className = parts[0];
            let classObject = Module[className];

            if (!classObject) {
                return;
            }

            let prototype = classObject.prototype;

            parts[1].split('|').forEach((memberName) => {
                if (memberName === 'constructor') {
                    Module[className] = new Proxy(classObject, {
                        construct: (target, args, newTarget) => restoringStack(() => Reflect.construct(target, args, newTarget))
                    });

                    return;
                }

                // Note: a static method is a member of the class itself rather than of its prototype.

                let owner = Object.getOwnPropertyDescriptor(prototype, memberName) ? prototype : classObject;
                let descriptor = Object.getOwnPropertyDescriptor(owner, memberName);

                if (descriptor.set) {
                    let setter = descriptor.set;

                    descriptor.set = function(value) {
                        restoringStack(() => setter.call(this, value));
                    };
                } else {
                    let method = descriptor.value;

                    // Note: an overloaded method (e.g., SedDocument.serialise()) is dispatched using its overloadTable
                    //       property, which Embind retrieves from the prototype, so our wrapper must have it too.

                    descriptor.value = Object.assign(function(...args) {
                        return restoringStack(() => method.apply(this, args));
                    }, method);
                }

                Object.defineProperty(owner, memberName, descriptor);
            });
        }));
    });
    // clang-format on
}

} // namespace

EMSCRIPTEN_BINDINGS(libOpenCOR)
{
    // JavaScript string.

    emscripten::register_type<JsString>("string");

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

    // Note: we register our vector of strings ourselves rather than using emscripten::register_vector() so that its
    //       strings get converted using toJsString() and fromJsString() (see stringconverter.h). Otherwise, it behaves
    //       the same as a vector registered using emscripten::register_vector().

    emscripten::register_optional<JsString>();
    emscripten::class_<libOpenCOR::Strings>("Strings")
        .constructor<>()
        // clang-format off
        .function("push_back", emscripten::optional_override([](libOpenCOR::Strings &pThis, const JsString &pValue) {
            pThis.push_back(fromJsString(pValue));
        }))
        .function("size", emscripten::optional_override([](const libOpenCOR::Strings &pThis) {
            return static_cast<unsigned int>(pThis.size());
        }))
        .function("resize", emscripten::optional_override([](libOpenCOR::Strings &pThis, unsigned int pSize, const JsString &pValue) {
            pThis.resize(pSize, fromJsString(pValue));
        }))
        .function("set", emscripten::optional_override([](libOpenCOR::Strings &pThis, unsigned int pIndex, const JsString &pValue) {
            if (pIndex >= pThis.size()) {
                return false;
            }

            pThis[pIndex] = fromJsString(pValue);

            return true;
        }))
        .function("get", emscripten::optional_override([](const libOpenCOR::Strings &pThis, unsigned int pIndex) -> std::optional<JsString> {
            if (pIndex >= pThis.size()) {
                return {};
            }

            return toJsString(pThis[pIndex]);
        })) // clang-format on
        .iterable<JsString>("size", "get");

    // APIs.

    loggerApi(); // Note: it needs to be first since it is used by some other APIs.

    fileApi();
    sedApi();
    solverApi();
    versionApi();

    // Make sure that the WebAssembly stack pointer gets restored when a binding throws a JavaScript exception.

    restoreStackOnThrow();

    // Make all registered vectors behave like native JavaScript arrays: expose a .length property and [Symbol.iterator]
    // so that for-of loops, spread syntax, and destructuring work without requiring callers to use .size()/.get().
    // Note: the code within the EM_ASM() block is such that it doesn't rely on top-level comma-heavy literals since
    //       those commas can get interpreted as macro argument separators by the C++ preprocessor, which causes
    //       compilation errors.

    // clang-format off
    EM_ASM({
        let vectorNames = 'Doubles|FilePtrs|IssuePtrs|SedAbstractTaskPtrs|SedChangePtrs|SedDataDescriptionPtrs|SedDataGeneratorPtrs|SedInstanceTaskPtrs|SedModelPtrs|SedOutputPtrs|SedSimulationPtrs|SedStylePtrs|Strings'.split('|');

        vectorNames.forEach((name) => {
            let vectorClass = Module[name];

            if (!vectorClass) {
                return;
            }

            let prototype = vectorClass.prototype;

            Object.defineProperty(prototype, 'length', {
                get: function() {
                    return this.size();
                }
            });

            prototype[Symbol.iterator] = function*() {
                let n = this.size();

                for (let i = 0; i < n; ++i) {
                    yield this.get(i);
                }
            };

            Object.setPrototypeOf(prototype, new Proxy(Object.getPrototypeOf(prototype), {
                get: (target, property, receiver) => {
                    if (typeof property === 'string') {
                        let number = property >>> 0;

                        if (String(number) === property) {
                            return receiver.get(number);
                        }
                    }

                    return Reflect.get(target, property, receiver);
                },
                set: (target, property, value, receiver) => {
                    if (typeof property === 'string') {
                        let number = property >>> 0;

                        if (String(number) === property) {
                            receiver.set(number, value);

                            return true;
                        }
                    }

                    return Reflect.set(target, property, value, receiver);
                }
            }));
        });
    }); // clang-format on
}

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

#include "unittestingexport.h"

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace llvm {

class LLVMContext;
class Module;

} // namespace llvm

namespace libOpenCOR {

// The target for which we generate some LLVM IR (i.e. what Clang would otherwise have derived from its -cc1 arguments;
// see compiler.cpp).
// Note: everything that depends on the platform is decided by compiler.cpp (at compile time), so that our IR generator
//       doesn't have any platform-specific code path.

struct IrGeneratorTarget
{
    std::string triple;
    std::string dataLayout;
    unsigned int longBits {0}; // The size of long, in bits.
    unsigned int largeArrayMinBits {0}; // The minimum size, in bits, of an array that is to be aligned on...
    unsigned int largeArrayAlignment {0}; // ... this many bits (if not 0).
    std::vector<std::pair<std::string, std::string>> functionAttributes; // For all our functions.
    std::vector<std::pair<std::string, std::string>> unprototypedDeclarationAttributes; // For the functions that we only
                                                                                        // declare, using ().
    std::vector<std::tuple<unsigned int, std::string, unsigned int>> moduleFlags; // An llvm::Module::ModFlagBehavior
                                                                                  // value, a name, and a value.
    unsigned int uwtable {0}; // An llvm::UWTableKind value, for the functions that we define.
    unsigned int definitionVisibility {0}; // An llvm::GlobalValue::VisibilityTypes value.
    bool dsoLocalDefinitions {false}; // Whether the functions that we define are known to be DSO local.
    bool dsoLocalDeclarations {false}; // Whether the functions that we only declare are known to be DSO local.
};

// Generate some LLVM IR for the given code, which must be written in the subset of C that libCellML's generator
// produces with our generator profile (see cellmlfileruntime.cpp). If the code cannot be compiled, then nullptr is
// returned and pError describes the (first) problem that was found, followed by the offending line and a caret pointing
// to where the problem is.

std::unique_ptr<llvm::Module> generateIr(const std::string &pCode, llvm::LLVMContext &pContext,
                                         const IrGeneratorTarget &pTarget, std::string &pError);

// Same as above, but return the LLVM IR as text (or an empty string if the code cannot be compiled), so that our unit
// tests can check the LLVM IR that we generate for any target.

std::string LIBOPENCOR_UNIT_TESTING_EXPORT generateIrText(const std::string &pCode, const IrGeneratorTarget &pTarget,
                                                          std::string &pError);

} // namespace libOpenCOR

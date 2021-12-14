# Copyright libOpenCOR contributors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(PACKAGE_NAME LLVMClang)
set(PACKAGE_VERSION 13.0.0)
set(PACKAGE_REPOSITORY llvm-project)
set(GIT_TAG llvmorg-13.0.0)
set(RELEASE_TAG llvmorg-13.0.0)
set(INSTALL_DIR ${PREBUILT_DIR}/${PACKAGE_NAME})

set(CLANG_LIBRARY lib/libclang${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_ANALYSIS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangAnalysis${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_API_NOTES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangAPINotes${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_AST_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangAST${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_AST_MATCHERS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangASTMatchers${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_BASIC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangBasic${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_CODE_GEN_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangCodeGen${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_CROSS_TU_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangCrossTU${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_DEPENDENCY_SCANNING_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangDependencyScanning${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_DIRECTORY_WATCHERS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangDirectoryWatcher${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_DRIVER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangDriver${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_DYNAMIC_AST_MATCHERS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangDynamicASTMatchers${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_EDIT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangEdit${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_FORMAT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangFormat${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_FRONTEND_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangFrontend${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_FRONTEND_TOOL_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangFrontendTool${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_INDEX_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangIndex${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_INDEX_SERIALIZATION_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangIndexSerialization${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_INTERPRETER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangInterpreter${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_LEX_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangLex${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_PARSE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangParse${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_REWRITE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangRewrite${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_REWRITE_FRONTEND_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangRewriteFrontend${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_SEMA_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangSema${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_SERIALIZATION_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangSerialization${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_ANALYZER_CHECKERS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangStaticAnalyzerCheckers${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_STATIC_ANALYZER_CORE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangStaticAnalyzerCore${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_STATIC_ANALYZER_FRONTEND_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangStaticAnalyzerFrontend${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TESTING_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangTesting${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TOOLING_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangTooling${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TOOLING_AST_DIFF_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangToolingASTDiff${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TOOLING_CORE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangToolingCore${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TOOLING_INCLUSIONS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangToolingInclusions${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_REFACTORING_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangToolingRefactoring${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TOOLING_SYNTAX_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangToolingSyntax${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CLANG_TRANSFORMER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}clangTransformer${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_AGGRESSIVE_INST_COMBINE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMAggressiveInstCombine${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ANALYSIS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMAnalysis${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ASM_PARSER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMAsmParser${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ASM_PRINTER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMAsmPrinter${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_BINARY_FORMAT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMBinaryFormat${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_BIT_READER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMBitReader${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_BITSTREAM_READER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMBitstreamReader${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_BIT_WRITER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMBitWriter${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_CF_GUARD_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCFGuard${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_CFI_VERIFY_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCFIVerify${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_CODE_GEN_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCodeGen${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_CORE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCore${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_COROUTINES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCoroutines${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_COVERAGES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMCoverage${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEBUG_INFO_CODE_VIEW_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDebugInfoCodeView${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEBUG_INFO_DWARF_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDebugInfoDWARF${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEBUG_INFO_GSYM_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDebugInfoGSYM${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEBUG_INFO_MSF_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDebugInfoMSF${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEBUG_INFO_PDB_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDebugInfoPDB${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DEMANGLES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDemangle${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DLLTOOL_DRIVER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDlltoolDriver${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DWARF_LINKER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDWARFLinker${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_DWP_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMDWP${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_EXECUTION_ENGINE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMExecutionEngine${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_EXEGESIS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMExegesis${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_EXEGESIS_X86_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMExegesisX86${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_EXTENSIONS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMExtensions${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_FILE_CHECK_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMFileCheck${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_FRONTEND_OPEN_ACC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMFrontendOpenACC${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_FRONTEND_OPENMP_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMFrontendOpenMP${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_FUZZ_MUTATES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMFuzzMutate${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_GLOBAL_ISELF_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMGlobalISel${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_INST_COMBINE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMInstCombine${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_INSTRUMENTATION_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMInstrumentation${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_INTERFACE_STUB_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMInterfaceStub${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_INTERPRETER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMInterpreter${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_IPO_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMipo${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_IR_READER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMIRReader${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_JIT_LINK_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMJITLink${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_LIB_DRIVER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMLibDriver${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_LINE_EDITOR_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMLineEditor${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_LINKER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMLinker${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_LTO_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMLTO${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMC${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MCA_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMCA${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MC_DISASSEMBLER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMCDisassembler${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MC_JIT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMCJIT${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MC_PARSER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMCParser${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_MIR_PARSER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMMIRParser${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_OBJ_CARC_OPTS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMObjCARCOpts${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_OBJECT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMObject${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_OBJECT_YAML_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMObjectYAML${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_OPTION_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMOption${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ORC_JIT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMOrcJIT${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ORC_SHARED_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMOrcShared${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_ORC_TARGET_PROCESS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMOrcTargetProcess${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_PASSES_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMPasses${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_PROFILE_DATA_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMProfileData${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_REMARKS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMRemarks${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_RUNTIME_DYLDS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMRuntimeDyld${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_SCALAR_OPTS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMScalarOpts${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_SELECTION_DAG_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMSelectionDAG${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_SUPPORT_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMSupport${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_SYMBOLIZE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMSymbolize${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_TABLE_GEN_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMTableGen${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_TABLE_GEN_GLOBAL_ISEL_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMTableGenGlobalISel${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_TARGET_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMTarget${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_TEXT_API_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMTextAPI${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_TRANSFORM_UTILS_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMTransformUtils${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_VECTORIZE_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMVectorize${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_WINDOWS_MANIFEST_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMWindowsManifest${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_X86_ASM_PARSER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMX86AsmParser${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_X86_CODE_GEN_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMX86CodeGen${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_X86_DESC_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMX86Desc${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_X86_DISASSEMBLER_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMX86Disassembler${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_X86_INFO_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMX86Info${CMAKE_STATIC_LIBRARY_SUFFIX})
set(LLVM_XRAY_LIBRARY lib/${CMAKE_STATIC_LIBRARY_PREFIX}LLVMXRay${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SHA1_FILES
    ${CLANG_LIBRARY}
    ${CLANG_ANALYSIS_LIBRARY}
    ${CLANG_API_NOTES_LIBRARY}
    ${CLANG_AST_LIBRARY}
    ${CLANG_AST_MATCHERS_LIBRARY}
    ${CLANG_BASIC_LIBRARY}
    ${CLANG_CODE_GEN_LIBRARY}
    ${CLANG_CROSS_TU_LIBRARY}
    ${CLANG_DEPENDENCY_SCANNING_LIBRARY}
    ${CLANG_DIRECTORY_WATCHERS_LIBRARY}
    ${CLANG_DRIVER_LIBRARY}
    ${CLANG_DYNAMIC_AST_MATCHERS_LIBRARY}
    ${CLANG_EDIT_LIBRARY}
    ${CLANG_FORMAT_LIBRARY}
    ${CLANG_FRONTEND_LIBRARY}
    ${CLANG_FRONTEND_TOOL_LIBRARY}
    ${CLANG_INDEX_LIBRARY}
    ${CLANG_INDEX_SERIALIZATION_LIBRARY}
    ${CLANG_INTERPRETER_LIBRARY}
    ${CLANG_LEX_LIBRARY}
    ${CLANG_PARSE_LIBRARY}
    ${CLANG_REWRITE_LIBRARY}
    ${CLANG_REWRITE_FRONTEND_LIBRARY}
    ${CLANG_SEMA_LIBRARY}
    ${CLANG_SERIALIZATION_LIBRARY}
    ${CLANG_ANALYZER_CHECKERS_LIBRARY}
    ${CLANG_STATIC_ANALYZER_CORE_LIBRARY}
    ${CLANG_STATIC_ANALYZER_FRONTEND_LIBRARY}
    ${CLANG_TESTING_LIBRARY}
    ${CLANG_TOOLING_LIBRARY}
    ${CLANG_TOOLING_AST_DIFF_LIBRARY}
    ${CLANG_TOOLING_CORE_LIBRARY}
    ${CLANG_TOOLING_INCLUSIONS_LIBRARY}
    ${CLANG_REFACTORING_LIBRARY}
    ${CLANG_TOOLING_SYNTAX_LIBRARY}
    ${CLANG_TRANSFORMER_LIBRARY}
    ${LLVM_AGGRESSIVE_INST_COMBINE_LIBRARY}
    ${LLVM_ANALYSIS_LIBRARY}
    ${LLVM_ASM_PARSER_LIBRARY}
    ${LLVM_ASM_PRINTER_LIBRARY}
    ${LLVM_BINARY_FORMAT_LIBRARY}
    ${LLVM_BIT_READER_LIBRARY}
    ${LLVM_BITSTREAM_READER_LIBRARY}
    ${LLVM_BIT_WRITER_LIBRARY}
    ${LLVM_CF_GUARD_LIBRARY}
    ${LLVM_CFI_VERIFY_LIBRARY}
    ${LLVM_CODE_GEN_LIBRARY}
    ${LLVM_CORE_LIBRARY}
    ${LLVM_COROUTINES_LIBRARY}
    ${LLVM_COVERAGES_LIBRARY}
    ${LLVM_DEBUG_INFO_CODE_VIEW_LIBRARY}
    ${LLVM_DEBUG_INFO_DWARF_LIBRARY}
    ${LLVM_DEBUG_INFO_GSYM_LIBRARY}
    ${LLVM_DEBUG_INFO_MSF_LIBRARY}
    ${LLVM_DEBUG_INFO_PDB_LIBRARY}
    ${LLVM_DEMANGLES_LIBRARY}
    ${LLVM_DLLTOOL_DRIVER_LIBRARY}
    ${LLVM_DWARF_LINKER_LIBRARY}
    ${LLVM_DWP_LIBRARY}
    ${LLVM_EXECUTION_ENGINE_LIBRARY}
    ${LLVM_EXEGESIS_LIBRARY}
    ${LLVM_EXEGESIS_X86_LIBRARY}
    ${LLVM_EXTENSIONS_LIBRARY}
    ${LLVM_FILE_CHECK_LIBRARY}
    ${LLVM_FRONTEND_OPEN_ACC_LIBRARY}
    ${LLVM_FRONTEND_OPENMP_LIBRARY}
    ${LLVM_FUZZ_MUTATES_LIBRARY}
    ${LLVM_GLOBAL_ISELF_LIBRARY}
    ${LLVM_INST_COMBINE_LIBRARY}
    ${LLVM_INSTRUMENTATION_LIBRARY}
    ${LLVM_INTERFACE_STUB_LIBRARY}
    ${LLVM_INTERPRETER_LIBRARY}
    ${LLVM_IPO_LIBRARY}
    ${LLVM_IR_READER_LIBRARY}
    ${LLVM_JIT_LINK_LIBRARY}
    ${LLVM_LIB_DRIVER_LIBRARY}
    ${LLVM_LINE_EDITOR_LIBRARY}
    ${LLVM_LINKER_LIBRARY}
    ${LLVM_LTO_LIBRARY}
    ${LLVM_MC_LIBRARY}
    ${LLVM_MCA_LIBRARY}
    ${LLVM_MC_DISASSEMBLER_LIBRARY}
    ${LLVM_MC_JIT_LIBRARY}
    ${LLVM_MC_PARSER_LIBRARY}
    ${LLVM_MIR_PARSER_LIBRARY}
    ${LLVM_OBJ_CARC_OPTS_LIBRARY}
    ${LLVM_OBJECT_LIBRARY}
    ${LLVM_OBJECT_YAML_LIBRARY}
    ${LLVM_OPTION_LIBRARY}
    ${LLVM_ORC_JIT_LIBRARY}
    ${LLVM_ORC_SHARED_LIBRARY}
    ${LLVM_ORC_TARGET_PROCESS_LIBRARY}
    ${LLVM_PASSES_LIBRARY}
    ${LLVM_PROFILE_DATA_LIBRARY}
    ${LLVM_REMARKS_LIBRARY}
    ${LLVM_RUNTIME_DYLDS_LIBRARY}
    ${LLVM_SCALAR_OPTS_LIBRARY}
    ${LLVM_SELECTION_DAG_LIBRARY}
    ${LLVM_SUPPORT_LIBRARY}
    ${LLVM_SYMBOLIZE_LIBRARY}
    ${LLVM_TABLE_GEN_LIBRARY}
    ${LLVM_TABLE_GEN_GLOBAL_ISEL_LIBRARY}
    ${LLVM_TARGET_LIBRARY}
    ${LLVM_TEXT_API_LIBRARY}
    ${LLVM_TRANSFORM_UTILS_LIBRARY}
    ${LLVM_VECTORIZE_LIBRARY}
    ${LLVM_WINDOWS_MANIFEST_LIBRARY}
    ${LLVM_X86_ASM_PARSER_LIBRARY}
    ${LLVM_X86_CODE_GEN_LIBRARY}
    ${LLVM_X86_DESC_LIBRARY}
    ${LLVM_X86_DISASSEMBLER_LIBRARY}
    ${LLVM_X86_INFO_LIBRARY}
    ${LLVM_XRAY_LIBRARY}
)

# Either retrieve or build our package.

if(LIBOPENCOR_PREBUILT_LLVMCLANG)
    if(WIN32)
        if(RELEASE_MODE)
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             30332d621da2ba8dc7917937686441ed0af748fe
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 7e44ecbf289cb0d892266e8cfdf4366266890a58
                                         e8b7fbf78e00896b0cb04a8b1385692b25c824da
                                         e7ce35c76a807bbb2ec2084f64f7983b813846c1
                                         0bbe8fdf1b1fad94c60731cbd7ea9bccfc8090e9
                                         f75f3ac2ff29d99a86e4bedea1cd069e6c09a24d
                                         3e1213731d0630bed1d9406307a2b1b000a31c2e
                                         f1c8997931e6b5f8abd081efc2c9d02a99069f6b
                                         e1c9b3bea763442ecd4db14e6ca9ca0e6190161f
                                         497006a1847477d8e698ad110995382621db811b
                                         a7977a0a19de5c95c6d8b823be5139f9ed2cdf71
                                         df942d14039a589945aea392494257d945938f17
                                         159f0708fd66d6b6d4d50addea575d76c16d1bbd
                                         85158326920a8405d96c88c0ffda51dfbea31f19
                                         39c3407515714c3548548b51e0bfbd3963ec8150
                                         37abc204646e1605bac2b95429d2195413b9372c
                                         8b50fd9d5fc650c5ac58f55175bb23026943eb21
                                         f76a49634e88872412821dd7ef3aacf3de6f87fb
                                         13d5bf0b08069d30bf2dcee74b2966292b3755f2
                                         4ced5157a1b0ffc2260b1761c90e17d8375d18d9
                                         bf252982e8cc133b99e92d2016f15ac962e51633
                                         dee4aa5bfbf88c5870715d9cf65afb7256bdcfc6
                                         c938dbe9d1efb4c674d067bc8b5452f7aa204fda
                                         d1482c681373953c03aaa75954c78a03336ee552
                                         181b106f2f0d628e5cf20d5aef696b7fbb9b07fa
                                         7f4f7aecddb0d04505c05d63678bda661c244fd2
                                         edb7bfeed9ef470b06c8fdd823c69ce77843b675
                                         caf8df45867322efcbbcc2936e44b9eb632034b1
                                         2693c61c97c6863ebf41f53432a0271f927ba999
                                         6f6f59ce6fcb6b180d6fac7ce493f36360236ce3
                                         c9f6399d39637b5be2679410573ce5005a611aec
                                         992e4330b322b01006d1bf19a2635568fe185238
                                         20a68169fb0fe29584ea64a3ef459e7bb93e8c0b
                                         a4881294ccb2cb7a8a218b80f89a32abec7ca31b
                                         2b035ccbb877b24bf3924632b28c09b7333e6571
                                         7dd6483d25630f1198ddb0bd61aa1c2ff71324bf
                                         081e6ad8b8ebf215d19fcb235bce00b98bd8047d
                                         78c6257f753b2017f6ce7f548fbe5ef8c27fdc01
                                         a076450975bed983b2197c7b0d6117ff82349c20
                                         8024489796e820cbc18357f1639ba090502282d1
                                         44dabe360f8945d88cc7c1d5f80c746a0878d7f3
                                         66229a718cd8afbafaff0d69a6d0ee162dad1173
                                         0bce3cd5d09586b1c14e4d48db2b9c392489092a
                                         306be8eb300b312e9f474aa53a610dafcba9d3dc
                                         90e20ba9bd4ad8ff69f789e7d188f700ff191596
                                         0bbab2bf1c94e92ef142becc60884ece4d45f88d
                                         9277e0b900f2f463152d03ab365b9e35655bceed
                                         da04270558db3edbfadc2ea11fdb8fe86e98a120
                                         feec9b5e15dba7cd783267a3a61e7144b560bd2c
                                         04e5a55ae16c18622c7258ef0669791536d78bb0
                                         4c65e5c14eddcdc4edcde30f97a03df2ad4ad7b7
                                         83662d7410b25fdc83b01db38c87a0aaf63b4461
                                         6fdcb4c25143703265f21ee5e7cfd1efec2a0db2
                                         8c5b16468ded491e04945159f0405a1c5a393597
                                         a15c131e4dcbb2d5f3db26f1c616926cf0524d99
                                         addef973591c5a9cedd088c1a451566266e2d4da
                                         032b313bfc345c096965a18a5f483d325cfe74a7
                                         0c1483745f7b54be7f7b265dbd9619af5903538b
                                         b82e6d4e87b6f909c0e68c23f72cc6fe74e7c34b
                                         a01a65cb923196466160be468c47a918b1130987
                                         02af7a0208b63598861e177f4fec930809b10e47
                                         b8d3507608baf3079bda5ab513893b919871f0bb
                                         351c4693426353b008140ba35a6b3469254ca2ff
                                         7e7acb106e49b7b7fe3949b3d7d206c6f5728b99
                                         b4c67b6c2eb2ff2a7370ecdcb7890e987281118f
                                         ca7c8841623329c68a474f9c8d7967faadc4f671
                                         b0a279cb9e3b61cc4bf4d41ddfd978353ee903ce
                                         7f4660be2694e08a149f8d33ebbec7f3ddce0f50
                                         94ecbb6acb3172c6383287a7af55557d570e5eeb
                                         600ce8ebee9d8c5f47d8d974fcaa2275cc1d49cf
                                         9fcac5d4dd3eb4ab30d39089964cd4dde69c4432
                                         049a1eeb14831ff78c26a43f3dd6b6f8e04b3616
                                         84f818127cd9ffdb545c3c1c29b63f3cba86574e
                                         24f14860069a17ddf250f07c294ed65978d664c2
                                         e00a3ee3cfeaef158b17dd79326056f7071303bf
                                         47ceb0415a2459d3ef0e7d072f476717e167bb28
                                         8afc9cffa2d3d67a37cbdee588994cd2e2a44d5b
                                         34d57800667b6f123b58d16043687529b78bf4fa
                                         3fa9308fe4a4323529da38001a718f2acf5785b5
                                         763f0ee58faee610e075b51982f0d7f7a00f6167
                                         5a4e13e2ba98598528edf7f3c48b8cb1c7d7dda3
                                         d08149ee080ba8abe32ea80b613541933ae53b69
                                         f6ee0df4ebb7f03151926a8b949dd11cdc24fa54
                                         7edf41f7bdd271093fed8ebdcfac6bc67b44101c
                                         502536f3a71fea1a9cb4b365df04e4edce8b8012
                                         9f2c6d9d169b3b057fb30831ce2dd4ed19968923
                                         61737b3f6118de20f7e6151af5f5e568ac71bc02
                                         28367bbb1603266a748617c0f67e711dfc88d06f
                                         396c9aebdac400c72eb4de8dcd15a6c4ff8ad59a
                                         1b3701e056a351cf8b7b02200c0bde80c0208ae6
                                         157b8d1ba099072b99195fef5a7a6fe4bd6c05b5
                                         3bcb1088c96be65a475c759d4601604069a45655
                                         538ebe1d0d72552c2b3e6dd3253554082d7a8dc9
                                         a6e2e0656d996331865a696c6139cc914a99c230
                                         f1f6095aee1f16a8df8996dfaeb1e4dfb47cadc7
                                         b0b95a90a32cdf44a045b9c3d68481c8b1caa2fb
                                         a62701e5e10eb64e9819e75544d78563f9de9e64
                                         9f947f6556b7eb7404046627de7d537509df6588
                                         68c0c2984b83dce5eadf99b3e2a81cf82509f518
                                         3ae3b68c30be16f2750452445155680f563d7b00
                                         e53031b04862b808c45cb3b8a14a213bb82e616a
                                         921a33d68213fe6e327ed565c119a08cd55a1bb4
                                         3d1ab2edbb69d83cf19c1e6b997f09e308c2d64a
                                         f87f01e7dc5f028b00284ece5f2403e75b3ff3b4
                                         8d9922d366658d49708a0fc52d905ac7d4571fa6
                                         ec956df2c1f4d795cc15d88a4d9b4f56711393c1
                                         25e23c8d2714ab6da6174275e1b34abe131f5c99
                                         2c890ee8e0b88820d9d69d3113680a71596e5aae
                                         2beda73c286e0a9604a2dcee1fe93e85a402e9bc
                                         97c4b2209f86ce6c0c088e4da0983b9522cd5086
                                         9b408660942e5e9abd9c7bb9c396a0955652f62f
                                         13efd728b8deb0d019f1395e18d3b2152030824e
                                         4b90ffaeba707c3a41192a2accee1aff0992a2f2
                                         9f0bc16a8ea598d861918e44da5d89b4c0f27ff5)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             2fd5ba7cb19cae3845e276677d31eaff9cd63ac7
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 7e44ecbf289cb0d892266e8cfdf4366266890a58
                                         1613adb366aaec390372ef2e75fbad52ff113fa8
                                         cb31924609d5fc5666a5ba6dc8a7110694a33f4c
                                         890fd36d634df205b7860b707e30be7e258c5db3
                                         4c935a6a7ef49a01bdecb2443a7cb10c0a5899f8
                                         52dc78cabc726cb5f53b12c3d748d3b19013a1fc
                                         f62d9682c8712f26da256645ca950ae63a780b1c
                                         29a177bc7d24ec2c862c0e50bd286b91577c7e16
                                         0b3e5c17bf0c1132e07b025814d95aba3946c509
                                         b9acd95950957d4d3d8e253708ad438927ed8e8e
                                         5740a45ed805d2b8fc1feb8ffbcb5fd6ae6d0489
                                         f61bc7abefacc86e723ae715a9d0f4485df6e5a5
                                         cb6795be4c0f1937fff99171286d6b04b6db0d15
                                         dbdb18cad83d9d45d5528498c196943b16ccf031
                                         3feab77886bd5f58e9daa366f4408e13e498f251
                                         e54e30751c7964d15683b8fb2adaacc8cc05b230
                                         f10eec484ce3497c9a7dd8f3eb061b9cec9671fc
                                         e1534cf4381349c60beabc89554f796cd10d4e70
                                         b1f7ff9874065749d1f13bad2a5ea35028c9b7cb
                                         fa1fd2fc7bb6c30d0e4dfdb053e7117a6e4b87ac
                                         11d48f865301bc20212fa59ab9be161991e952a6
                                         9270177881ac5a5aea4d69ce748f795b4704ff31
                                         c2dd7dc8991aed41990ac25558e6480a84e612f8
                                         555b4b46451d1d7420af7e6acdebd53c617a1247
                                         6307b64abe831a6f38dc645827a9ca5789aa53ab
                                         13a1e0f97b2e5a97ac852d3b3c29e076d4464b87
                                         fcef1b0cf74ae710be29445f83c3a4359a9f0b55
                                         61b01cf8bf7c5cebdf75052a4cd16f5f2f079194
                                         339bf6bf912a243ccf2dcaf777a5f18633964c9f
                                         988e529a181e5df8033f7d5e8355f0d3bd5ae435
                                         d594163dbc62304f25a681a7fb6e9c310375b3ac
                                         e2a91434475e9ada53f5b7e3eadbc2ad72b688fc
                                         7cb9d5cee8a1ef74cee52c0d4fa5717e6e002130
                                         68b2332fb229a35b8153018b25109a697ab263db
                                         204a4c44065e475a454415640f4365412907d9e2
                                         d7a829f3a71d933dee03b83b80bd63c78e5b029c
                                         97ce31ef41555f6d63f169c32e45daa3f4654d68
                                         c1564e3a461f831209c8aea14fb606415eb94190
                                         b3263ca54f8169a99b4f8fcc0eec08db58979f41
                                         e1a5ca9fc7e01009b7c43d0fcf52cbe6d2d44a0f
                                         54daae5cc64796dedba9a838d036b94d8060d368
                                         74d2e54a6be8af7dcb381c56c6b3b23f930c457a
                                         8b302a06007dee564577d1fb87f7c3baced0fb91
                                         da72447c23af9a091e6abd9f92dc20bb2a4903ef
                                         327b4f5593c6140bc6e9bc4d7964631ee77d5f7a
                                         77eb03b2723e46af2617c1fb1ca35772c60bc334
                                         6ba52c214ea14f982c345ef70438baefe6421932
                                         d55ba508851f3ca614218da33538bfb1d0ab61e1
                                         6f17f6a893a259a20a8f755eb1e962d2bb4a5ec6
                                         f194cf0ba3b3c67ef536da2de656dadb4ef57342
                                         263e7a15caa2d1e1ee24b2b89575761b5c26faeb
                                         aef06568fbb0ba220740339a30066e003f75d5ca
                                         ce0db6df4ff0963e034e831a1404e2bd411b7540
                                         559e1a7522cdf292be6f88aec7c8d49cee166aa0
                                         85dd8399561c6064de42a4dc9b204a8605ca0971
                                         2c4e39a14cf1fe770393542ee9b0a6eb70c738e2
                                         392f08d5e65282a806d81c127f228d4b373b6ed3
                                         613a8039165acb9f37c88341d97fc6718bbc4d8d
                                         ffe1d02670741f836316178b3a1e85fdd13283d2
                                         b85a87f9f42e759ec3e3a4b2c656c9e810e0b16d
                                         a2a4e9f0299cbc0ffd53b8b45fb13bac7226a664
                                         71bf32b80484c06df60a1d5276cd730a377021ce
                                         edb1d2ed38ed375f5415f2d4f0118f4a0a45bc60
                                         c76c2f3026f378bd2f209d015d33a714bb12fd82
                                         cb68b7683d4970f5802efb6522f5bc6f5b612bf8
                                         3f1cd01ee3dec0895a9dffe1351f825a19d4567e
                                         288132a879c590bfcc41e832dc289220680af9f4
                                         81967aa6e08d03ad710c4120978230769cbad3ad
                                         3dc54de4d06bb0e66e5b7763fff7fd0b34bdee67
                                         0296ea535fce25144507c09ac146025d811df87f
                                         aaef613284194c4aec6492c1b38a49d5b3f50a5e
                                         00102fa3287de1724712e11fac80f7b6e059cbb0
                                         a0943edbba578f42fdfd13fd1411a14446b4a3ea
                                         21a3c32ec07d34755fa039899bbd7faf355701bb
                                         7a86d617aea1886f859c05dd96f056961a214165
                                         93f3e9f2540e1443c5fd8315c7deca1e1efa9e31
                                         59d8079b23cea5d97e84748a38bb6e9fb70a8f0b
                                         3d3f23d4c19271866c8d6455a3f8755c6572866f
                                         7cdc530b715f7dec0ed6ee8b42bd796bb6581233
                                         83d908e640d84decb480073a662f291093ba1dd5
                                         c559e501e2481a9962630fc8e6bdfd90572434ff
                                         fc9f1d04c2dcdf4024d59b56d6bff5e3cd380d17
                                         09ddddea173f25952081082a770ad3363855caba
                                         cdf8c6f27a43d4a48c39a2a9e021e238b6b272a2
                                         02cf61d97c6ee75f225d727e08beede61ffedaaf
                                         f273d918832cc2bf5986c5eab09dfc10b9d826db
                                         1994e8db58627cbfb639ee9b245a75460446e12d
                                         0c727d5d54987e7e8f22112004e8fc1823077173
                                         8a1a5f141f7eebe107e7bb8186419eba8ed348d4
                                         39b67a13d9b19e9c9b93883e0151b96614950225
                                         6cc12e3f614cb6d250a72fa0554f2e24a01fd5e4
                                         6c4e07a96ca7f8523659678b36a23626190b8d80
                                         b76de6a0ded44922bbb80d90d84953af38872f26
                                         0a59f0ffa0c25532e188fa7303df6f91537d9fa1
                                         a4b3b8e7da3f86d73734bef48c367d5348210529
                                         f04c5f60a16f18eec145af02c2f13dbcd9f48660
                                         940d025dc7954b20449c63999fa94f42cb2f01e6
                                         051afbe55a3787107a279a763366e85c10f0a1d8
                                         8df4514f03db7731b15c8dc93d20143d4fc28d6f
                                         2f58ea20a604c4ceea98866b9990b46f1693a5ce
                                         71a2a99fc1e0f4d296658264a05b68fc207c5708
                                         393cde28a6ba8cf4c597bbc3508e9ae54ebf7e72
                                         5bddac26100169666955a210a8f2074211b63446
                                         a5892da4d3e0c0e04440cbe62c79ed4f294b2479
                                         4ae94c36756fe3ddb21e196d4a1a2fc7c8d2e714
                                         f3c1fd08bf7df4e0f1b150550ad0ed03152dbe3a
                                         44e06779b115d9ba279ef042f86cc34612fb7e48
                                         07be4dea516bcbba2772f7931fa9a37a8ae5a414
                                         4fbd9ec1a7ecb1fe53c5e957b4cba64d80d14e11
                                         55656b809723916b18659326b40ba3b6a21817d7
                                         8c8525b1cce9c600dc9e128e7b54b87dc2725adb
                                         2e73d292c5d76bb63d877deca64aed87cadc3805
                                         91e31e6fa56eb6b2e988b7f52b2b3bd686cabc3f)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         5974c8276b7492ab449622acc4fad9122cecfdcd
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 15188f8e9491150bbbb944421cd4b2123f0edaf4
                                     e0b8e3956aad561185f7b2f717308a3ee9efcc1d
                                     5d438020e33b3cca30f8731c1369b9f0a889a49e
                                     df27ee18ca74de862dd3f2b0251620bb61f8aba0
                                     a0df8a5c0adde5fe29e97c91a8f9e2cca4af0146
                                     ce0a354b98e5c210157eaa183c30b0c339e6a684
                                     8ea6e39f0c439709dd40832a3a8b2fbe3ac44a53
                                     2c31a2655eae34d8e5d3136dcbe87b9c6c1677bf
                                     32170e474825918d44c6adb06030249ae6723e59
                                     d8009a8f1d76fa6a34c4594ff318ac3bd9e546a7
                                     96be34b175df5253358569626d3b1e174912aac0
                                     bb485bac1d35e8f2efd7331007c3455e2317dd35
                                     e19541630ced00b09c368dbbd3b4465e830d1b6d
                                     91d05a425fe13f0f9968d94a3890b4ebf7b74dba
                                     5800dcfc75cd1deadcae6705b64ecea9154801e7
                                     8c96b11a5022d3a91122f1ca83be7724820739fd
                                     89446d667e94526069205e8e4d797a0d69fa7213
                                     a71ece0785a78823eece3ec014d8601820944b42
                                     76949b1d21930243542ebd186bbd6c3b430c901f
                                     924afbcc4a9e55ba65f05ee8dafc51ee54877afd
                                     18c55f8529aa3151b497ab3e4538a9b2180124b4
                                     8088fe40dc9362600e38a5ce566e5317538d9484
                                     03acc3e0daba46571cdb91f58b8e19cab0b2e7a1
                                     688dc459326e4388f642016685d2a89b0c81a931
                                     38ad750af6b3af558867113db009f5b0f7bc6776
                                     fba630c1e8bea26336af1af8ce07a56fa94f51e3
                                     d1ea5d91a9b6bd5391565510c975a3818d7ca119
                                     af70b46d4aa6e0a58d0425d78a7f32090d26e56b
                                     ade205257957a9c645cdf1f2bb260e1bb6ae176e
                                     a5049929e7f99f22efbcce99426c3dc751376d92
                                     05d27a4d76fb90cb48ca62e86d5a1eca38bc4afb
                                     d3c82e6edd85cc3c4ef7b9c57b88eb9e7868ba13
                                     1209d847ab0e4cdf4be18a292d300169ff557d21
                                     1b8691916a7e651151155fe55c736fd3948a7023
                                     82aed1faa4efb56a60b981474f38ff1efb352b60
                                     ae441a616fa2b9cf848e56a94acddfaa01d37bd8
                                     f5668d022ada0ad866e9392cd6b79116cabd7e10
                                     0550f8a98b5598d166e293fee86fa70e68ed669e
                                     410f11764b8adb4c09f095f580c4785ecab1e2d9
                                     efdc89c47f245d407e03f6a206a912279e93278c
                                     81d0545a28960042d9fd76f6e9c8c20fc303c979
                                     9342ef9ababf8ab99613eeedcf45e510572c0c49
                                     58c04f2cd2b4a6470ad775735bbd27ddae44c62e
                                     c4ce07fc8a2e5ddde584d16fbd65dce8de84d3ef
                                     363705a9a9c818a828a4ce80162a30a42bcf9272
                                     7839f411c76832a5a72a83d1e8e7c5475c468004
                                     b3c4e8ea6f89832a31289586e28e2b0f88d91542
                                     d3a6f8738adf45dfc9ab2b1c511d71a6ee6ae669
                                     b3c844bbfdb36e62c5975ce2a8c93df18771a67b
                                     9b940491eea93b7e33f72541adab291cbc998e42
                                     4a19da128dcf9c0be85b70508a1468b63bbf74c7
                                     53d238a89a26648c1baa01736fcd13af14c75e20
                                     b563654d54cfcb0e3bc5099b7b19eeab41ae8467
                                     0e0777e3dd5fa1ab081488df06d2d2e45f53821f
                                     d628d4b196b74de1e74d55e2f54dc1b7cccdc276
                                     cd64dc59eed780ed1fdac3dc9c932fe2de3cfbb3
                                     0bf52809cba0781c432ffdf4ed7fdbfcbbc392e5
                                     03a2ff2b7f924505d7a6e66cbc1e6af72069a259
                                     92308b19995ac276405e99da1061d0f8fc124c7f
                                     17453830b7c465f813b51276efc24e402c15da4c
                                     b71513548f0ce0dc32880af9a63a91041320835d
                                     9d59a446f3159aab311926ccfca92f57fb8b6cfd
                                     43f2034034b0d73618ad199d8fc6be1321b7bb7c
                                     260fe6717997f84ba6413a6872c1fa58e29562c2
                                     5afc8d5cec39bbf810a85a759773d09d65881d5d
                                     ee7fd014ef333b3625a860e9127de9261dacb573
                                     8f1680a0eca8b0f305fbe06490a1f91b7d281e78
                                     e9498cee8f586ae53cb23e24d4bcf714d2c06c6d
                                     f79a2e519970628eceb3b56be0b578ecc11f6144
                                     dfbb543acea742c5cec308abb0180e115558e39d
                                     1dd1e20dd45fc9794f406c9c240e9884e8a1ef04
                                     9d2c48454d5d90881a8d5fc25c43b379aad10b1a
                                     ef6a75099d7224b657a85cddb51d647c70d44492
                                     ad36dfb169130b3666dffa0ec68c1187b8ff2420
                                     0c7879e9f5aecca6b01fda1d28bec3085251b72f
                                     4cded0269c7f6a3506c4db14d1009c2cbded2a55
                                     6216b13c3168daa8e670825d689a6f5d139e89f0
                                     6e7c385a734e9ea7fa1cebfb45a828ab364993d0
                                     db71e1705db9d4adf0c4c198d6fb786e113ce340
                                     d87313787f9fc84b589d7d7d9e3f092e405d8b0b
                                     9791fe20df914e7e228c15fcf3987ca47a09da70
                                     a134b289635df59bb004f4e5b2e00129cdecc348
                                     4705fbc473775bc2114d2943dcff4b0e13a2a58d
                                     54eba3f2b985c8f121db36226e1c4d475c9aeef1
                                     b79c089a4aab485f1fc2aeea51bef030b67c7547
                                     8e999107fe66aaa4d782e8d505816288aef142fb
                                     de16bc4ce68001fcad84d31c1873d7933ff0402d
                                     09823c364d18cbd14c9c3693e95bee77d07775eb
                                     9051940abd77ee7ab1e4221eacb23a25a18b218f
                                     6c64c00e3adda389033ae75f626893b52ecd13e1
                                     1dc6f6ecba6f8ad85f0e1e8d0a31c5f8c1aa4439
                                     227c54fac865728e4031c1edf54cf43567ff1eb4
                                     99853f5be8d0f75d914ccc9784ddca75feec0126
                                     4ee7f91c10e01be59f46fc99f2307099d8c66bfe
                                     e8d8ac7c4ac2d9dd14c1be35a6986980258d867d
                                     09e46280e0088377db8977a898a36a35cd7c5d3f
                                     4e30d5d966fe7fbfefc649f357a3f0d7ff510a65
                                     519631732fc017e1735f98b14f57f1235153a816
                                     531ac07fe1e67700a1d679b717a7f738d25d5610
                                     ae91e79d93a8eb44563f45fc8c250db085e93745
                                     adebc325ed28636522cdc327ff79ac1a772ad01b
                                     ed4e83b42be9e7b1f2b4e142ea5f04b0fda566e4
                                     14eff492131019644394f9bdd5dad323e9b89784
                                     1dbea801d21055845fa4db100dc1966ed579bb8d
                                     319204471746b1b692116eb54ca31dd8639dfa1e
                                     43792fd04789e4026303eea98bd41739dbeebd93
                                     e09bccbd363489de81f9e628b203fbe2b916176f
                                     df7675b9face5b34d469832bfb5820596f60c34d
                                     c49cfc49b231583521cf057fd3daa54897c0a8b4
                                     a1dd0ae6ca3bfb0fd9954f93c0b29dbb805c7ef6
                                     778c14a31649a91116e6b9b40d9606ece83cb0e6
                                     245aaac58da8a85ed6c55b13ec72677399cd1048
                                     d8773d39a0e809e512a78bdcdb48e438298b4f0e)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         51eef820dc0fd3073c0561963b482e5e979c8315
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 21d4c8839d263ebf9ab4031e0d0b30f43163c763
                                     f594b1e56000487953bd3f7d3f7c20631d510646
                                     823b16a1ff1a616f567c3b1960d4d4fa58d114f8
                                     bb03b4a2ebbc958015e9d2b69a517bbf4744d4e4
                                     6911cf619bb1c2ffe103bdf6d729607722ce6a46
                                     dc07e2ffe98a081ec8383d66a364233ef4d76fb7
                                     283368aaafdb42eda8a843a1ab0c38d884dab46f
                                     ec4ede842d49f8906371a8ab68541d9183a61a6c
                                     a43abb6d7ee12aa0375ee7e6ee4067bf817a88f2
                                     25701136c415879bd472798e6bdf9a5190d9d84f
                                     078d22d65b68170466e1c24aea21d1a0f7e5b723
                                     6f63a07ffb12b4441343cde28baedcf00860d519
                                     fbb58ba95e02e4b77327ca2d55755d5b13b71632
                                     0f90c2a1db30aab08a9245abf06a05c7c8415405
                                     b6d2c580c3227e4bd83833aa7f28dedd03bd8ddf
                                     352915594c1d3804bd2c9fd13d9829ca0d06da38
                                     66f2603b03816267ccc6b09ba48320d4b01d0a94
                                     acfba9bd5b8d701d3db24f07b2811b677023da24
                                     69fdadecb1900cbbac4cc9aa35910a6871a826c9
                                     22ac6d3ecd678aafb9151ac7b626b6b8b071cab3
                                     73c52bbc31011057515ee47f0fad8f98955b8891
                                     b8672c3218817e627fa2a8f1f5207a3cd5640374
                                     e28d41dddc17a1ea321c985208086a4e4488da9c
                                     3a1736f0324053e42211b508e960f5485f2f0ffb
                                     c3016ddc42dd05d9bd9562e6be1e58004036ea2d
                                     72fa5411b430b51a9a3a921217659c427ed6d589
                                     44a84dc0ea3581fe34a3869b370de07a6cb51404
                                     aec40d7034743d90450edb872e7e355155e16c3c
                                     5dc5340fd2d33d7a270adfe39a89c5800ddfd0e4
                                     ecd2afcc644553ec894324ea4034c0de79bd90dc
                                     9c00270bece6054e3b3dac12708b0a1f4259c77a
                                     9012f2f89bdffd86f698b0f90eb456d594f7c965
                                     858ee9b7938978f6b499cc55c51ee9308cb1262e
                                     fc19be46717455f9613d488b8f78efa7abfb8a50
                                     9ee8f8ab05153d68931ffe35c49e38f5f7068672
                                     bb3469072f05b5012061eb85f04d6b91b3330c0a
                                     f811f0bfebb305303e381d64c2b853310f5d5761
                                     5ebbfea621c04eb5651947648717b6998c62d6f8
                                     79d55ab6636c771a4da23e9d06589a4b306646f5
                                     d328e225f32ce1ee08c6feba2ee90e060edf5d3f
                                     bd46ff9cd75ded5b5bacdc9bddfdad493d057074
                                     6577190395c71fcedab2299c00d8772a1b52bf49
                                     d528ecca009ddd3b87812e273790c55e8b481988
                                     397e3ad2c71af479c648a1b3a197ee8ed1c86398
                                     2800f66bb1973b1231386d6f1bd454838fdf91b1
                                     495b3a36accbb6bcea2073153dd0ae175de35aba
                                     c494a5ca9cc501d417ce40bb140785ee47a9cc43
                                     02476c9bfeec0266f8fe6f5861e21ee1f92c7111
                                     dd8c3da8ee140bec8b6415094542b51f65f75c42
                                     f2d57b49e23fab21697e3ec7a16a2dfe3695cd1d
                                     799cf83b47aab504a0339a578e07d527298cbfee
                                     4e44f8ea704ff2c6057df1cd3de766636dc2321c
                                     9be210ee13c8b3253197984b20afa6aeaeb429ae
                                     694661193f348277bbb9daab90e6d39d867b6fd0
                                     4bc3abd61216c6fe7a125dbc74781422585a48f7
                                     9a872ff452bed83883b278954994e538d90e49d4
                                     fcb62d07f4b414b58fd76bfafaa94ae83edb8c1e
                                     3bcb256af950b0ed31830536242946e6f5710dc7
                                     ae02d7ed3d0398a7b1363eebe335ef1aada14ba3
                                     e9097bc28940d4a99fa33c720fb8828d75403ec7
                                     1c098778262f9716a233be5c1665c8a19dda1adc
                                     a4cb16fa32295d97ec117852fa1c7c6d3c1a303f
                                     2c8718863922da2c21c089f07f27a1ce2b4f290d
                                     16cb887af1264c05741fdf7bbd14920f440e824d
                                     0adef535506140fa5aed81dcbad621e48c0188e6
                                     e3b6d83093aea1cd97c231f1128ea62cefbd97d7
                                     85f083e406dd2dd4f4fb95dcfec9fbffeb2dbe46
                                     adb722547cc195c0353c01ab5a1df0734a180b34
                                     c1ab45c81a48e267ebd8c7691e9187b482a4ac43
                                     8de3a6809d8e2fadb78f7ba3fbc27f1d2090fc56
                                     44bd730a5882cbfdc3c1a713478d59fac065a0b7
                                     6c23dee6778a8937832a16fa3a684dc81229a410
                                     a42a71eba980779ee1a860c5e6f10a34d24b56df
                                     418a7618d2b48a028bb443811d07f9be26ef2825
                                     8df3ce07bbabbfa5df650bc84ada05173bbf3060
                                     7acb91f8692bfc8365cc48a18b394e0e78ccabd3
                                     e3e786116a8d94b97b9e0a62c8f5810fe1c2d2ec
                                     e4a693c1bf6ac0bdc9991715c8c32bb36d414820
                                     e29b5804ed98640a95b257a14ae4b177385c3e16
                                     0b5aafaa7cf66988d9bd888b431e69e757bed6e5
                                     4dfbd2db89bfe14970ce53c323657b14e17d6183
                                     6c70eefbf8ed6a48dffea2ab18070fb810cc2d0c
                                     9161ba1b69ad8ca92d4c9df058c1cdaa70722c67
                                     1c885bf8033ed1c7f351f88ab8ca5e7954f75f26
                                     badf567b4cfa82bb8f00b83c907d80d63e90e235
                                     48db29f9bcfde3c233ed72c8d77e71525912eee5
                                     2b416e72677e784f64d67272a4452af956da4615
                                     25d7f0072fd7ec0d7b1f846c35945140f961e1bb
                                     7cda1fa7d933da4a62bd12e3d9450672c9a3d918
                                     72455ef7833c0f86e0e06c1753b8939fe7e930a6
                                     5658555fd9528eae9b60583c5a0dcd7393678715
                                     29a9d2d1e1623acf56e0dcdc8a51fc70e9032086
                                     bbf0d68461dc7ebf838cd708bdc6281466d32405
                                     39a034efa116462646159048390b30d0053ba966
                                     9c83c77f105904efdf403ceb3b7237bdfff4f065
                                     62e0682757db5c8251634a33954e04f916ab5e46
                                     9f4546af996d537c0edef97f4468f38b760cf316
                                     5d4fa02b5d39def3354dd1f8703457b3e5c77a9f
                                     bfa6bfda8fd165bf3a52b6f15e248df681572513
                                     3cd9c3995f76048c61e52dbd73f2af1c952f913c
                                     d215b372bce073b22d574226285a3bcf6b78394c
                                     1bb6660c1ed20fb1490582d3f00977d2fc56ec3e
                                     c96251edce24261be8a2ec235f91c7d9216fa08f
                                     e8f8c0a0f1692132b390eda9aba0e9dbef361bdd
                                     12c1ee2b1044a17800f2b81e2ff5e1384a06ec02
                                     0837aeddf842310dfff4ad56f0fc44a17308a8ae
                                     be6db15185259264191a0a1976c0de3e4da090d8
                                     f39c2d0b35cea0ce0f8a2ba51ed121955e914869
                                     0f1008b92831b799d6df3c71b66897e1c343b9cf
                                     5b9278fb1ec24f7b94644a3ce3c2df716e4af163
                                     dbb51350a33bc66a2dfa52d01421c00824d17da2
                                     855022c02df878e401655cc27d6d2b334781ecd8
                                     6fccaee56b90576d569ef8b39fb2c93a45348861)
    endif()
else()
    ExternalProject_Add(${PACKAGE_NAME}
        GIT_REPOSITORY
            https://github.com/opencor/${PACKAGE_REPOSITORY}
        GIT_TAG
            ${GIT_TAG}
        GIT_SHALLOW
        SOURCE_SUBDIR
            llvm
        CMAKE_GENERATOR
            ${CMAKE_GENERATOR}
        CMAKE_ARGS
            ${CMAKE_ARGS}
            -DCLANG_BUILD_TOOLS=OFF
            -DCLANG_ENABLE_ARCMT=OFF
            -DCLANG_ENABLE_STATIC_ANALYZER=OFF
            -DCLANG_INCLUDE_DOCS=OFF
            -DCLANG_INCLUDE_TESTS=OFF
            -DCLANG_PLUGIN_SUPPORT=OFF
            -DCLANG_TOOL_HANDLE_CXX_BUILD=OFF
            -DCLANG_TOOL_HANDLE_LLVM_BUILD=OFF
            -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
            -DLIBCLANG_BUILD_STATIC=ON
            -DLLVM_BUILD_RUNTIME=OFF
            -DLLVM_BUILD_RUNTIMES=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_BUILD_UTILS=OFF
            -DLLVM_ENABLE_ASSERTIONS=OFF
            -DLLVM_ENABLE_BACKTRACES=OFF
            -DLLVM_ENABLE_BINDINGS=OFF
            -DLLVM_ENABLE_CRASH_OVERRIDES=OFF
            -DLLVM_ENABLE_DIA_SDK=OFF
            -DLLVM_ENABLE_LIBEDIT=OFF
            -DLLVM_ENABLE_LIBPFM=OFF
            -DLLVM_ENABLE_LIBXML2=OFF
            -DLLVM_ENABLE_MODULE_DEBUGGING=OFF
            -DLLVM_ENABLE_OCAMLDOC=OFF
            -DLLVM_ENABLE_PEDANTIC=OFF
            -DLLVM_ENABLE_PIC=ON
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_PROJECTS=clang
            -DLLVM_ENABLE_RTTI=ON
            -DLLVM_ENABLE_TERMINFO=OFF
            -DLLVM_ENABLE_WARNINGS=OFF
            -DLLVM_ENABLE_ZLIB=OFF
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_DOCS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_GO_TESTS=OFF
            -DLLVM_INCLUDE_RUNTIMES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DLLVM_INCLUDE_TOOLS=ON
            -DLLVM_INCLUDE_UTILS=OFF
            -DLLVM_TARGETS_TO_BUILD=X86
        BUILD_BYPRODUCTS
            ${INSTALL_DIR}/${CLANG_LIBRARY}
            ${INSTALL_DIR}/${CLANG_ANALYSIS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_API_NOTES_LIBRARY}
            ${INSTALL_DIR}/${CLANG_AST_LIBRARY}
            ${INSTALL_DIR}/${CLANG_AST_MATCHERS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_BASIC_LIBRARY}
            ${INSTALL_DIR}/${CLANG_CODE_GEN_LIBRARY}
            ${INSTALL_DIR}/${CLANG_CROSS_TU_LIBRARY}
            ${INSTALL_DIR}/${CLANG_DEPENDENCY_SCANNING_LIBRARY}
            ${INSTALL_DIR}/${CLANG_DIRECTORY_WATCHERS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_DRIVER_LIBRARY}
            ${INSTALL_DIR}/${CLANG_DYNAMIC_AST_MATCHERS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_EDIT_LIBRARY}
            ${INSTALL_DIR}/${CLANG_FORMAT_LIBRARY}
            ${INSTALL_DIR}/${CLANG_FRONTEND_LIBRARY}
            ${INSTALL_DIR}/${CLANG_FRONTEND_TOOL_LIBRARY}
            ${INSTALL_DIR}/${CLANG_INDEX_LIBRARY}
            ${INSTALL_DIR}/${CLANG_INDEX_SERIALIZATION_LIBRARY}
            ${INSTALL_DIR}/${CLANG_INTERPRETER_LIBRARY}
            ${INSTALL_DIR}/${CLANG_LEX_LIBRARY}
            ${INSTALL_DIR}/${CLANG_PARSE_LIBRARY}
            ${INSTALL_DIR}/${CLANG_REWRITE_LIBRARY}
            ${INSTALL_DIR}/${CLANG_REWRITE_FRONTEND_LIBRARY}
            ${INSTALL_DIR}/${CLANG_SEMA_LIBRARY}
            ${INSTALL_DIR}/${CLANG_SERIALIZATION_LIBRARY}
            ${INSTALL_DIR}/${CLANG_ANALYZER_CHECKERS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_STATIC_ANALYZER_CORE_LIBRARY}
            ${INSTALL_DIR}/${CLANG_STATIC_ANALYZER_FRONTEND_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TESTING_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TOOLING_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TOOLING_AST_DIFF_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TOOLING_CORE_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TOOLING_INCLUSIONS_LIBRARY}
            ${INSTALL_DIR}/${CLANG_REFACTORING_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TOOLING_SYNTAX_LIBRARY}
            ${INSTALL_DIR}/${CLANG_TRANSFORMER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_AGGRESSIVE_INST_COMBINE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ANALYSIS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ASM_PARSER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ASM_PRINTER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_BINARY_FORMAT_LIBRARY}
            ${INSTALL_DIR}/${LLVM_BIT_READER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_BITSTREAM_READER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_BIT_WRITER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_CF_GUARD_LIBRARY}
            ${INSTALL_DIR}/${LLVM_CFI_VERIFY_LIBRARY}
            ${INSTALL_DIR}/${LLVM_CODE_GEN_LIBRARY}
            ${INSTALL_DIR}/${LLVM_CORE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_COROUTINES_LIBRARY}
            ${INSTALL_DIR}/${LLVM_COVERAGES_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEBUG_INFO_CODE_VIEW_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEBUG_INFO_DWARF_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEBUG_INFO_GSYM_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEBUG_INFO_MSF_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEBUG_INFO_PDB_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DEMANGLES_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DLLTOOL_DRIVER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DWARF_LINKER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_DWP_LIBRARY}
            ${INSTALL_DIR}/${LLVM_EXECUTION_ENGINE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_EXEGESIS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_EXEGESIS_X86_LIBRARY}
            ${INSTALL_DIR}/${LLVM_EXTENSIONS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_FILE_CHECK_LIBRARY}
            ${INSTALL_DIR}/${LLVM_FRONTEND_OPEN_ACC_LIBRARY}
            ${INSTALL_DIR}/${LLVM_FRONTEND_OPENMP_LIBRARY}
            ${INSTALL_DIR}/${LLVM_FUZZ_MUTATES_LIBRARY}
            ${INSTALL_DIR}/${LLVM_GLOBAL_ISELF_LIBRARY}
            ${INSTALL_DIR}/${LLVM_INST_COMBINE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_INSTRUMENTATION_LIBRARY}
            ${INSTALL_DIR}/${LLVM_INTERFACE_STUB_LIBRARY}
            ${INSTALL_DIR}/${LLVM_INTERPRETER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_IPO_LIBRARY}
            ${INSTALL_DIR}/${LLVM_IR_READER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_JIT_LINK_LIBRARY}
            ${INSTALL_DIR}/${LLVM_LIB_DRIVER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_LINE_EDITOR_LIBRARY}
            ${INSTALL_DIR}/${LLVM_LINKER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_LTO_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MC_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MCA_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MC_DISASSEMBLER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MC_JIT_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MC_PARSER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_MIR_PARSER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_OBJ_CARC_OPTS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_OBJECT_LIBRARY}
            ${INSTALL_DIR}/${LLVM_OBJECT_YAML_LIBRARY}
            ${INSTALL_DIR}/${LLVM_OPTION_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ORC_JIT_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ORC_SHARED_LIBRARY}
            ${INSTALL_DIR}/${LLVM_ORC_TARGET_PROCESS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_PASSES_LIBRARY}
            ${INSTALL_DIR}/${LLVM_PROFILE_DATA_LIBRARY}
            ${INSTALL_DIR}/${LLVM_REMARKS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_RUNTIME_DYLDS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_SCALAR_OPTS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_SELECTION_DAG_LIBRARY}
            ${INSTALL_DIR}/${LLVM_SUPPORT_LIBRARY}
            ${INSTALL_DIR}/${LLVM_SYMBOLIZE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_TABLE_GEN_LIBRARY}
            ${INSTALL_DIR}/${LLVM_TABLE_GEN_GLOBAL_ISEL_LIBRARY}
            ${INSTALL_DIR}/${LLVM_TARGET_LIBRARY}
            ${INSTALL_DIR}/${LLVM_TEXT_API_LIBRARY}
            ${INSTALL_DIR}/${LLVM_TRANSFORM_UTILS_LIBRARY}
            ${INSTALL_DIR}/${LLVM_VECTORIZE_LIBRARY}
            ${INSTALL_DIR}/${LLVM_WINDOWS_MANIFEST_LIBRARY}
            ${INSTALL_DIR}/${LLVM_X86_ASM_PARSER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_X86_CODE_GEN_LIBRARY}
            ${INSTALL_DIR}/${LLVM_X86_DESC_LIBRARY}
            ${INSTALL_DIR}/${LLVM_X86_DISASSEMBLER_LIBRARY}
            ${INSTALL_DIR}/${LLVM_X86_INFO_LIBRARY}
            ${INSTALL_DIR}/${LLVM_XRAY_LIBRARY}
    )

    create_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                   ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                   PACKAGED_FILES include ${SHA1_FILES}
                   SHA1_FILES ${SHA1_FILES})
endif()

# Let others find and use our package.

set(LLVMCLANG_ROOT ${INSTALL_DIR} CACHE INTERNAL "${PACKAGE_NAME}'s root directory.")
set(LLVMCLANG_INCLUDE_DIR ${INSTALL_DIR}/include CACHE INTERNAL "${PACKAGE_NAME}'s include directory.")
set(LLVMCLANG_LIBRARIES
    ${INSTALL_DIR}/${CLANG_LIBRARY}
    ${INSTALL_DIR}/${CLANG_ANALYSIS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_API_NOTES_LIBRARY}
    ${INSTALL_DIR}/${CLANG_AST_LIBRARY}
    ${INSTALL_DIR}/${CLANG_AST_MATCHERS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_BASIC_LIBRARY}
    ${INSTALL_DIR}/${CLANG_CODE_GEN_LIBRARY}
    ${INSTALL_DIR}/${CLANG_CROSS_TU_LIBRARY}
    ${INSTALL_DIR}/${CLANG_DEPENDENCY_SCANNING_LIBRARY}
    ${INSTALL_DIR}/${CLANG_DIRECTORY_WATCHERS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_DRIVER_LIBRARY}
    ${INSTALL_DIR}/${CLANG_DYNAMIC_AST_MATCHERS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_EDIT_LIBRARY}
    ${INSTALL_DIR}/${CLANG_FORMAT_LIBRARY}
    ${INSTALL_DIR}/${CLANG_FRONTEND_LIBRARY}
    ${INSTALL_DIR}/${CLANG_FRONTEND_TOOL_LIBRARY}
    ${INSTALL_DIR}/${CLANG_INDEX_LIBRARY}
    ${INSTALL_DIR}/${CLANG_INDEX_SERIALIZATION_LIBRARY}
    ${INSTALL_DIR}/${CLANG_INTERPRETER_LIBRARY}
    ${INSTALL_DIR}/${CLANG_LEX_LIBRARY}
    ${INSTALL_DIR}/${CLANG_PARSE_LIBRARY}
    ${INSTALL_DIR}/${CLANG_REWRITE_LIBRARY}
    ${INSTALL_DIR}/${CLANG_REWRITE_FRONTEND_LIBRARY}
    ${INSTALL_DIR}/${CLANG_SEMA_LIBRARY}
    ${INSTALL_DIR}/${CLANG_SERIALIZATION_LIBRARY}
    ${INSTALL_DIR}/${CLANG_ANALYZER_CHECKERS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_STATIC_ANALYZER_CORE_LIBRARY}
    ${INSTALL_DIR}/${CLANG_STATIC_ANALYZER_FRONTEND_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TESTING_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TOOLING_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TOOLING_AST_DIFF_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TOOLING_CORE_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TOOLING_INCLUSIONS_LIBRARY}
    ${INSTALL_DIR}/${CLANG_REFACTORING_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TOOLING_SYNTAX_LIBRARY}
    ${INSTALL_DIR}/${CLANG_TRANSFORMER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_AGGRESSIVE_INST_COMBINE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ANALYSIS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ASM_PARSER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ASM_PRINTER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_BINARY_FORMAT_LIBRARY}
    ${INSTALL_DIR}/${LLVM_BIT_READER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_BITSTREAM_READER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_BIT_WRITER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_CF_GUARD_LIBRARY}
    ${INSTALL_DIR}/${LLVM_CFI_VERIFY_LIBRARY}
    ${INSTALL_DIR}/${LLVM_CODE_GEN_LIBRARY}
    ${INSTALL_DIR}/${LLVM_CORE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_COROUTINES_LIBRARY}
    ${INSTALL_DIR}/${LLVM_COVERAGES_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEBUG_INFO_CODE_VIEW_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEBUG_INFO_DWARF_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEBUG_INFO_GSYM_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEBUG_INFO_MSF_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEBUG_INFO_PDB_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DEMANGLES_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DLLTOOL_DRIVER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DWARF_LINKER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_DWP_LIBRARY}
    ${INSTALL_DIR}/${LLVM_EXECUTION_ENGINE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_EXEGESIS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_EXEGESIS_X86_LIBRARY}
    ${INSTALL_DIR}/${LLVM_EXTENSIONS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_FILE_CHECK_LIBRARY}
    ${INSTALL_DIR}/${LLVM_FRONTEND_OPEN_ACC_LIBRARY}
    ${INSTALL_DIR}/${LLVM_FRONTEND_OPENMP_LIBRARY}
    ${INSTALL_DIR}/${LLVM_FUZZ_MUTATES_LIBRARY}
    ${INSTALL_DIR}/${LLVM_GLOBAL_ISELF_LIBRARY}
    ${INSTALL_DIR}/${LLVM_INST_COMBINE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_INSTRUMENTATION_LIBRARY}
    ${INSTALL_DIR}/${LLVM_INTERFACE_STUB_LIBRARY}
    ${INSTALL_DIR}/${LLVM_INTERPRETER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_IPO_LIBRARY}
    ${INSTALL_DIR}/${LLVM_IR_READER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_JIT_LINK_LIBRARY}
    ${INSTALL_DIR}/${LLVM_LIB_DRIVER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_LINE_EDITOR_LIBRARY}
    ${INSTALL_DIR}/${LLVM_LINKER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_LTO_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MC_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MCA_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MC_DISASSEMBLER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MC_JIT_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MC_PARSER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_MIR_PARSER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_OBJ_CARC_OPTS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_OBJECT_LIBRARY}
    ${INSTALL_DIR}/${LLVM_OBJECT_YAML_LIBRARY}
    ${INSTALL_DIR}/${LLVM_OPTION_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ORC_JIT_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ORC_SHARED_LIBRARY}
    ${INSTALL_DIR}/${LLVM_ORC_TARGET_PROCESS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_PASSES_LIBRARY}
    ${INSTALL_DIR}/${LLVM_PROFILE_DATA_LIBRARY}
    ${INSTALL_DIR}/${LLVM_REMARKS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_RUNTIME_DYLDS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_SCALAR_OPTS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_SELECTION_DAG_LIBRARY}
    ${INSTALL_DIR}/${LLVM_SUPPORT_LIBRARY}
    ${INSTALL_DIR}/${LLVM_SYMBOLIZE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_TABLE_GEN_LIBRARY}
    ${INSTALL_DIR}/${LLVM_TABLE_GEN_GLOBAL_ISEL_LIBRARY}
    ${INSTALL_DIR}/${LLVM_TARGET_LIBRARY}
    ${INSTALL_DIR}/${LLVM_TEXT_API_LIBRARY}
    ${INSTALL_DIR}/${LLVM_TRANSFORM_UTILS_LIBRARY}
    ${INSTALL_DIR}/${LLVM_VECTORIZE_LIBRARY}
    ${INSTALL_DIR}/${LLVM_WINDOWS_MANIFEST_LIBRARY}
    ${INSTALL_DIR}/${LLVM_X86_ASM_PARSER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_X86_CODE_GEN_LIBRARY}
    ${INSTALL_DIR}/${LLVM_X86_DESC_LIBRARY}
    ${INSTALL_DIR}/${LLVM_X86_DISASSEMBLER_LIBRARY}
    ${INSTALL_DIR}/${LLVM_X86_INFO_LIBRARY}
    ${INSTALL_DIR}/${LLVM_XRAY_LIBRARY}
    CACHE INTERNAL "${PACKAGE_NAME}'s libraries."
)
if(WIN32)
    set(LLVMCLANG_SYSTEM_LIBRARIES
        version
        CACHE INTERNAL "${PACKAGE_NAME}'s system libraries."
    )
endif()

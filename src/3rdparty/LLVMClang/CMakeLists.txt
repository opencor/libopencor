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
                             25df1099fd5806ecddd776784cffb19239d0676d
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 1c8cd7f0f7e2a0804ea08a26f443b00ccf39cf0e
                                         daab55ae33fae10bfa2e8c48ade3d4af909720d7
                                         1b86d2a153c4ff54897517108101a62c78aad5fb
                                         fe5b93885445257640ffe52bfc3276d368d74401
                                         2e21540f350d403888a9bc7193d35ccc765ef438
                                         c38c9a6bdb4663c0b45122905530eae90f4a890e
                                         b22e63e47c60ebce01504450ea8266b95e2ac16f
                                         5b34e6f48aaf3347656d453ffbdd54881fb82856
                                         27d4d9a270df1441c3360413b0904a0f378e2939
                                         85006cd7091b43f3c79bbf2b822339c34939c298
                                         ad591d425bc270f100fb8940cea5a6e29f26dcd9
                                         44a415574b0b59e7eec63540bf11819441b9e26f
                                         5c688e466a26ec1eb816e2149c05d5e64187607e
                                         8c5b2e095e8f05754da5c406de921f8fd14d6db5
                                         047aad6379d9630925fd0571ba8c81ae7fb14da9
                                         8d0ba94c15f7de8d0581644d430f27e31468bfaf
                                         95d20176aa77c586103a0101edc3cfdbb424111f
                                         2daf771ca2808757c2916b2326b3778b036d6853
                                         86f35414d8b04a647754ed78e2e874d116780d6e
                                         9a7e750a9e8ec56ba81055e573acc82fa37427e7
                                         960470ed5c2af1ce71afb278cc3a6bb67bc1156b
                                         5f558fd12d71ee8b4edd1eb66b82e47b87bd84b1
                                         6ad429e6bedea853cb640d2ad3a99c6825dbdb45
                                         63753581898bc6301f39b692d3df1716e74bc930
                                         a8c4360c7f0b6ee831ab7a69bbfade78a7ce6512
                                         1f20e0ab803e57eda11bdf8b4222612fb43c0739
                                         c569ca6081c5f057e63e8131da42086f763ab079
                                         d325bb444a779f06cad0bbac4159b96c025b9d44
                                         87f59d33826b39659aaab9d72343ede21ba68e78
                                         4b2e682481d010ea9bb1f254bb68e6d60dd66871
                                         c53b09884f0c9e1172b9a5434b744366947157ef
                                         3f829371499a7466765880940bcc3e8f0d1e2635
                                         3314e7717066b043cb94f016e490f0258b6e803f
                                         7f42e1fca5b93a0e84315680d16c203f8a321928
                                         72fdb6540d07468c4d3adf38e4a4a955b6cfc45f
                                         93b312ef1f69a3b0c5ac6fc3bb624ee397eae9ae
                                         26629a4a37a5d8f2449c82c48fae268a47ba2dbd
                                         743dbc4986ecf25fea40c882c6e9760e48b00b85
                                         aa28933080ae7a12b2c99c723cc95c96bc9f9705
                                         549a50a1aabce380b9c18fd090a135d3fa7604cb
                                         017d98741c7359c7921e581bc901dce5bd8dc446
                                         d450a82c5fbe1b72ba6325a34a2c11b981a136c8
                                         c6f9a56051c618243600ffe860c02fa7553f729e
                                         63df9dd1386868a679896f48c4fbac530b09ed11
                                         0fec4e2284ca3ed13d3785e5df3a08d75d44f9fe
                                         278588fb8fd21f2ba35b7da4a7c5b3f14caf4933
                                         ac3159e8c79ddce8e6566faf37908e1be9b8d695
                                         c4c60209fe0f9c3281cd636b8a39f6ce65236ebd
                                         c8b595ce324ab653a203b025ace0e9def1a2efe7
                                         592293fabb4439369c6d6563db5e559e331eace8
                                         1dd79cf58805dfd3fab02b7f5ef0b13f3a7a3c29
                                         a75cb23534ee6d54d8ba3d2f1d14b6216fb09ae8
                                         d687c4fd426435952df71de77a25854eb0a048cb
                                         a614ab746e19431ef2ace8e75d1e3dd9ef7deafc
                                         4fbc9e7a0147d665c56fd1e23280e2cb3daf8555
                                         35651abcff44ead5fb82a641dbaeeb45f4caf52e
                                         d65a79bd646377a334bdce13a4783fd3c70d552d
                                         4ea2345df293b0afe458d4eb351b6c2165c8f637
                                         04691e740fcf3e1c4a1561240f9f35b6274e118a
                                         607c388fdd03dd99171e47ddf399422727f20899
                                         1fccb64f0004089f68f4f360f2abbdef36429f69
                                         5c4fa980ac4d5be028e8ad68487eccc572a6ed05
                                         ec3129dbdd30c6a0cf8f4722849eb7991ea91835
                                         9f756cdb406e5cc91fb88ac5ce0db644e94c000f
                                         95f6769c17d531f20f24ef854e4deefd20004819
                                         0f4cc6dcf7be25bbeb3d366811110d0af613bef2
                                         8c7d5373e91a0fe162d00ae12b6e02f4fb8c6e0b
                                         e02ae154dddfbc0041f1ab68212b3069668846ff
                                         b35a6d4c6e9ef6186fca7065e59c65c1f1c414cd
                                         2e6f89cc21c90cf5cdb50743a1d2aa0e216071f3
                                         bb15d08a38f2723572368ff2c027141fe6554f39
                                         defa0da502daaa8b46e0b7abd41f84fd454ab124
                                         1b4b0adf737ae030a520fc61ab79ec0bd9bc6e30
                                         9c1e6cc5e7b2497b52a521610b9cea14897e251e
                                         9b3fefcd25dce77316a54af20ac85683b020b79e
                                         b865660d55f9f94effa6989ea75e95ad1b52af71
                                         dad4dfe2aec9684819ed39774c647280e2212d3e
                                         1a04139f2d3094dcfdaedd3a275476b080857277
                                         e628c91aaa32cdec23b7b7f6ddaf3fd105b2e8a1
                                         e0668c71d0c099b66fb7ae5f6734ee0ffb73c93c
                                         e29244c5fdd4c404759928f5715202eecf8077ec
                                         59c74c99c111434c146486f923d15e4930273555
                                         266a2441be7378944714508096cd1a3e8d3b9241
                                         4a37f00055178e3c57e581d9989eae55134c1f20
                                         89f0cd048abeea3f3878a32bd51616397a415b6a
                                         5a40ed1e2105ed6ddf079d21e3c18dfd715585fa
                                         c682233c5ecba954f11ab37d9d39f9d9333491d0
                                         15ed00a3a668bd8495e62dae8f7056893d0735fc
                                         3fc0af4d3883442d07c0dd544d4608da45baa825
                                         d47f63e317f76030abef44afe36978b06255ca74
                                         8628cfd4aa665cbf42ae16d29c34dcc2a968fd20
                                         fc8e4980f16b6d1fd2f87d981a5a070cc09f00cf
                                         09206f06bf5726fd5d4136f90403f15d2c1ecab4
                                         a7da99d8d35447efcee640ebf15a7558a7cd8ea3
                                         7dfe61dfd9e75de81270ef545e9a5ebdbb8b95b3
                                         80b22eba259c009cbb1f4da4830c34120554ae04
                                         3625d9f94cdfa71f895b21f257395e1fb5a66e41
                                         ab49469b83cad25637acb4b7d5e195730203ea5d
                                         917ab36a13982ba7740172a435a2c113620ba195
                                         de8dec80a03e09f170f3cf8ec6b5abad01eeca3e
                                         c02a540fdf9e86ca1819997713e4c3e89fc17fbb
                                         a8b76ae05df161550c0062588b17fd27da9cdb35
                                         c09e4988b89fbe40c63d8ca8bccc1276e361c828
                                         549e07dcdc8f3690896c9723da706528d752d175
                                         238bb4ff297ea878c0d7d58337e1985eafe4caf8
                                         eaa83f294bf60b8df7bc89e9a511906b49c79701
                                         d754259911c7eaf49f1c600a188f8f0d0d02736f
                                         7569579027b240289a3cbb3a4a2061513b0cfa85
                                         9fd5b825e060d6087bec32e4bdd22373d1aa903e
                                         e54500ae959a7c9842d2fbbb9b38e201a20d7a00
                                         71ba6f99618880a3de3690441944cc956ecfebea
                                         441c257614dac787f14a6f4ed2a588f583b5ad8f
                                         21ae622f7c3c07f4f51302f028b9b295beb9d862)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             7dbfe4abbe900970ab12a39be3dc200ba868817e
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES b7c8dacb02e12467f97dd9908d24d73c7078e781
                                         3ef7a6335c9ea18be76b7e60df0377e2fcdc6036
                                         7489aae3a83283db5bf860be4a16b732f15cb279
                                         9858213aab320a759bd68e94e91fc6994b87b560
                                         039e7320fc067fa2f825e78923010080e23717bc
                                         ac043b72334cd363ef39518a6ff366f5de52c607
                                         36a5986211442d47888e6b0a37272a029ace3e97
                                         71ab0e866a0542f2c018aca2a9cfc9e0c0b36af6
                                         e3930ab96fa1d90d362a284c04d3acfd5e377cf1
                                         57527468b728bcb27f8dda9a5b65cac859f37fc6
                                         96afe43678fec2cd4a82a80100e2fc6a3284520d
                                         6636eb23924e734db0a12c3626b6bae58d8ead9c
                                         b7399e2aca7b1199712831abab703ce1ccb3eb25
                                         36557eb87816b63a0d9bf469bbc6281879652fa7
                                         730a1bbed033cced386dd61bb111620f27d25965
                                         a206ed383535336dc8bea697659876a08cea35b2
                                         12a4a886f51bca393ebef36b0b6bd2cc80f1a334
                                         96134efebf2ebb3769ca3e0e600afaeff216f7e1
                                         7ad2a6792cb402ed4b8aa86a402c706fb4787811
                                         d570a321af5cd885e68d79329e8d27391d65a857
                                         63d1c583d7940c64268f3c3dd05e6915d0538415
                                         3fbcbbcbc3700042718693649e13ce9082656c5d
                                         75533be489784738586232f2ccbe8d04dd100000
                                         5450c0124609169d73923aa8a91acdfc0311c8ff
                                         b10a63af4447d3a75edf4fd0167c3447fd1c368f
                                         20a08afa5ac21406ba6d81f17330e6f7d42f4b98
                                         bc2557b337b4548e331ba7cdcf7270d843e0cb97
                                         91a39801a3ce6cbcbd237219c6d2f5fde8141570
                                         9e74fd630e127ec71b1899eaf8336859f6e90eb2
                                         ad0f894896b1aa1085e39564cf75e3970d5de26c
                                         f13c8b510d8373143a355803d5567f3f10a921d5
                                         cc14613535676c2b12cea0035094b415aef31eff
                                         2ef72d20a78e0439982e56f80f9186e8bf964c10
                                         4616165f2981779b17918768ba87d73168a230e3
                                         14000be5d05929e580f7f560ef1d9a23efb605a2
                                         7c36c46bb3475e8f37f4306d3d586f08b7661daf
                                         5887085458b52ebd0547c74a13f4dbec1364bb27
                                         a232012c2f8181a6b707b58aa0c2c59803ef31f3
                                         15a6bf8c41a9762a6f7f488c73357623f06c8533
                                         245bbe02137a9f0847730cac80f64952cea36429
                                         c61645ab5fe18c1f0757e32a0df4eb11c7ff6fe3
                                         dbd8f981c53cd5dbf4911464d547aded678730d2
                                         516cdefe7df43f46e69558e97bb24e7366e5cab1
                                         0ee4d737f40d6bf4b613ed3bf480206ca5c82cbc
                                         41e091b70bda89fb8e97a7de6efa33bcd042fcb7
                                         276ce3a6bd830ed3a5c63d9fac02db52d8c75ea5
                                         92c9f6094bbc3fdb8cb83e506531bb0d28e4b924
                                         8f7101cbdc66a04c6f08a94dd9ad8f7a84cb7cb1
                                         0de94e8952a29047096a79ca6494731897ec49c8
                                         995349ddb66ecf741569cc49ecb6d26cfb9b1450
                                         cfebc406c494d7de6283feb65106d5add4c7618a
                                         256c6178dd5f97fcfd5ce6f538f865226cc864dc
                                         83a59477193f6f7c9c37d26550cc68ab9ebf785b
                                         a702523f8d930d47b765a43fcc834d9d5115283f
                                         c849165943d73958ecb0fce0a323bc1282271c7d
                                         68d97676df213e5382ace512ba7d56314f5737cc
                                         a7a022dbd30593b1d46d846a6fd0f6ca8b891635
                                         0192553992710a7a3b8f4bd642d3b4433aa2b30f
                                         98c9af76eab3ea5b889c10a9943ded0eee12aecd
                                         418f9f730b4d3dcd225c6a4adebc7b34d7800680
                                         7734b8d3f32d26e3e1a8f99042e51ea03a187c75
                                         c8529c767029d8bcb1fcb283f7bb5100dff4e280
                                         b778132872bd2c6af6981102231645da588ef8f6
                                         16a3456ed32f86acb8c500669a3d96858f027f35
                                         8934caa4de3b2b406aae49c4449e3a8f1c93fd43
                                         eb56b0e2d4119fefebbe54bb2914e93fa0df7a70
                                         5f17be3f1440303800794edbb77f6b3e43679dd0
                                         d67fb582a14a918a777f8cb0b74ac0f8fcb84c5d
                                         79acedcdcc7cdb81b77d29640cb990aa3ac9a261
                                         b46f7539f71accf7b0161dcd6862ef846bfa5c05
                                         5313a8a42f6425c62dd845c1bfb33c9ff27559d2
                                         ce15c8c4e2aebb26c4f4f05237666ca54ea47461
                                         50caae23e255b60f6fbf699088cc9a805ff487c2
                                         8dad6b9dfd97ab645ef5c1a3398361e98a7bcfad
                                         675111eb220a29b25c44465505a329977d9d47ee
                                         c11390131ecbe8dc4fd6f50875ea5cfebf92cf0b
                                         075c2a594ceeac25d99c0d61909c90c7f4632b76
                                         a75f664f500652fde2a8479ba5eb517e8077e93c
                                         709967164209fcd0a8604782ce3c5cac2f8fa351
                                         74240cd5aad0fad2c8c0027ada27dcf8c0d9790d
                                         7f767176a53615941cecf5821c55655247e25ffa
                                         e24aa75fc40d8f847167ab8bd7671e19c9182f1b
                                         e4978126f22f8e05e660b7395d0923670dd5b1b2
                                         1cdc3816b70c96d4d935b782550125ea07d5907d
                                         4179a584e76b0eb9d61898a74391d6914db8c84f
                                         64e09af302d24ad5400fd471a7963471e1c17efd
                                         f1882d9823bdb58cf8d1a7340508b5a7d545d4ec
                                         79f36e9cfee02d78155817175b6495c1cdfb25cd
                                         7d2741fdc327645f9adabe1ed4f83442626791b1
                                         0bf9a0f039ab456beb8bddf95991bdff736977f2
                                         6f0a8f359e904cde7c1bf0a53b337872ca77d54b
                                         f9ad871a3bbaeb98a2a16accc5f8d42a2c706b1e
                                         f7400e24c114a2030d31fda6599c6868ccbb6cdd
                                         91137010033299fc57a914002727c48eabd765d3
                                         fa277b8525edb6d606cd18b502f659324fb0c4bf
                                         dccf334895cf8dbe53fb3fa56043e8403e99d9ca
                                         15f5dfc85160e3c3047e847f54196fdf03e8825b
                                         297eab667ff2af90e792e25353caf21dd7065fcc
                                         291d6cd668d3ad57d7d68ee17982218a6c8351ba
                                         be0128272c328a69793e709a33f8614df0f2068a
                                         60775c9ccf233aadf3423cce4072c4115a3c1d5c
                                         cb9f81637557f5d62faaa8b2de03dacd86419fde
                                         cb8a03b48470199e0470a6286eaf422f5271c79e
                                         93b54f1f72464850389961c7b638b4d068b42f1b
                                         fe920b6e2966673ea7b830a10c6caca2cdb282e8
                                         b55624271a4351165aa2b6c48235c2b6232426ea
                                         9f0cfdedd9c0881e776a0af2b5787a36fbba4317
                                         b19288a2d8c3e04f15140559d82c0eed3bdcc6ee
                                         b91787693e7f338d8ba924e94563e3b70103a7f8
                                         ac5d6fa6d7ef88a6ae1ea92a47028c5ae2ea5ff2
                                         f6b3a0edb6a44277414caf0bbb71414b749fd2e4
                                         cab3471b8fa01b347dbe045e5fa7c30295993156
                                         69a4c11498c6393104f986c273761c6b23571a13)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         1e4c5dcdb243fdefcfe03e4e33ef8a5c64189bb2
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 43b03c15c96a3b51f706e40b4f36786a0e2ca4a8
                                     a706952f1099ad8cc46ee8a8aeb9c2cafc4c7b01
                                     e717dc6d9b3e4edb49855c57487ee6e5c161e84e
                                     7fc9d229648319530cb72830d5b0aaf83ee6d527
                                     78a9f1d26dedc349fe110655f1ac9f32a6fd8f18
                                     cc4659953edeec57d2be4f1883a5ff87a44b8774
                                     26244e10d8f6e5bde68d5c52b29561924343dbc0
                                     fbcbbbed10a79741defff023eb6192bf870bd3ac
                                     61b396472f7ba580c9f191563aab822287f16fa1
                                     b327738a754e815490070a805a937a9cf317d7bb
                                     32dc61886454c2d75f88ba82b05549e17e2f2d45
                                     31e44be9ce5a565895b5c8e1750d91d11ef63586
                                     3232650854fca2cacd6cebe5d9bc6c80a56cd1fb
                                     8738ae1b69d56f38c51a77f6f2f3dc3ca4f3f57f
                                     8f6b953659829b3678b9c48e26b5be74ed0b31ae
                                     eac0dde1dfdfc736903a93827bdec05507342fb0
                                     ac63470b37a8daacc456577e3499ad0394b7e64e
                                     a8be0ef742c31dd59ccb2fb25c6b8bf89e56f597
                                     3c7d6de771918d0523f23e0fe69b9b090c2f5ac1
                                     33851c8aa43468ba238d9eb3a565604408e82dfd
                                     f4f34b1572d1536005a28000dc297dc5e7ccca10
                                     4d5a50e57e44b005ceddcee70bb2708ce6ce504c
                                     e1147c333d06f1bddd9c9dce9393779572f9d107
                                     fda8d26c5836a37e1554f623091dc9ef0aa7e3cb
                                     ca451e70b0c04d6530ea9896c8066e6b9d683814
                                     70662acbe2bf0c57e11d96c7bb88502f7401ca03
                                     5e66addcf9c7de16c32bc1d1e30f27162eeb2932
                                     7de0306f4aa53bc41f57cf5a6a11eb34b14aa230
                                     21e3e2b14849ac506bf90b2967749166cb2fedd5
                                     3c71d498c9e7f35cafe0b961924d66addde1ea8e
                                     eddea08ccc32e441bbcd528025b8d656cd9f4591
                                     04b99c1c83745c14b559ac4ed6a1496ba4ded803
                                     ca79f55e4724d3a159dcb7a8c9f8927b5bf5df1b
                                     ade2efd64d8dc978b31b054d1fec2090948bfb79
                                     d25ec94de321c91d4157dd1b133ac79bfe471470
                                     e767e0066795ddbd8bc591011772cae2ac59b638
                                     02c6a3d1d49a4e30e5d8ebfd53145296a6d7c12e
                                     be9e1fc187423ed5072c7580bac35dc4acc26f89
                                     2c5b92987c70c142d256caaf2576543261b0257c
                                     6aa788569cc20b9bf619579c4abc05d5bdbdc6ce
                                     ed7ff70c41f44675f695d1932e4b1bf4a633c9e7
                                     b4f8654be471c354894fa51b5cede431644a58da
                                     93925ca2fcb10d941881d28e69851d19a0900e6f
                                     eb715f6a901019a32fb804b09a4da453f31fb42f
                                     befb137b21dd1e676676ea3d7e3d67c92fb47bc2
                                     d7901e44eacb75d824b2213b65f12e611e59037f
                                     956a8580c63ec558277ba2415084207d91a5f723
                                     76bbb3827fbcb8720fe22060fb65311ec9c011b8
                                     cead89c94363828cf989764e71f33fe05ad15007
                                     176f741334582d6286f7ccff4f6ddc45a960a6b6
                                     4b42f39d21e9a70fd78aefd72d60cb412a20e361
                                     6373be48a74273da7c4d41766aaa5df2deb05a7f
                                     7044bbf1c044d5cdb968ced678a18a6017269659
                                     8d8d70dd8731b0ac37f09faab0e31dad0cfbfbe4
                                     52703b62630aa05660c6e26c5f5a5f127b182f67
                                     964efc44f9b5a13cb466bafa17d57051379bd82c
                                     198993790a13e71676bddf5c2b465e50d75cd9e5
                                     cf4f24030a96da5f54e4dd6365bc9984dc45859b
                                     d651d10e65b44e0c2d0160df861d88c6ec6dc11c
                                     487019e0f3483d26ebe7c0afb51bb053f4137742
                                     04cdaff93f3547df3357210ddc7b0a5ce852b063
                                     4b1bf035eaa5328c3c2710b1e61e15bef89b5611
                                     e6394e8ee57d654dfd27725007f441b9a02def40
                                     c36030391a3c2e57603873d69867c084cc27a4e3
                                     880e1c0faa0a1b663fdc35671d0421adef8fa25f
                                     57f56458dde8497477c13540f0f00f0e1819609a
                                     f76eabb90024e81b5b7822a669c2766241794863
                                     207e6c83534dedc30d5fcab7d485f1d6ace2fa27
                                     3058f07b7d3475249174a5ec8721c8e1fcad7973
                                     3693d7e2cb7ef86f5b0f3d6a4d37d8681de70941
                                     37b396a0ca0a5ade62ec7daeba8370ab7b827362
                                     8b0faaebdc70feaee2d05b569fa699143df8e24a
                                     63bf074f0add8b8e4dee8c2c46c4dd1a7dc42f41
                                     135fe0fbfb3e2b62125d0dea491b3b5fef2a60f4
                                     119b0ef68b551a196926575b3f01f27474e64cdf
                                     30dfac3341e90493503531f4e65f5965f6ca4c7b
                                     1884194a904994516ef560d15fdc17896a5de850
                                     dca824385f8a327c264450cb1f2decdda57723ab
                                     f7e7405c81dc9c40e9062296ca8d80a4d0953dd8
                                     2cd12359e180b8c9c07d6cc70c39137aa76e7564
                                     866e8c28944bd93088e981aa9e58e29b0fe11964
                                     f008b736b01982f2bc96d571aef8a1977b71afc0
                                     fbad5e507b1c8c1970c3b19e2638368834dcb50d
                                     c5d0d7e5bd0a06e22a3a46c88c7c3cc9962e8816
                                     487ad5551cf659d8704e5f59d4569296db272b0e
                                     2053013244e754820baddb052b0333feb4176b29
                                     8ab69696fb959297e4fa4f79ae60ccd42eb1e9e6
                                     44412d1eeea565fe5552a3e7d6a974cfeba57c54
                                     3022d656a75837527720ac7f7acfd5360cee0ae0
                                     3be7d3e44a26f29c606441d735343aaa957a76c7
                                     eda689302ba475c10779cbb30791f669b7005ea1
                                     69c5e41c17ae2a85025d0bec85ee08b048119f43
                                     a627e180e2abbac548027865123ad164498d5363
                                     c4e912761f73e4fae3e5d80f780caa95afe47438
                                     2b7bfcb1e243fec5d76f684b8ec2928644af818e
                                     d581dabc8b3b6caf524dbb4cb802074fdb97cd5f
                                     6b5170b4c3dcb60fdd0c2133870e21a188b3d192
                                     21e2ed4a4aa8bc8f87fead449d58b656aaf43d65
                                     78440106c94d6eb72b296039e7323a3f2a7ad30c
                                     37b81d7a1e7e9021121387068878b95dbddf54ef
                                     8b002b24bbed20f9d68711be1297fca25e63534c
                                     c4059a38d46d4d8231b051a157d05ba062afad3b
                                     27e2b1ab036997ede96d30570e71fb3d4906f332
                                     6645072ccaf2ddce5b8d5907f3e99c6991a664cf
                                     6c331363e82ab0b3471ebfefdb45616b6896bb1a
                                     7ad2b98e442d0e859bf7ff748a332a0447aa0df1
                                     006160c61489ffa3dbc23f01b6494568b0e1c859
                                     7f9ef412ee3a1b3193e6dc097e04f3ff668d2ee0
                                     24af46f7218b318de49ec8877bf2bfb464a50d57
                                     cf11605293908fbed4b61d6c66a7a03bda72673f
                                     7f0533e40079f04f5a806d1a965ca180e002a56c
                                     bd0f8f2edb6d537bb0d6402d8e1ad984cfbce836
                                     6f64d78d17c732beaacdc804b32e2a5886397940)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         b7b4832d2d1927ee46af07bd5ed8391c3599b47e
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 80f64f572c53fbcf5e5f07eb10246fbc8cbaf5f1
                                     5a0bea5b14446501cfb94168ab20b79adb4f15b2
                                     6e42d65c8aa765fe6a02ee4260132bae639f311d
                                     f335bf35dddf393b01ff0a30eaf729253faff2b1
                                     01ad9868476a863ecdd7466d38d936425ae8b8b2
                                     84fffdcf4bb89602354098b72b709ead03b2d99a
                                     1e400ecd404918ebcc2c9d558fefc30c58d27d25
                                     1c71679eac3e78802dca8dd6428088f586f8952c
                                     7fb083ce414d5ab397862b6dab0ec2fd0592c49e
                                     fd8de77078abd8eaa66a8375beae580c27b1f5d3
                                     fcc9d7c7b9a44d9e4002ebb3e6b8f359176b7d9d
                                     f80c7008ccfac642db44b010247e90cbc9b628a4
                                     bfc7df3c1ba4ca705c0a22fd1f5562912f1d7e41
                                     30166b853a0b91b5e3a236a23b5b2ade917a468a
                                     f05d536d7650c2aef64ce9df95e84efa40193769
                                     2a9badbed9b38705dad0a91de8c3368db7d9dbab
                                     04ebcf04066150edaab7d737c06e39c11eab25cb
                                     b2890043123111493d6a223a7e9980d582973bf2
                                     a874d5f97f8b38d2945d604cda9911dee626acc3
                                     526f48c65be483a8c02d4455a041d319f19fa109
                                     aa4c2fa566e0ed51a8e6de1bb1376c81f4d9e2e2
                                     1548ffed7fb2e7aaa8bcaedfa7a3f13ce31cb81d
                                     c5bc2e7bd9e6306291ea11692d37df307449a72f
                                     b6cf093a989a1ea8be5f8a64460b12a120e3d5fc
                                     7bd4923b638da105f9ae1ca56f04d5f8c8ee446d
                                     104a1e2be3201986cf1935686a51edfa2a793590
                                     9aa7274a016ba687163b052a463d82ba2bec79ff
                                     26f8e902cf5d3588b73c801f0d58bf1a63099467
                                     5dc5340fd2d33d7a270adfe39a89c5800ddfd0e4
                                     8f3a856b3800ab50774df4254fed71892dd3800e
                                     0147501a4867a15e50441718c75cd67cd9ea965f
                                     c0b9db3ee2a05619df1692a81cfc69f619326d8f
                                     f812355433d8a7bc15cf3d7ce76d6cf8939ba6f5
                                     6cb1c64d77cdf2cbc7d00abaf534453a41672254
                                     41a4df4076df864a8f4fdc1cef5274c5c777667a
                                     ba8b06930271711b0b0f20fa266b13a1a23a2cc3
                                     2a1451443ae6b6e577844e44bb043cf1a50e1e7a
                                     ea6b6d5829bbb663241dbf16b2704a9cd20d784c
                                     0185281fce5cf51bfc5a3d89101412bd34371dd9
                                     c36f240824e3081d846e7ecf061b950d84f71a54
                                     83533c51bc9c93ca3c236e09056418b4ac318263
                                     31c19de3a023300de847b4e0c8b4043c29993a5a
                                     68b62cee727014a720abfc1a324e0ce9f403961e
                                     1f6164b7345bf471240873b8cac20c859d34d5ee
                                     ec6b25474dfeac11700ffefd6a4dc1413bee5721
                                     d10febfd287245707e002e8dd1d05140b3448a23
                                     5648102669aed0988d31a8299b4334ca14a24642
                                     e12d2fced7b8d8358a0d9e3a95229e6e3cb1a4f7
                                     f8931adf2f0075b7e33c676645e8b5bfc2b9b95a
                                     5f7a53c7464bbf8973465c836fa3503040c9672a
                                     f01c9d74dfd449dcacb5d978f28a82dcb4d76b44
                                     9927523c3325bf5a5a5bad69fecabead0d958f98
                                     a76d40dd0b7f27d40a9f0b2809ad30d473888194
                                     49264fb6968dc99add651300e1937c75b6165e84
                                     4beada47d698c43af92ce512a4ba805cbc39b21b
                                     385f57481ca81b1bebc1a71350d8fde3362b0f1c
                                     626ac34bc2f9f41ab55a033c34fae93ed4fbaf09
                                     6fb3650da29503ea5c5063a8fe1a25cb0754c67b
                                     8ffa6c3bd387d9cd940d4e78d51473f8827f544c
                                     9284b24c916fcca627775349e80a51d84295094d
                                     403b0482f6200a5f42dc6937dfe7f6e5d69a2ae5
                                     3e3a17de1f5de694c89527aeeb19a9460b9a86ee
                                     2c8718863922da2c21c089f07f27a1ce2b4f290d
                                     113a6d1a57d15c03a21da8a936260d567043cc56
                                     0adef535506140fa5aed81dcbad621e48c0188e6
                                     158fa3c559f0ada87caba5bc7dab45110fb28339
                                     5cfae79c465a2b801f769e9365bb284683f07c82
                                     6eb57f090b14a400d05fb0f11166935c7ff7e3fa
                                     2196226e1d6d9b54fda6688bc1f60b0f6670f0f0
                                     10d419deb131336e6490259065119e6ec724643b
                                     002785925ed953231a3f6a210069e6c9a0504585
                                     d474706577608e433588ff55f8019b441cdd80e1
                                     20a8bc0b2987f9dbf26e596d2020f4bbe8602d1c
                                     ce0bfe14966a14bd6ef4dc7977b3310d2ffd7648
                                     9a58218e6fe21ec588ebc987ffe33d7f792a97ec
                                     4aff7b8ad2eaeb3a3920c4a85b296f8d91751b7a
                                     ea3b2a1da1295c01e550e1c7e1de7f3e6339616b
                                     4d2ae7730c152df08ca074c6b1d1918e8cd76149
                                     13547b2baa0a2f093c6e3e178e87de16777a38a7
                                     fa426d21eeac155441d27bfabc8f6cf3f19bdd96
                                     02d527f3280a404a7b1c10519cc307a149369a75
                                     cff156c703fddc0f6a8861d64ca7efe0b7fd16b7
                                     42eab91f810c267075823c7e344fb1d54db7fc21
                                     f240582d5585c4191cb227ef0af68276d1862292
                                     4ee0bacb06562c602efb65aab3a0e799ecf569f9
                                     10dd409665d20ef82eb619f243c28ab568f2136c
                                     88f29ef29ac6f6c8d49438c93e25131967a19a5e
                                     083383fd129be2d5b07a8ce5f6c5e2267b05cda9
                                     8c41c43888ca8af208d1ab16018915e0424296ea
                                     0c699cea432499b56766b11ea0d485aa0be74f37
                                     57bcf34a0169aed5620438cc85d6fc432ef8e6e2
                                     ce94176ee03676e8a36c6112823e3e192219f30f
                                     1291961239fde57cfeb35ebe00f6a855f7f8bf40
                                     0306a0dcb16e137153400a8df5781173b0a17e99
                                     badf0b27b08442e96879e6cd25540aadaa11a567
                                     1393fdd3e233ce9d4c1db97a0f2c775a57b2270c
                                     c6065d4116813b650c90f06b763cb5ba99c47e80
                                     987e705ec7fe10e962d829c1fe5a2c9ac011810b
                                     b7ec3020274a8d523058784e1a787da97edee6c3
                                     3a0f257f53f60054e26505b52f6a711c17dbe30d
                                     183c8396958c944ba1569d65b13b446c30b8693b
                                     19df8bbda1a9051ea07ba1017ce9405380de1695
                                     1a62991005ed5819ca36c037232e4a2ddb11451e
                                     b466fc5238e0160895ec4231b1b2f7177517c869
                                     2b41b608bd90ff39f9b729a29bb3676ca541123f
                                     a84535e068d860eeb3b80f48bc915e434399078a
                                     b468b80a15792d8e04cc34a0082d960fcda7e7ee
                                     29ee2bde9fffedd777153dd47b71e411f851dcf0
                                     985709b2a1800a59064dc73505bbaad367d03111
                                     69fc5628939a09743fedab6726f2a5e01873073b
                                     dbb51350a33bc66a2dfa52d01421c00824d17da2
                                     855022c02df878e401655cc27d6d2b334781ecd8
                                     5753ea4e82f09128fb4624f6738245c546d98e32)
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

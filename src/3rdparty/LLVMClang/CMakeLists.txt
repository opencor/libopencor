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
                             db12e7b6d6dc7a176a2789bf4c6e68c59d19015d
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES 83dc604623659fcf331869681fafbf95e377e9d6
                                         7188e7c077ea4839b04d43fbc1310dfa53768a8e
                                         b570e8f3055bd088533ea212b5ff2c96687e1c30
                                         8152aa0ab573f1a7505eb9ca2c833fd64760701f
                                         217932443aff66ed147780d129c121b4fc8db06a
                                         3bb2a50b8817881f70d1bd6e9b3880b5ea568f06
                                         23ec7334a63b8dfe7aca16d46013b29aa050a9d0
                                         2325f41c407b466d81aa808cb3a695f9529117e7
                                         0aed706ce6eee1e330a8feb5831b2067fba608dc
                                         7b2f1cd11d866c52eac63aa2ed36abbedfc6faa8
                                         dbae2fb1ca520ff8fd0ae04d3606a41a4efca0d5
                                         3b7d52a1e5f7435f10cd12898c1f6d8f6bd8d3fe
                                         55d4b59590f09af30a400767d7daface1b6336bf
                                         b4a78201a75acfb7fb26383598fe22808a8e3f0f
                                         30d4f4d1d516410387e766642d6f6c25e029712e
                                         979eb3a50496b8d519b16c10403e715d92214457
                                         e537532782e4bb25918e82047fdef64c0b76ee1d
                                         fba985e1bde42e6833dc35ff9ffa9c5a56e9a643
                                         3880e4c49589ae76d2fcf030ea9b31972e8a61f9
                                         3aab12d7dbd75f7285120a23b50df0cf84dcec4c
                                         1664a4c8a1be32aa5fe4f33644046cfd0620ae27
                                         bd2d6873908286bc2e70206310fa69060491b4f0
                                         f3aaf04225d57bd9241bc048597127b2139126e2
                                         575b9a5d452875b68e1e5ae29f50f7948c8d4429
                                         e1f09b9a235ef6de150ab9511973f6efa569c9b7
                                         a3bf4ddb30dbc0fcc1e46eb7762670731fb90392
                                         9369f660f9998334928f44b299d5ffaf25e31694
                                         fdc6d48577a5ca516a54518ed1e2aab97d700bc0
                                         fd3832e50974904f7b67099dfdf940497c59c7ca
                                         122b8e5879cd6458f71e2a8f9a18d1e69d924a4e
                                         fe053917b8653319efff613a88a36aa1ff5125ae
                                         33842ec33563f08de5018d18025e1609a83b639f
                                         6a5279ad1e82b65818e36ca553a402ebc3a4338d
                                         520615778abb3983022d8ad5ff308299d6f3f62c
                                         a80cf6aa13e5759e1e11dc0f67de1f84e5370ee6
                                         a1415bd02cf9fea92c07c9a45222757c81a807dd
                                         1bf04b55f0480cf0d45988b947d3767a1f8e76f9
                                         d62fb190d003ed566affdc67854504e703f642aa
                                         13eb1185015b4a0a536a7576d719f244947f3ee2
                                         203756f0ba640924627c01388a46878a6b90d1c8
                                         633d0b5104067bcec45f71c310551b94e9367fd3
                                         35635dc24580f5103ef259e5a0e23fe4a0187225
                                         28d3a40a44d6f48259a292fd007d5a14b2893e43
                                         71839628a2fb7a6b23c9b3300f0967fa19e02199
                                         15ed086e967f8450f5a32e456ff497789ec16f1b
                                         f86a7a0a636c5f084a9e4861280611aa2885074b
                                         5afe1e5ae4f5502410bd25a57ddf5ae042d67426
                                         da04e780a2e8b59226d4ea375b83cc9b10961e9c
                                         f8b239dd8ce1ca49d5814a6edf3514295c252630
                                         1a4dacfa2073bbddb202915e4cc86d9f839f140d
                                         a0b3d9272cd4c54f7e54b8438917706c74014a00
                                         cc6c419b2d44a3fd48f3f4e0cf47348e0ca1de19
                                         83f1fbe51c7247bb8790082694baca10ac6458b4
                                         5903e3370c2993020783fe4e7b77d45fdfa07ab5
                                         902dd55999b14a472bc6bcf7780b7cde73a182e1
                                         5465c9f5d60d5c1e3e80f0037954ea52d3041066
                                         1e8ce527775abcd12b45804ce7564820ae310a22
                                         40ac0d3b9fead6b1cad7bcd10db7d3d6fa35d36b
                                         2691e4b6831a23a373d27dcc2843857802215412
                                         d390e6815c9a0df26c2e20beb888034929cd57db
                                         708887845c5127a5dfd50d3aef131c4ddd6d26b8
                                         70eca90251c5236a51aab415c723b52767982d18
                                         5be18f71d02d797b0be26dc7e3a98bda55effb2f
                                         5c909a68d6f727ced00bcf882eb5bcc1f99f6952
                                         1d67e7593cb251c49d68842698937bb0ee87bc49
                                         f64a41a60c639ccffad4ecfd48a592ad49ad3254
                                         30af56d377359abb46ebffd75cdf0220e582b2ca
                                         ad9ffa424f47030fa41ddaeb425f189d045e9c70
                                         dd6ca89d1d6ba098fff6afae96c93697e7c7321b
                                         dbd4e2ac0bd721cf3c57d3a701545f701efaa7d7
                                         31b9b49128a8ea8fe4b5b99f5ef2b4e0c9740ceb
                                         20fc87c63eb925cba06f46c1ef98cebc066957a7
                                         44b44e76e715ce1292817304db49f195849b5a96
                                         d43ee162333b3cbb138032574e310c9e30d52c9d
                                         550370e95452152fa1a8c79f6b42bdde71f8a260
                                         f18f7505a5c9406b7b46940260e26d289033ef8d
                                         563ce3ab1be36ec7a6fb594d31aff875aae99ca3
                                         f5baf43186d46faa23d8f9fee8f1fb753e402b8a
                                         0fbdfb431525407056ce022342ab5dc9bcb646e3
                                         6468b6f7cb361233d6ada641210647af7639486e
                                         7ede608cb5e81a79b41e081bde0dbb350439fe0e
                                         e0a60b0c92bcb66c870ae66de75176366aaaea8b
                                         0115c3dcd1cbb6f9581c0ebda6473d8b28439f4c
                                         225a12c61063c5a08322c7e532e6b762fed1d0ac
                                         a53260749baffc771f9002ab44d2a79da47d8ec2
                                         579148cf9d56d5dc527db738af45ebf0c89f4ebc
                                         16f3b421cea71265605c0811701dc4d00ae59526
                                         eddc13fbbaa7f5c74d2749302d30efa3533e0031
                                         d3909f0a4f06bd94e3c528f8442d4c3cfee623b2
                                         76181bb0a867577efcfff1eb9e47602b489c5748
                                         30929b4bd203cdebbd5807393771ce30155ac9ee
                                         52d220e1906f81e6bc2e5d1b3bb27da17ae1fe3a
                                         7fc6fdc70e38b8c106b6a8eb168ea35f48cba15f
                                         e4dd6e3982b15a25f89b6bf188b9b15140daf4bc
                                         a7e1eacc26ee62de37cffcd0c5bbeffb0c70ff1a
                                         6c4951282f16a950918ddee13f019e0ab3b471cb
                                         a1f7198677b77635bd87ce89fcc871757c5c1fea
                                         33ec2b9071757501754d388cc271fd1fd422abd3
                                         03b0b22769f9c18d9a15788ec177135024c1f03a
                                         8531ed7a57d8062100287fbd6a63da721e0c107d
                                         190adb203b37cf9fe4ff0f5af51e2799a1ed01c7
                                         cf552428c1e5d71c9eb344974b66ecbc2b883243
                                         55f17a573600d1291f70226156522176850429f3
                                         6c9020bd9a73353af31ad42cf55ece23954fdbcb
                                         63e8c598f09811675f1eb625ddcbe3489599208e
                                         d292c49267f0141c2b0a6f97877d6dc65617ce4f
                                         e74769c0319e2e06e4851c08a3ad42acac828914
                                         b919c8ba2e66a0c27cbaaf464418b66e42446c96
                                         d5063c8685d666ae6bbdb2fd497c6dba5f513baf
                                         b8bd26e30c701eb24a2b791a01858830ebdecc1e
                                         14e29c72b407b95865aea9d37c2dcf7eda322b19
                                         7307f62ddc98e25d8eb1d675cdf3680a45d4a4eb
                                         55d4706103e3a2a58b61f0678386190b8abff198)
        else()
            retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                             ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                             da57e2e9f012980509a1dd50bf6db07e9d500242
                             SHA1_FILES ${SHA1_FILES}
                             SHA1_VALUES d45265e7204d9de0ee3aa23b2990ae9cca88a662
                                         ec7f6c642d9d89e1b71e9dff015e5188fce12008
                                         4cd87688a4adb20d97d46bcec45e72be52f24446
                                         a085f2bb3822cdec581fc9b0b7a583e1fc055266
                                         5201160ed18176cfcd4716f143a80d696e4ff1d3
                                         0d84ccc2a350ff9fd25c2800e6111eba5d101280
                                         189dff7a01952fda8471aa230bd7d88695fa34e2
                                         fb92a2a6aad7aac1d8c8b1c797c9d09c1a97abdd
                                         243b6d065cebd070b2db305143a7d67d675c919e
                                         af5efe837a28c6f2dbddbdf5ad6ef6e2aa306b05
                                         41f0bbdbc514e9f7c5f61e14b7b1394c99520d64
                                         7ed703cfabda310c552ec61d9af925bc635ed2b9
                                         8b5693d71ebd784603076246596531cf45b67510
                                         788110959cd796fd1ffef3f421187baa7b1f3a88
                                         0044b62d3f5debdc5b4392bdaacc2f8810e9480c
                                         83ab3711f059b080cedb99d0860699304e9a4c55
                                         c4b69c2ea18ed509589ebfd8edf4466180f72f9b
                                         236a4337805372a998e43955158df5bca81649be
                                         4a8eefb36cbfe5fb63707c11751d75843bba5282
                                         a7775e3405109afbe04095c5e045e94077e6b762
                                         3528fe14065d1784d591cbd0d2fcb2036ee669cc
                                         7f43871d5db7af65ae65f96f427b450a4bc88743
                                         5d95f87387ec23304333955349262c17d53d2d18
                                         69b9ebf70b566f13c396bd3c3eed9c5ddc79764e
                                         c7b6ead031a42fb20b47196a3c71eedd82c377f1
                                         514b7a1b6a516d639b57a53ecc7d2d37278ed6ef
                                         cbb8558d047ffecd80c6aba8a7787a80eed08cbb
                                         66459041404195d36a7edf653f2aa32234f73629
                                         e414396c1b8b2d84381de7401a51027022e2816b
                                         0f52411ca7f26958fb4ec7cf53a328166da40638
                                         0afe4c8dfef3f41f95b81c8de71e174be132149c
                                         bc756b08adedb4823f43cff311d1b2d82c28d168
                                         898235f1a64ed632341b204cb2f95639db0be37c
                                         b1ebbcb974cdf7f6e3af70713306c195fe7257fa
                                         05dfb8a44ff351261739315a0b405e43b60711db
                                         43bbde6edcd866c9606b59fd9f073e547cd33d4a
                                         459c98bfc8761c1d8bd1f8ce7d85090960548562
                                         c59be923b1aa5a0d12fea9a3f976eb35e6035e6a
                                         f88b304a59b74814e3825ad2dc66ef5c0f880c04
                                         d4b09a81bf184708b22ac72c0571eca347c6970a
                                         465b3982efad305f83ca777ed450d5cdc1d6cbd4
                                         410ddd6f2e168636f7279bcae851371f22cd28bd
                                         617e1a7954016e81ee7c477d39f081afbd4e51ca
                                         6c14c57d25af5c1f4e88b096c38fd2c46d76252a
                                         896263b0435941f9a8a68da3020ce6c71031f0cf
                                         c95aac2360def35ed7420774eb4b537ac25c117f
                                         ac1b69eea5ff8f481a10688059642cb7e6605cd0
                                         7adbc55d6900bc1af90cb3a9669f55c28aafa51e
                                         188707f40e7ad400d05010c1cb1c07121e20058a
                                         de832cb6a08843790ae05e390fb3320441053df7
                                         1b97fc2f9146dc0979778a5e20348cf4d85fbf5f
                                         2ad6e0e6522b73e5ebaf570db7a82b9f01b87c56
                                         d723b1ad3c1bfa281b534eaafed4967fc0093628
                                         43a304b6eb4c5f62c467825fed8ea43277945371
                                         358efa1dae7cfa092b74c72efd8c667a1dbdd706
                                         99a43837b50e5ec2e217bb184fd5675f48e9bd4f
                                         33bccb378dc0e5461288cb588aaf515951386a7f
                                         774b7ec34f277d81f87d5a0084656fc727b326d9
                                         c464a613c199b80408c4d3afbc18b86c29945017
                                         84264e35ba0b5fb73e956029e508dfab9d7b7410
                                         4ba3dc49b4902646abfd394d9a1b773ccf124637
                                         ebb7c96d9e704d22cbdf2e9db987976a3f9dca9a
                                         7ebdce285f61ea51d8c144cfc6c48ec75edf7b2d
                                         8915e5c673b465ed896444fbc4f7b2e9ff84ec74
                                         61a14d24607250d847b4b6952d7cdb6b7de76460
                                         a27f5ed60664f46328e5679eedbc6e162d14e38a
                                         67e8e7af6ec6ab1a7d2fe7af081ae316ee6cd118
                                         39baa9d0c126b0c2a7d9efd8c41b52eed2d6285c
                                         5e2c6d51c1fa9cbb14867f62bb62c5e647edce39
                                         39765b9ef3a883c88ce76d87bfc99615085fb203
                                         573b738c8597750e2c340e8cc320675ae9bfd48d
                                         3d8b9f363e6140c989a0164e4598cbef6266f9c5
                                         aff5c60e491a3f1cea092fb259134e34c56256fb
                                         7130612fecc664baa306e0246c93f9bba10c44ea
                                         d282499d1797ee232d5fc504d5f1fdcd81aece70
                                         4fde2be8c618800219b9f16bae5a669569a7a630
                                         68437fdd1c6d9ba03cc6a0ff196f6b667ec499b7
                                         9c9db52ed814c7659b2f4deefa7f7a50a0020249
                                         40c79c4a42063874321916963c9e9eac1c5cdae4
                                         184575a13335eb5871f6d12b95187834711b2f25
                                         24c3310f84c508264abfc9cf03a44fbc7fe767f4
                                         ea17ba09044a279fd40b9223e6ac30327bdbf0bf
                                         5f9648fb092eabd391802fe26eadae2fb50106cd
                                         031c25ffe003c9585737ec0a4b587b975da90197
                                         6d2981a53294089594999336f577d4e211210c59
                                         99821af937e5f24fbfa94a94e656501d2641d3df
                                         f1597a71e62ea96fc500464828e32030c04f0aa1
                                         df69bf7a3618535ec6e7a707a998c1c26a0c594a
                                         74e29c5cfbb062b30f02af9fff5e7c076da513b9
                                         4b9e10a7b36263bb3be1b875ceebdc7376b25a2d
                                         82405ac7d38052e9255c34069383d228ebea72e0
                                         e383cd3095a1ecdd28c003ea7c2c5156fce2c42d
                                         afd75a346c8f8c4d69c09ea834841411a287a526
                                         3e82cefce00c9991daa6d2bf33bc654627a682e4
                                         9bb71ec5985dd977fa921c55667118f80c3793ea
                                         36f24629d93af96228885dcee41069d8b51d89b3
                                         6de50903ae8de69931f00603c3b4f2ca1f98628e
                                         6e98931406d517715621beb22fae7d7c9eaea679
                                         bf493be1166a64a7a7a81315074987843532adb3
                                         6a9f93906f4f76dfbb60fe9efce172ea03a52b15
                                         18f9fa1f98d45ba2e9534741394e9b2f445e354e
                                         5384747b8ebfd13388777a8f25d9c873dd9853d9
                                         9ade0947e7378d9b40ac29359fadff129437887b
                                         3e85fd1cab61e1de611cbebc5d25fa7da14fccff
                                         204ab775775d988c340ce59b68a63b21b6be5304
                                         7fec2d52a08a1c6b0ff70dae221a887039d69da7
                                         1936e6eef61a8e09ac9cc34540d4affc61389497
                                         7723dd1d23d37fb25090830d9f5bd95cdf504257
                                         1514f26dc4ebb4e7d7181b5e9eaf09ca18c66541
                                         eb57a6a3063a8ffd0c1dc2cc710197790edf8216
                                         cb2ed6ba573944fd6ab2de4e25ac322d22f3304a
                                         c3ead2dd3327e5a847e137489e6f3c6febb8dcff
                                         2d1a6e7fa629bba5ba000f843c9475ac86cbe00e)
        endif()
    elseif(APPLE)
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         9709baa6c5b9ccaeae85e44f6e323bd7ea63fe02
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES 94506be87ceed06ba54955bbfe8a1d8fe07df0ed
                                     99999d9b92ad1a49bbb0d51fae5a0a464fd7a7d1
                                     4250d91b084fbf16032f127d55a0921f234dcea3
                                     32f13e3d14561b8ce94f9e67d749ef3104a7be09
                                     a6163d899a6f6bcf13acde00d18d36ee0da95984
                                     7a70e61f9e04da10d6452c5d8bc80b0ef9306144
                                     ec43196614333025ebbfc840b95897de67f404fe
                                     6f9b398d49a90f88d94bbcef219649d9eff00ef2
                                     654f5a51906e983c57788a7d051de79cc0c9904d
                                     4ecc755c512e83cebae207c7489052df10ac5f06
                                     897885da19bce0ec506c57dfede549ecf309d30e
                                     15228b54c13ebbca975c150641a7bb0528c76bf7
                                     94bf83e2bee7affefbe5724a8ce792f2214e4e9f
                                     d57883a8deb7b0227e72cd44bb7ffe42e459abf8
                                     81ba1f529ce4ccdbac306d51b7ef5d62d8af5fd0
                                     03148e386db1f369fe9e522f6a76b8aa6cf7d937
                                     3d1b2975dc12ddd83a40ef1313ef58e8961894e8
                                     7a9163470311582fd680eb6ef45e1d2a58b39d68
                                     997900b948a69dcfdfa985a9e4b7cff70cfb2211
                                     8469a77eee744a6bb94b2dbed590a03a33e4b7f0
                                     b4b2662dcb3aecefde25d04f3445fe8b0b6d4645
                                     24c65491e7dc83a3848b9a8ddbb455af1fbd8ad8
                                     5c1961d78ecddbd0ee9ef672c0295373ebe44089
                                     33d59b99b992d10b530e183b178a32c5254711d9
                                     72a03ba888db1e4be43a4b38a81486a5fc16f3ba
                                     4a3b2eb5ccd652d80482f502328a3806026a5181
                                     a19c254a42873bd20f94b3b40e0f37b7f4e86188
                                     6cb4c9b8e46985ae17cf6873350ceb226bba2777
                                     40c3220063a068bd76b842b7290d533c2368e49a
                                     a29b2018f184bc2251fde02a7782913a3c9054fb
                                     ffc199a0a7db29fbf4146beb1debd5b5f4e431e6
                                     f355815970d50787efb4428b40b80a7bc9b18295
                                     14020bad3ed6d5980ebb9b926d049e638d179311
                                     7243869da184a20157c276621682189410e1aea2
                                     7d11d9dc581e57ec01dd32f73ce0848d2d9128d6
                                     934c80c20008b2a5f6ab554ab447bfcda28ed7b1
                                     d46ba056b2e3fb2bbd2c9923ba467c9755848ed0
                                     795c7ec56a1201328ae43e385b5111af2c466599
                                     57dd6f43070ce71ad916f6475d25082df5165b04
                                     50a09f2acd6aa36dd7238c0793627aec40a307f3
                                     efee20a5cb9d1190a111de283afb7ca67cb492ec
                                     1bf66e8c6dc136ad84ddbb3538743c8a073adf14
                                     0058249fb13c57c7de9dfb18678a627274af91fc
                                     9486e6c920633644fb06b20d975251743dd4c170
                                     d20d1003442e71707accfae0a161e03f9eb68e57
                                     c3821fbd4e35626e6d3fb08359105c2f8c45ca79
                                     d2ab457a8d5207dde84aaed343da8e449a93c4c1
                                     3b3b032cd20daafad53c0e54f4949783238b022e
                                     8100e04d8a793ce9057cb965a4ecf0c56d198481
                                     cfc6c03a4bf8009ad508cd2423bce85c1687f066
                                     a0a8238492a0736c2e1689602b1fd24f25a00ac8
                                     9ea6e916edd87062a75e56490ba23fe3e81480cf
                                     bdf3051a9935632e644006734b11ca0ed3e05af7
                                     80e617db6a6cb8afd797938f9d3eb90ce725213d
                                     ebe9c607ffb6db01362783eb22758dafb8b6dbf7
                                     00fca1a369589c2d6abdf04731955af760a42b11
                                     40b6dbbfc9d3bc744c109d1b352c5f9759947136
                                     ade6a2db6ce2abe692e12cc2b92562cb6115feb2
                                     02563de4748135dbab78f021d5deaf6baecc1cfc
                                     93e02a76a236ae58d0051cf3bd21a565a91d94f1
                                     4ad2bffe64e3a328e57b5f1bc201e874b00eae49
                                     18ac1adb700f24ddc5d824337688fab8da4d5572
                                     2947f37d7a1c90ffe97072b6a5798cb42af02a58
                                     fd4dee4fc2807165ffa1890b921f795b4377a031
                                     9bb45ad2664b7e887792e00f36c51ca403e7bede
                                     a6ed3a4394b5afa5759e51a55f936ab260ad17fc
                                     870bf269c9235663e55820391ed32f7fa6470511
                                     13970ff716e791ae08d9f301cd78e8d613d2045b
                                     dbaab88d10d0d471e6814e1a7ec6e832080385c6
                                     f39b3063ac938944dd12c86d12f08ca4826688fa
                                     75af8f78eec8a815e53efe9bba7d7e48fb9d9c42
                                     c0f0185be6feb1bdc46816edf5a8cd112ff5cd8d
                                     6cca6d4b49cd41f7326f754653565986f8745797
                                     85b12375e0ca3bb6789dd22b9ca96f31e73fae2c
                                     5bde9295deae65fcaaa17ad82f83929ee10302ef
                                     b5545f1b647fecdf8d561bb483189f5f1a7e0c4b
                                     bdd026a687f1503efe04bfabe8e277f050e3aea9
                                     d2ed928851d816d55428309e4b580482e8e2b77f
                                     cb54bdbe3e51a56ab821f952ee76eb3101e3474b
                                     d6742a3d18bf7e4d79c23d80feee46240b00917d
                                     141423a00e002988cc809aba0e8d4ba857cb0092
                                     d5f24b1eb6dff9f1a890848b77878fc49e830f6a
                                     6e9aa0289941d2e85f7a93c71c2edb3b877b3a83
                                     f19025e2bf538ef86998b355dcf29d5b3b8b490b
                                     bc9b90a67249973caeb9d877744ba6e0b74098fd
                                     5f2f217c007371ed3de8fac9340e96e7c7280e5c
                                     4239bcf79654288bf6f390bdac7ba4719a40a455
                                     7ba541a67a9eda1619e5b7c83edfec07238c988c
                                     2c798ad36958ccf9228241c09c7a3b5bdf9ae33f
                                     b575782151cf270a344dc8893f39dffab00e19a3
                                     97390ebb78dd37244575a5492e38f27b55776f36
                                     89d2b12e4576d297bb4573d116f6dcd8ee7f5776
                                     adcd45bc3a64b6b8d85f451130aa120bb1dfac7f
                                     14276af24348a6af35e71f2e391984c64b2e56a2
                                     54fb493317ecfac99638598793e49efb207be918
                                     3f3533b859fde19b3840c2c9e41337df4266f115
                                     d1ba6b52599abdd00fcc214b1d9247f5e4c23f6b
                                     ba492df5be3e15910b9c79cfd6298443f64fba9f
                                     39a596bd56cd6531aa348557e1f4ceca422f2b97
                                     01a78931c3f4a8084b796ec0baecdff5bb5502d6
                                     d56d01831da5af45d67c8919e5a08967d38c54d0
                                     bad88474fc9fbc64ca408b1ce660236acd70a294
                                     d587465d4902d70c8361a1f5a35a1791a81ebba3
                                     53a28961c4122ecbc2e4624a6d30d8b10579afdb
                                     0954196895569981af005dff0cb1c3b4ac7a2b45
                                     c21853956753066ebb4c51f5d6ad819bc8a0e9a5
                                     0d6b215833b0c04174fa99db0677124b95405641
                                     f11fc9a0e970ff5ac06ebace55cb1e48deeab4d1
                                     43f6a1d5d3eb058dfe33d10710a251d3ff4cd1ae
                                     1a994cceb26e48a8888b551ca2ede7d9e7ccdcbc
                                     8315fee1bc9418984ab3e8ccaf32cd6a6479dead
                                     a725b7778e94cd7e56d81eafdcb4d0a5c056250b
                                     8e0dc60c4c5116bea544d2aa9afe54ecc81be1a1)
    else()
        retrieve_package(${PACKAGE_NAME} ${PACKAGE_VERSION}
                         ${PACKAGE_REPOSITORY} ${RELEASE_TAG}
                         7474eda351688373d1d830809a816c7b1c1e5851
                         SHA1_FILES ${SHA1_FILES}
                         SHA1_VALUES bcd48cc03352e7588e3092a7ad21085cd7e3ece0
                                     00eeb0dd137b92621649e41ea03251b5b4c60eeb
                                     6e42d65c8aa765fe6a02ee4260132bae639f311d
                                     6c4019a9b5c30b3929970b23a067e8960e7a14aa
                                     349a43d1cd458e4b377768bb8e189ee0be0a6928
                                     b76ea0f8d13a25d66cb96dc818661f4ad084af89
                                     a46664e744d2dd94746267d7a46ede5b92a30cb1
                                     ca4b630672362b8f0db2a283a2d88bd79f68aabf
                                     323b7ae8ea1d0aa6c438e6237384811a890bbc5d
                                     fc840f38e4545dcc9e14c7e8ad29a055a52f76ed
                                     d1c1d1a3ec922903f408e8d79656b1f4743f4ff8
                                     d4e256c9ffa3e0a90033182a4d15716350184cbd
                                     2f223504c4546f4bc54758818ecb3264e93f03df
                                     2c80c34d4b4bf7c9eca0dffc3d409a5d99740578
                                     b90d12b20158e17279dd549e7985d6e7077e2f73
                                     2a9badbed9b38705dad0a91de8c3368db7d9dbab
                                     010f42c045de5a7221c031c0b607b072b3c5d12b
                                     b2890043123111493d6a223a7e9980d582973bf2
                                     6a7503e321f33abd2f088769fac7209c056dd263
                                     6ec490e6c2b07ba2b0e3e65c6d57f30f19fa3308
                                     5eab3f31245065919cdd5d208fd61f79a3e2ae1a
                                     1548ffed7fb2e7aaa8bcaedfa7a3f13ce31cb81d
                                     247d696e8fd51add701bed982f747c9c03585e16
                                     93933402a6ce842dbcef4e89c723455f091f11b8
                                     6f023e038c9c85b1084767d67c1e4956b128570c
                                     b6fb0236706c7038263f413da7cfe8b794adda28
                                     3b74b6d7f80d675f5c67ff2774312de2de6859f7
                                     5faea2646174eafd3dbd3a3df8bd28d14b292894
                                     5dc5340fd2d33d7a270adfe39a89c5800ddfd0e4
                                     3247a86a9968c3298f218491799b377533d4d616
                                     20519a9ea5a7fbcb5d83b29f55f0af3594f9fa0a
                                     aeafe003174513851b06e2142320d3d5aeaed668
                                     3e00452ff4a42b21955a8fd4d55f1d7a97575623
                                     833ce59ba5dcd465d1257eedf23f80851cfafafd
                                     72ba29366a6e770dcc1a8dd155d5a0c1a1e06c8a
                                     c307e56e9b2832104542e6807de26092268ac315
                                     1853d3ced76f5209dc87b5a71c98f3d71f2980ad
                                     16fa79f302344928ca33d65e859edb05e2d5aae0
                                     9c64b91e1c46660b5db8436a93d628ea16d252e5
                                     f2daeae8c2d825858db7a7750d92fc551f1fc628
                                     a5b0cda20374e66db5bef532773f6a8dde1fce06
                                     20d681855e2ec4f37c1b4ba583d4ac16705246b3
                                     21f7b91ea4276eccc31cbb05509c76ec533c2492
                                     d2186c0063fa9d0f35cb4cca8001425d3478994d
                                     a90936ee0af078f8c163f7ab6cd612cc1ab04014
                                     4e2058e55e34afa5db464a4aec4777435d94048b
                                     3d4ba957cd33ba7acb2cdaea9aa66de158aec80c
                                     5d28202b6a66907d90426cfe11e014cef39321c6
                                     2384287169fce3790908f9a0582ed7a4f2c2443b
                                     44e7ca9b566c9a535bca2c32fb9a3815308c8b50
                                     cfd8ebcbc4bbac2b276f41ab198ad6eddb90d4cf
                                     3c77a382a7590737f077f3e2734c8c308d43f37f
                                     060aa5cedd0109728622764b3326bc386843147e
                                     a25c6c9b503561e4a59a65dcc805c9e5acb1a05a
                                     9e582b8cd67678adba63daf50407d1c91112febc
                                     b12d511d58ba2694ceea335a3a347fca38222d74
                                     9c536e08102af82c3934ae673fcc552e69f704e6
                                     2f24432f57536ea4525a7f8c2fa664e69c7e567e
                                     b2a2749e5968fbf1747a0f34f231a0eeb5c6d874
                                     4cbdd22243a92f8adb0c493468577a967c504d73
                                     8686c31eee30ad1f9374658dc714ddcb4bde9003
                                     dcae5a01d28c6453131c872a9bd3033f100f499b
                                     2c8718863922da2c21c089f07f27a1ce2b4f290d
                                     a0b30fc0fb440323be7c12692a5d8f18b1272d8b
                                     0adef535506140fa5aed81dcbad621e48c0188e6
                                     33b150993627ef6e1254d13f476e9d9de2030f25
                                     e1089dd03f4e4405ecd4b12246982240e43a095e
                                     d4a0961b287706ba572a3a832e6cbf2e3dcdf4b3
                                     ccf2837ec78195bfe16a4356e31d19a89d737cc3
                                     2097744e4959592c371177c5611ef7965f15eb59
                                     5dddd10a71c3954a21928853030ba3893dff7633
                                     da62b02b39f202bb9624e0d547f4b88fcffb46a1
                                     cf04025312581cd09a11c5196bcce94fba3133f4
                                     ce0bfe14966a14bd6ef4dc7977b3310d2ffd7648
                                     2e367fb4fde069dd7d1cf15c452b2054908883bf
                                     4a23054bf5dd3ef33761abdbf49cf5f076e5813a
                                     ef5b864661aeef10ec5beffa43d07886b98e1a7a
                                     6ecaf44a9a9dcdb6e22bcccec199707e7c79d488
                                     37831c5f6491ed4740a9a8b4bb1d4737a2dacdc7
                                     3a38bdf3ad817e94a26a31a290e4d15f707599f1
                                     079e0e6d966c8f4805fd0bcfb5872e7c47908a89
                                     a86f36dea727f5a3f8ce2110c80097064a5ab209
                                     71d7374afa098c44a154f65a30422c2e431d4b3a
                                     6a1302efd079a60ba9a329c20a47339884ec54b5
                                     900241f739418c3c64b430908b9d061af79f3b0f
                                     ee1e7debabf56c21ade8affab8f29e43af0fc9b1
                                     9752209d7b25c8e27dd99eaabbeca561cf45e96f
                                     d6e21c0dcf1236f004d401bf76d07d9294108729
                                     b90eaf2f6c14ee5d48d7ff53fb3c561204e07404
                                     360022aec6db3ce8af0a082d3a17f6f62f3b3f57
                                     86734db5911d68f47c4242e354ab5bfeeefdd87f
                                     ce94176ee03676e8a36c6112823e3e192219f30f
                                     21309736f1b88e5b5b7e5f0c4efc231937299132
                                     dd3f8ab944d8010930fd21867c620df2108f01ee
                                     e034a1506c1cee0a4025af5dcf34b013ab227e93
                                     7ca2a5ae5a5983cb9a9d47afd7d3181e2f0fdfae
                                     186eb348316f03c9df9d0bf8efb3e7c323972e18
                                     fcf7497d030c6972398701a4da0cf5a2dfca19fb
                                     c01f71ab877e954e2af93917a227a9324fffbab9
                                     3d3853ef31a93ab6efb7e9a52e2c293f73a196d5
                                     21e7cca808a73e0a49559f644e3b3ff1b6c24646
                                     67c277b64c0984ddf400bb75bb53a0e1c50c053e
                                     94bdc8cd0eced9428b07541e1620026fa2bfd729
                                     def4fc6f7bb46999be50ef4251f71774e79632d8
                                     305d5806d1c42495c456a6e9c9ff1454109e851b
                                     d89c531d6daee80c8dc7b7bb24b63f0a46d18e85
                                     644d17d1c4bcb9c0f74f646acd339849c0d6817e
                                     483843bf93ac4394394142ad888d59b50e8da26c
                                     e194730d2b2b24984859c3206b0e37d2b10f90ca
                                     267fe9f0e25e35db2b5810861822b8d29d0ca2d7
                                     4892eb43497fd33a4878b4ba76adc2c911721908
                                     855022c02df878e401655cc27d6d2b334781ecd8
                                     3e79dd5e847f37361fbbae1f81beec351f155935)
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
            -DLLVM_ENABLE_PIC=OFF
            -DLLVM_ENABLE_PLUGINS=OFF
            -DLLVM_ENABLE_PROJECTS=clang
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

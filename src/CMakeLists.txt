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

# Enable compiler caching.

if(LIBOPENCOR_COMPILER_CACHING)
    if(BUILDCACHE_EXE)
        set(CMAKE_C_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${BUILDCACHE_EXE})
    elseif(CLCACHE_EXE)
        set(CLCACHEWRAPPER ${CMAKE_BINARY_DIR}/clcachewrapper)

        try_compile(CLCACHEWRAPPER_EXE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/cmake/clcachewrapper.c
                    COPY_FILE ${CLCACHEWRAPPER})

        if(CLCACHEWRAPPER_EXE)
            set(CMAKE_C_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
            set(CMAKE_CXX_COMPILER_LAUNCHER ${CLCACHEWRAPPER})
        endif()
    else()
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXE})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXE})
    endif()
endif()

# Replace some compiler flags when building using MSVC.

if(BUILDING_USING_MSVC)
    # Replace the /Zi option (i.e. enable debugging information), if present, with the /Z7 option (i.e. enable old-style
    # debugging information) otherwise caching (be it with buildcache or clcache) won't work.

    if(BUILDCACHE_EXE OR CLCACHE_EXE)
        replace_compiler_flag("/Zi" "/Z7")
    endif()
endif()

# Build our different third-party libraries.

add_subdirectory(3rdparty)

if(ONLY_BUILD_THIRD_PARTY_LIBRARIES)
    add_executable(${CMAKE_PROJECT_NAME} dummy.cpp)

    add_subdirectory(bindings)

    return()
endif()

# Configure the version file.

foreach(VERSION_PART PROJECT_VERSION_MAJOR PROJECT_VERSION_PATCH)
    string(LENGTH ${${VERSION_PART}} VERSION_PART_LENGTH)

    if(VERSION_PART_LENGTH EQUAL 1)
        set(${VERSION_PART}_PAD 0)
    endif()
endforeach()

set(LIBOPENCOR_VERSION 0x${PROJECT_VERSION_MAJOR_PAD}${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH_PAD}${PROJECT_VERSION_PATCH})
set(LIBOPENCOR_VERSION_STRING ${PROJECT_VERSION})

set(VERSION_HEADER_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in)
set(VERSION_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.h)

configure_file(${VERSION_HEADER_FILE_IN} ${VERSION_HEADER_FILE})

# Build libOpenCOR.

if(NOT WIN32 AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(GIT_API_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/filemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/issue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedabstracttask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedanalysis.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedchange.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedchangeattribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddatadescription.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddatagenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seddocument.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedinstancetask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedonestep.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedoutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedrepeatedtask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedsimulation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedsteadystate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedstyle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/sedtask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/seduniformtimecourse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solvercvode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverforwardeuler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverfourthorderrungekutta.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverheun.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverkinsol.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solvernla.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solverodefixedstep.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/solversecondorderrungekutta.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/version.h
)

set(API_EXPORT_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/api/${CMAKE_PROJECT_NAME_LC}/export.h)

set(API_HEADER_FILES
    ${GIT_API_HEADER_FILES}
    ${API_EXPORT_HEADER_FILE}
)

set(GIT_API_MODULE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module/${CMAKE_PROJECT_NAME_LC})
set(API_MODULE_FILE ${GIT_API_MODULE_FILE})

set(INTERNAL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvercvode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverfourthorderrungekutta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverheun.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverkinsol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvernla.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverodefixedstep.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solversecondorderrungekutta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.cpp
)

set(GIT_API_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedabstracttask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedanalysis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchange.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchangeattribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatadescription.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatagenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddocument.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstancetask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedonestep.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedrepeatedtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsimulation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsteadystate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedstyle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedtask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seduniformtimecourse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
)

set(GIT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/file/file_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/file/filemanager_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/issue_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger/logger_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedabstracttask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedbase_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchange_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedchangeattribute_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatadescription_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddatagenerator_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seddocument_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstance_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedinstancetask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedmodel_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedonestep_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedoutput_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedrepeatedtask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsimulation_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedsteadystate_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedstyle_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/sedtask_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sed/seduniformtimecourse_p.h
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

set(SOURCE_FILES
    ${GIT_API_SOURCE_FILES}
    ${INTERNAL_SOURCE_FILES}
)

set(INTERNAL_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solver_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvercvode_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverforwardeuler_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverfourthorderrungekutta_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverheun_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverkinsol_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solvernla_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverode_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solverodefixedstep_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/solver/solversecondorderrungekutta_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/cellml/cellmlfileruntime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/combine/combinearchive.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile_p.h
    ${CMAKE_CURRENT_SOURCE_DIR}/support/sedml/sedmlfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/unittestingexport.h
)

set(GIT_HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${THIRD_PARTY_HEADER_FILES}
    ${VERSION_HEADER_FILE_IN}
)

set(HEADER_FILES
    ${INTERNAL_HEADER_FILES}
    ${VERSION_HEADER_FILE}
)

include_directories(file
                    logger
                    misc
                    solver
                    support/cellml
                    support/combine
                    support/sedml
                    ${CMAKE_CURRENT_BINARY_DIR})

set(JAVASCRIPT_SOURCE_DIR tests/bindings/javascript)

if(EMSCRIPTEN)
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/../../../../${JAVASCRIPT_SOURCE_DIR})

    get_filename_component(JAVASCRIPT_BINDINGS_DIR "${JAVASCRIPT_BINDINGS_DIR}" REALPATH)
else()
    set(JAVASCRIPT_BINDINGS_DIR ${CMAKE_BINARY_DIR}/${JAVASCRIPT_SOURCE_DIR})
endif()

if(EMSCRIPTEN)
    add_executable(${CMAKE_PROJECT_NAME}
                   ${API_HEADER_FILES}
                   ${SOURCE_FILES}
                   ${HEADER_FILES})

    # Set the flags for our generated .js file.
    # Note #1: we use -O3 to get the best performance, -flto to enable link-time optimization, -sALLOW_MEMORY_GROWTH to
    #          allow the memory to grow dynamically, -sASSERTIONS=0 to disable assertions (which are not needed in
    #          production), -sEXPORT_ES6=1 to export ES6 classes, -sEXPORTED_FUNCTIONS to export the functions we need,
    #          -sINLINING_LIMIT to increase the inlining limit, -sMODULARIZE=1 to generate a module, -sSINGLE_FILE=1 to
    #          generate a single file, -sSTACK_SIZE to increase the stack size, and --bind to generate the necessary
    #          bindings.
    # Note #2: by default, Emscripten uses 64KB of stack, but this is not enough for "complex" models such as the Zhang
    #          et al. model (see https://github.com/agarny/COR/blob/master/Models/zhang_SAN_model_2000_all.cellml). For
    #          that model, we need about 108KB of stack. However, Emscripten originally used to use 5MB of stack (see
    #          https://github.com/emscripten-core/emscripten/pull/18191/files). So, here, we are using 4MB of stack,
    #          which should be more than enough for any model.
    # Note #3: we originally used to generate both a .wasm and a .js file, but this doesn't work when trying to use
    #          libOpenCOR in a Vue project (see https://stackoverflow.com/q/73579436 for a similar issue in a React
    #          project). The solution is to have only a .js file, which can be done using either -sSINGLE_FILE=1 or
    #          -sWASM=0. To use -sSINGLE_FILE=1 embeds the WASM in the .js file, but it means that neither pthreads nor
    #          WASM workers can be used (see https://emscripten.org/docs/api_reference/wasm_workers.html), which may be
    #          an issue the day we want to run simulations in their own thread. To use -sWASM=0 results in a JavaScript
    #          file that can be used with asm.js, which has been superseded by WASM. And, indeed, although the resulting
    #          .js file works fine in our playground, it crashes our JavaScript tests. So, at this stage, we are going
    #          with -sSINGLE_FILE=1, but we may have to look for another solution someday.

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                          COMPILE_FLAGS "-O3 -DNDEBUG"
                          LINK_FLAGS "-O3 -flto -sALLOW_MEMORY_GROWTH -sASSERTIONS=0 -sEXPORT_ES6=1 -sEXPORTED_FUNCTIONS=[_free,_malloc,_memset,_pow,_sqrt,_fabs,_exp,_log,_log10,_ceil,_floor,_fmin,_fmax,_fmod,_sin,_cos,_tan,_sinh,_cosh,_tanh,_asin,_acos,_atan,_asinh,_acosh,_atanh] -sEXPORTED_RUNTIME_METHODS=HEAPU8 -sINLINING_LIMIT -sMODULARIZE=1 -sSINGLE_FILE=1 -sSTACK_SIZE=4MB --bind")

    # Package our JavaScript bindings, if possible.

    find_program(NPM_EXE NAMES ${PREFERRED_NPM_NAMES} npm)

    if(NPM_EXE)
        set(TGZ_FILENAME ${REAL_CMAKE_PROJECT_NAME}-${PROJECT_VERSION}.tgz)
        set(TGZ_FILE ${CMAKE_BINARY_DIR}/../../../../${TGZ_FILENAME})

        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E make_directory dist
                           COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js dist
                           COMMAND ${CMAKE_COMMAND} -E copy package.json dist
                           COMMAND ${NPM_EXE} pack
                           COMMAND ${CMAKE_COMMAND} -E copy opencor-${TGZ_FILENAME} ${TGZ_FILE})
    else()
        message(WARNING "npm could not be found, so libOpenCOR's JavaScript bindings could not be packaged.")
    endif()

    # Copy our generated .js file over so that it can be deployed.

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory wasm
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js
                                                        wasm/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}.js)

    # Copy our generated .js file over so that it can be used by our test Web page.

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_PROJECT_NAME}.js
                                                        ${JAVASCRIPT_BINDINGS_DIR}/${CMAKE_PROJECT_NAME}.js)

    # Let people know how to run our test Web page.

    if(LIBOPENCOR_JAVASCRIPT_UNIT_TESTING)
        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "-- JavaScript test Web page has been copied to: ${JAVASCRIPT_BINDINGS_DIR}/index.html"
                           COMMAND ${CMAKE_COMMAND} -E echo "                          and can be run using: emrun --browser chrome ${JAVASCRIPT_BINDINGS_DIR}/index.html")
    endif()
else()
    # If we are building a static version of libOpenCOR then we want to extract the object files from our various
    # packages and merge them into libOpenCOR. Indeed, a static library doesn't include its dependencies (it "only"
    # references them). So, we need to merge those dependencies into libOpenCOR and, sadly, there is no (easy?) way to
    # do this using CMake, so we must do it ourselves. Now, one (big!) downside of this approach is that it blows up the
    # size of libOpenCOR since it also merges parts of our dependencies that we don't actually need. So much so that we
    # cannot generate a debug version of libOpenCOR on Windows since it would result in a static library that is more
    # than 4GB in size (and MSVC's linker cannot create static libraries that big). This is clearly far from ideal, but
    # this is the only way to have a static version of libOpenCOR that can be used without having to "manually" link
    # against all of its dependencies.

    if(NOT LIBOPENCOR_SHARED_LIBS)
        # Retrieve the path to our various packages.

        set(PACKAGES_DIR ${CMAKE_BINARY_DIR}/packages)

        file(GLOB_RECURSE PACKAGES ${PREBUILT_DIR}/*${CMAKE_STATIC_LIBRARY_SUFFIX})

        # Extract the object files from our various packages.

        set(PACKAGES_OBJECT_FILES)

        foreach(PACKAGE ${PACKAGES})
            get_filename_component(PACKAGE_NAME ${PACKAGE} NAME_WE)

            set(PACKAGE_DIR ${PACKAGES_DIR}/${PACKAGE_NAME})

            file(MAKE_DIRECTORY ${PACKAGE_DIR})

            if(WIN32)
                if(7Z_EXE)
                    execute_process(COMMAND ${7Z_EXE} e ${PACKAGE} -bso0
                                    WORKING_DIRECTORY ${PACKAGE_DIR})
                else()
                    execute_process(COMMAND ${CMAKE_AR} /NOLOGO /LIST ${PACKAGE}
                                    OUTPUT_VARIABLE PACKAGE_FILES
                                    OUTPUT_STRIP_TRAILING_WHITESPACE)

                    string(REPLACE "\n" ";" PACKAGE_FILES ${PACKAGE_FILES})

                    foreach(PACKAGE_FILE ${PACKAGE_FILES})
                        string(REGEX MATCH ".*\\${CMAKE_CXX_OUTPUT_EXTENSION}$" IS_OBJECT_FILE ${PACKAGE_FILE})

                        if(IS_OBJECT_FILE)
                            execute_process(COMMAND ${CMAKE_AR} /NOLOGO /EXTRACT:${PACKAGE_FILE} ${PACKAGE}
                                            WORKING_DIRECTORY ${PACKAGE_DIR})
                        endif()
                    endforeach()
                endif()
            else()
                execute_process(COMMAND ${CMAKE_AR} -x ${PACKAGE}
                                WORKING_DIRECTORY ${PACKAGE_DIR})
            endif()

            file(GLOB_RECURSE PACKAGE_OBJECT_FILES ${PACKAGE_DIR}/*${CMAKE_CXX_OUTPUT_EXTENSION})

            list(APPEND PACKAGES_OBJECT_FILES ${PACKAGE_OBJECT_FILES})
        endforeach()
    endif()

    add_library(${CMAKE_PROJECT_NAME}
                ${API_HEADER_FILES}
                ${SOURCE_FILES}
                ${HEADER_FILES}
                ${PACKAGES_OBJECT_FILES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
                          CXX_VISIBILITY_PRESET hidden
                          DEBUG_POSTFIX d
                          PREFIX ""
                          VISIBILITY_INLINES_HIDDEN ON)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/api>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>)
target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/${CMAKE_PROJECT_NAME_LC}/module>
                           $<INSTALL_INTERFACE:include/>
                           $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME_LC}/module>)

# Configure libOpenCOR.

configure_target(${CMAKE_PROJECT_NAME})

# Generate the export header file, or don't use any if we are building our JavaScript bindings.

if(EMSCRIPTEN)
    configure_file(noexports.h ${API_EXPORT_HEADER_FILE} COPYONLY)
else()
    include(GenerateExportHeader)

    generate_export_header(${CMAKE_PROJECT_NAME}
                           EXPORT_FILE_NAME ${API_EXPORT_HEADER_FILE}
                           BASE_NAME LIBOPENCOR)
endif()

# Copy over our export header file for unit testing.

configure_file(unittestingexport.h unittestingexport.h COPYONLY)

# Don't allow unresolved symbols.
# Note: MSVC doesn't allow unresolved symbols, so we only need to look for them if we are building using GNU or Clang.

if(BUILDING_USING_GNU)
    add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,--no-undefined)
elseif(BUILDING_USING_CLANG)
    # On macOS, only use the -Wl,--no-undefined option if we are not using Xcode 15.0 or later since otherwise the
    # linker is going to generate some warnings about -undefined error being deprecated.

    if(APPLE)
        # Get the Xcode version.
        # Note: CMake has XCODE_VERSION, but it's only available when using the Xcode generator. So, we need to get the
        #       Xcode version ourselves. And, depending on how Xcode was installed, we may need to get the Xcode version
        #       using softwareupdate.

        execute_process(COMMAND xcodebuild -version
                        OUTPUT_VARIABLE XCODE_VERSION
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        RESULT_VARIABLE RESULT
                        ERROR_QUIET)

        if(RESULT EQUAL 0)
            string(REPLACE "Xcode " "" XCODE_VERSION ${XCODE_VERSION})
        else()
            execute_process(COMMAND softwareupdate --history
                            COMMAND awk "/Command Line Tools for Xcode/ {print \$6}"
                            COMMAND tail -1
                            OUTPUT_VARIABLE XCODE_VERSION
                            OUTPUT_STRIP_TRAILING_WHITESPACE
                            ERROR_QUIET)
        endif()
    else()
        set(XCODE_VERSION 0)
    endif()

    if(XCODE_VERSION VERSION_LESS 15.0)
        add_linker_flags(${CMAKE_PROJECT_NAME} -Wl,-undefined,error)
    endif()
endif()

# Get ready for code coverage.

if(LIBOPENCOR_CODE_COVERAGE)
    add_compiler_flags(${CMAKE_PROJECT_NAME} ${CODE_COVERAGE_COMPILER_FLAGS})
    add_linker_flags(${CMAKE_PROJECT_NAME} ${CODE_COVERAGE_LINKER_FLAGS})
endif()

# Build our different bindings.

add_subdirectory(bindings)

list(APPEND GIT_SOURCE_FILES
    ${JAVASCRIPT_SOURCE_FILES}
    ${PYTHON_SOURCE_FILES}
)

# Install libOpenCOR itself.

if(APPLE)
    # On macOS, we need to specify the COMPONENT option otherwise our package will be empty.
    # Note: we don't want to specify the COMPONENT option on Windows since NSIS will otherwise make it a selectable
    #       option which we don't want.

    set(LIBOPENCOR_COMPONENT_OPTION COMPONENT Runtime)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        ${LIBOPENCOR_COMPONENT_OPTION}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(FILES ${API_HEADER_FILES}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC})

install(FILES ${API_MODULE_FILE}
        DESTINATION include/${CMAKE_PROJECT_NAME_LC}/module)

# Install libOpenCOR's CMake files.

include(CMakePackageConfigHelpers)

if(WIN32)
    set(CMAKE_PACKAGE_DIR cmake/${CMAKE_PROJECT_NAME})
else()
    set(CMAKE_PACKAGE_DIR lib/cmake/${CMAKE_PROJECT_NAME})
endif()

if(NOT LIBOPENCOR_SHARED_LIBS)
    # Patch the generated libOpenCORTargets.cmake file by removing the INTERFACE_LINK_LIBRARIES line. Indeed, it lists
    # all the third-party libraries used by libOpenCOR. However, the way we build the static version of libOpenCOR (so
    # that it embeds all of its dependencies) means that we don't need and, in fact, don't want to list them. (Don't
    # want because otherwise CMake might complain that it cannot find libOpenCOR's "dependencies".)
    # Note: rather than patching libOpenCORTargets.cmake, we could use
    #         set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES INTERFACE_LINK_LIBRARIES "")
    #       but this will fail our Python bindings. So, we patch libOpenCORTargets.cmake instead.

    set(PATCH_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/patch.cmake)

    configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/patch.cmake.in ${PATCH_CMAKE_FILE} @ONLY)

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -P ${PATCH_CMAKE_FILE})
endif()

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
        FILE ${CMAKE_PROJECT_NAME}Targets.cmake
        DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_CMAKE_IN_FILE ${PROJECT_SOURCE_DIR}/cmake/packaging/config.cmake.in)
set(CONFIG_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake)

configure_package_config_file(${CONFIG_CMAKE_IN_FILE} ${CONFIG_CMAKE_FILE}
                              INSTALL_DESTINATION ${CMAKE_PACKAGE_DIR})

set(CONFIG_VERSION_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake)

write_basic_package_version_file(${CONFIG_VERSION_CMAKE_FILE}
                                 COMPATIBILITY AnyNewerVersion)

install(FILES ${CONFIG_CMAKE_FILE}
              ${CONFIG_VERSION_CMAKE_FILE}
        DESTINATION ${CMAKE_PACKAGE_DIR})

# Uninstall libOpenCOR.

set(UNINSTALL_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

configure_file(${PROJECT_SOURCE_DIR}/cmake/packaging/uninstall.cmake.in ${UNINSTALL_CMAKE_FILE} @ONLY)

add_target(uninstall
           COMMAND ${CMAKE_COMMAND} -P ${UNINSTALL_CMAKE_FILE}
           COMMENT "Uninstall the project...")

# Share some file information with the rest of our building system.

set(GIT_API_HEADER_FILES ${GIT_API_HEADER_FILES} PARENT_SCOPE)
set(GIT_API_MODULE_FILE ${GIT_API_MODULE_FILE} PARENT_SCOPE)
set(GIT_SOURCE_FILES ${GIT_SOURCE_FILES} PARENT_SCOPE)
set(GIT_HEADER_FILES ${GIT_HEADER_FILES} PARENT_SCOPE)

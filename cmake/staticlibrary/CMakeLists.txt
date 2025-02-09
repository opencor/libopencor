cmake_minimum_required(VERSION 3.15)

project(libOpenCOR)

# Retrieve the path to the prebuilt libraries and that of libOpenCOR.

set(LIBRARIES_DIR ${CMAKE_BINARY_DIR}/libraries)

file(GLOB_RECURSE LIBRARIES "${PREBUILT_DIR}/*${CMAKE_STATIC_LIBRARY_SUFFIX}")

list(APPEND LIBRARIES ${LIBOPENCOR_STATIC_LIBRARY})

# Extract the object files from our various libraries.

set(OBJECT_FILES)

foreach(LIBRARY ${LIBRARIES})
    get_filename_component(LIBRARY_NAME ${LIBRARY} NAME_WE)

    set(LIBRARY_DIR ${LIBRARIES_DIR}/${LIBRARY_NAME})

    file(MAKE_DIRECTORY ${LIBRARY_DIR})

    if(WIN32)
        execute_process(COMMAND ${CMAKE_AR} /NOLOGO /LIST ${LIBRARY}
                        OUTPUT_VARIABLE EXTRACTED_FILES
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

        string(REPLACE "\n" ";" EXTRACTED_FILES ${EXTRACTED_FILES})

        foreach(EXTRACTED_FILE ${EXTRACTED_FILES})
            # Check the extracted file name has an extension of .obj.

            string(REGEX MATCH ".*\\${CMAKE_CXX_OUTPUT_EXTENSION}$" IS_OBJECT_FILE ${EXTRACTED_FILE})

            if(IS_OBJECT_FILE)
                execute_process(COMMAND ${CMAKE_AR} /NOLOGO /EXTRACT:${EXTRACTED_FILE} ${LIBRARY}
                                WORKING_DIRECTORY ${LIBRARY_DIR})
            else()
                message(WARNING "The extracted file '${EXTRACTED_FILE}' from the library '${LIBRARY}' is not an object file.")
            endif()
        endforeach()
    else()
        execute_process(COMMAND ${CMAKE_AR} -x ${LIBRARY}
                        WORKING_DIRECTORY ${LIBRARY_DIR})
    endif()

    file(GLOB_RECURSE OBJ_FILES ${LIBRARY_DIR}/*${CMAKE_CXX_OUTPUT_EXTENSION})

    list(APPEND OBJECT_FILES ${OBJ_FILES})
endforeach()

# Specify the target architecture.

if(APPLE)
    if("${TARGET_ARCHITECTURE}" STREQUAL "Intel")
        set(CMAKE_OSX_ARCHITECTURES x86_64)
    else()
        set(CMAKE_OSX_ARCHITECTURES arm64)
    endif()
endif()

# Create a new static library for libOpenCOR.

add_library(${CMAKE_PROJECT_NAME} STATIC
            src/dummy.cpp
            ${OBJECT_FILES})

# Rename our new static library.

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${LIBOPENCOR_STATIC_LIBRARY})

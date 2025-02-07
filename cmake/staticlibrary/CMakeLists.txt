cmake_minimum_required(VERSION 3.15)

project(StaticLibrary)

set(LIBRARIES_DIR ${CMAKE_BINARY_DIR}/libraries)

file(GLOB_RECURSE LIBRARIES "${PREBUILT_DIR}/*${CMAKE_STATIC_LIBRARY_SUFFIX}")

list(APPEND LIBRARIES ${LIBOPENCOR_STATIC_LIBRARY})

set(OBJECT_FILES)

foreach(LIBRARY ${LIBRARIES})
    get_filename_component(LIBRARY_NAME ${LIBRARY} NAME_WE)

    set(LIBRARY_DIR ${LIBRARIES_DIR}/${LIBRARY_NAME})

    file(MAKE_DIRECTORY ${LIBRARY_DIR})

    if(WIN32)
        execute_process(COMMAND ${CMAKE_AR} /NOLOGO /LIST ${LIBRARY}
                        OUTPUT_VARIABLE OBJ_FILES
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

        string(REPLACE "\n" ";" OBJ_FILES ${OBJ_FILES})

        foreach(OBJ_FILE ${OBJ_FILES})
            execute_process(COMMAND ${CMAKE_AR} /NOLOGO /EXTRACT:${OBJ_FILE} ${LIBRARY}
                            WORKING_DIRECTORY ${LIBRARY_DIR})
        endforeach()
    else()
        execute_process(COMMAND ${CMAKE_AR} -x ${LIBRARY}
                        WORKING_DIRECTORY ${LIBRARY_DIR})
    endif()

    file(GLOB_RECURSE OBJ_FILES ${LIBRARY_DIR}/*${CMAKE_CXX_OUTPUT_EXTENSION})

    list(APPEND OBJECT_FILES ${OBJ_FILES})
endforeach()

if(WIN32)
    execute_process(COMMAND ${CMAKE_AR} /NOLOGO /OUT:${LIBOPENCOR_STATIC_LIBRARY} ${OBJECT_FILES})
else()
    execute_process(COMMAND ${CMAKE_AR} -r ${LIBOPENCOR_STATIC_LIBRARY} ${OBJECT_FILES})
endif()

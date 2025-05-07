cmake_minimum_required(VERSION 3.15)

project(testInstall)

set(CMAKE_CXX_STANDARD 20)

find_package(libOpenCOR REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC libOpenCOR)

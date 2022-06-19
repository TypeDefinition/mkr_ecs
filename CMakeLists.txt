# CMake Version
cmake_minimum_required(VERSION 3.17)

# Project Name
project("mkr_ecs_lib")

# C++ Options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-std=c++20 -O3")
# set(CMAKE_CXX_FLAGS "-std=c++20 -Og -fsanitize=thread")
# set(CMAKE_EXE_LINKER_FLAGS "-fsanitize=thread")

# Add Library
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES true CONFIGURE_DEPENDS
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.hpp"
        "${SRC_DIR}/*.cpp")
add_library(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})

# Download Submodules
find_package(Git)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    # Update Submodules
    option(GIT_SUBMODULE "check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}")
        endif()
    endif()
endif()

# External Dependencies
add_subdirectory(external/mkr_common_lib)
target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/mkr_common_lib/include)
target_link_libraries(${PROJECT_NAME}
        PUBLIC mkr_common_lib)
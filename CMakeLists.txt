cmake_minimum_required(VERSION 3.23.2)
project(mkr_ecs)

set(CMAKE_CXX_STANDARD 23)

# Source Files
set(SRC_DIR "src")
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES true CONFIGURE_DEPENDS
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.hpp"
        "${SRC_DIR}/*.cpp")
add_library(${PROJECT_NAME} ${SRC_FILES})

# External Dependencies
include(FetchContent)
FetchContent_Declare(mkr_common GIT_REPOSITORY https://github.com/TypeDefinition/mkr_common.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_common)

# Target
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC mkr_common)

# Test
enable_testing()
add_subdirectory(test)

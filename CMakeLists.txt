# https://github.com/selyunin/gtest_submodule/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.23)

########################################
# Project setup
########################################
set(CMAKE_CXX_STANDARD 23)
project(pretty_errors)

########################################
# Define include directories
########################################
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

########################################
# Source files
########################################
set(SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/library.cpp)

########################################
# Compile source files into a library
########################################
add_library(${PROJECT_NAME}_lib ${SRC_FILES})

########################################
# Testing
########################################
add_subdirectory(./libs/googletest)

########################################
# Testing
########################################
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

########################################
# Test files
########################################
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

########################################
# Unit Tests
########################################
add_executable(${PROJECT_NAME}_tests ${TEST_SRC_FILES})

########################################
# Standard linking to gtest stuff.
########################################
target_link_libraries(${PROJECT_NAME}_tests gtest gtest_main)

########################################
# Extra linking for the project.
########################################
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME}_lib)

add_test(UnitTests ${PROJECT_NAME}_tests)

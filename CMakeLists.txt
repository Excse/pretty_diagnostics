cmake_minimum_required(VERSION 3.28.1)

project(pretty_diagnostics VERSION 0.1.0 DESCRIPTION "A library that prints pretty formatted error messages." LANGUAGES CXX)
set(PROJECT_CAMEL_NAME PrettyDiagnostics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE
        src/file.cpp
        src/file_group.cpp
        src/label.cpp
        src/label_group.cpp
        src/report.cpp
        src/span.cpp
        src/printer.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(Installing)
include(GTest)
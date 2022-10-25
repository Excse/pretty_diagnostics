cmake_minimum_required(VERSION 3.23)
project(pretty_diagnostics)

set(${PROJECT_NAME}_version_major 0 CACHE STRING "major version" FORCE)
set(${PROJECT_NAME}_version_minor 1 CACHE STRING "minor version" FORCE)
set(${PROJECT_NAME}_version_patch 0 CACHE STRING "patch version" FORCE)
set(${PROJECT_NAME}_version "${${PROJECT_NAME}_version_major}.${${PROJECT_NAME}_version_minor}.${${PROJECT_NAME}_version_patch}"
        CACHE STRING "full version")

include(cmake/configuration.cmake)

add_subdirectory(src)
#if(${PROJECT_NAME}_build_tests)
    enable_testing()
    add_subdirectory(test)
#endif()

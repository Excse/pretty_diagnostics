set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Werror -Wshadow -Wsign-conversion -Wno-int-in-bool-context -std=c++23")
set(CMAKE_CXX_STANDARD 23)

set(${PROJECT_NAME}_header_path ${${PROJECT_NAME}_SOURCE_DIR}/include/pretty_diagnostics)
file(GLOB ${PROJECT_NAME}_headers ${${PROJECT_NAME}_header_path}/*.h)

set(${PROJECT_NAME}_source_path ${${PROJECT_NAME}_SOURCE_DIR}/src/pretty_diagnostics)
file(GLOB ${PROJECT_NAME}_sources ${${PROJECT_NAME}_source_path}/*.cpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers}
        pretty_diagnostics/report.cpp
        ../include/pretty_diagnostics/report.h
        pretty_diagnostics/span.cpp
        ../include/pretty_diagnostics/span.h
        pretty_diagnostics/label.cpp
        ../include/pretty_diagnostics/label.h
        pretty_diagnostics/file.cpp
        ../include/pretty_diagnostics/file.h
        pretty_diagnostics/label_group.cpp
        ../include/pretty_diagnostics/label_group.h)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${${PROJECT_NAME}_header_path})

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}-${${PROJECT_NAME}_version}")

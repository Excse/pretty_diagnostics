set(CMAKE_CXX_STANDARD 23)

set(${PROJECT_NAME}_header_path ${${PROJECT_NAME}_SOURCE_DIR}/include/pretty_diagnostics)
set(${PROJECT_NAME}_headers
        ${${PROJECT_NAME}_header_path}/library.h
        ${${PROJECT_NAME}_header_path}/utils.h)

set(${PROJECT_NAME}_source_path ${${PROJECT_NAME}_SOURCE_DIR}/src/pretty_diagnostics)
set(${PROJECT_NAME}_sources
        ${${PROJECT_NAME}_source_path}/library.cpp
        ${${PROJECT_NAME}_source_path}/utils.cpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${${PROJECT_NAME}_header_path})

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}-${pretty_diagnostics_version}")

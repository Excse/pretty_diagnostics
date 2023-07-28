set(CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Werror -Wshadow -Wsign-conversion -Wno-int-in-bool-context -std=c++23")
set(CMAKE_CXX_STANDARD 23)

set(${PROJECT_NAME}_header_path ${${PROJECT_NAME}_SOURCE_DIR}/include/${PROJECT_NAME})
file(GLOB ${PROJECT_NAME}_headers ${${PROJECT_NAME}_header_path}/*.h)

list(APPEND ${PROJECT_NAME}_headers ../libs/termcolor/termcolor.hpp)

set(${PROJECT_NAME}_source_path ${${PROJECT_NAME}_SOURCE_DIR}/src/${PROJECT_NAME})
file(GLOB ${PROJECT_NAME}_sources ${${PROJECT_NAME}_source_path}/*.cpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE ${${PROJECT_NAME}_header_path})

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}-${${PROJECT_NAME}_version}")

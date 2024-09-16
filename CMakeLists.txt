cmake_minimum_required(VERSION 3.28)
project(pretty_diagnostics VERSION 0.1.0)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the output directories for libraries and executables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define the library
add_library(pretty_diagnostics STATIC
        src/file.cpp
        src/file_group.cpp
        src/label.cpp
        src/label_group.cpp
        src/report.cpp
        src/span.cpp)

target_include_directories(pretty_diagnostics PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Export the target for local use in other projects
export(TARGETS pretty_diagnostics FILE PrettyDiagnosticsTargets.cmake)

# Install the library
install(TARGETS pretty_diagnostics
        EXPORT PrettyDiagnosticsTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# Install public headers
install(DIRECTORY include/pretty_diagnostics DESTINATION include)

# Install the CMake config files for find_package()
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/PrettyDiagnosticsConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PrettyDiagnosticsConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/PrettyDiagnosticsConfig.cmake"
        INSTALL_DESTINATION lib/cmake/pretty_diagnostics
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/PrettyDiagnosticsConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/PrettyDiagnosticsConfigVersion.cmake"
        DESTINATION lib/cmake/pretty_diagnostics
)

install(EXPORT PrettyDiagnosticsTargets
        FILE PrettyDiagnosticsTargets.cmake
        NAMESPACE PrettyDiagnostics::
        DESTINATION lib/cmake/pretty_diagnostics
)

# ================================
# Google Test Integration
# ================================

# Enable testing in the project
enable_testing()

include(FetchContent)
FetchContent_Declare(
        google_test
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(google_test)

# Fetch and build GoogleTest
FetchContent_MakeAvailable(google_test)

# Add the test executable for your library
add_executable(test_pretty_diagnostics
        test/span.cpp
        test/report.cpp
)

# Link your library and GTest to the test executable
target_link_libraries(test_pretty_diagnostics PRIVATE
        pretty_diagnostics
        gtest
        gtest_main
)

# Add the test to CTest
add_test(NAME test_pretty_diagnostics COMMAND test_pretty_diagnostics)

# Pass the path to the test directory to your test code
target_compile_definitions(test_pretty_diagnostics PRIVATE TEST_DATA_PATH="${CMAKE_SOURCE_DIR}/test/data/")
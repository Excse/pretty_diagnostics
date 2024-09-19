# ================================
# Google Test Integration
# ================================

enable_testing()

FetchContent_Declare(
        google_test
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)

FetchContent_MakeAvailable(google_test)

add_executable(test_${PROJECT_NAME}
        test/span.cpp
        test/report.cpp
)

target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME} gtest gtest_main)

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})

# Pass the path to the test directory to your test code
target_compile_definitions(test_${PROJECT_NAME} PRIVATE TEST_DATA_PATH="${CMAKE_SOURCE_DIR}/test/data/")
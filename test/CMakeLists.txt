include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB ${PROJECT_NAME}_tests ./*.cpp)

add_executable(${PROJECT_NAME}_test ${${PROJECT_NAME}_tests})

target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} gtest gtest_main)

target_include_directories(${PROJECT_NAME}_test PRIVATE
        "${${PROJECT_NAME}_SOURCE_DIR}/include/pretty_diagnostic")
target_include_directories(${PROJECT_NAME}_test PRIVATE
        ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_test(UnitTests ${PROJECT_NAME}_tests)

set(PROJECT_TARGETS_NAME ${PROJECT_CAMEL_NAME}Targets CACHE INTERNAL "")

export(TARGETS ${PROJECT_NAME} FILE "${PROJECT_TARGETS_NAME}.cmake")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_TARGETS_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION include)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CAMEL_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_CAMEL_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CAMEL_NAME}Config.cmake"
        INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CAMEL_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_CAMEL_NAME}ConfigVersion.cmake"
        DESTINATION "lib/cmake/${PROJECT_NAME}"
)

install(EXPORT ${PROJECT_TARGETS_NAME}
        FILE "${PROJECT_TARGETS_NAME}.cmake"
        NAMESPACE ${PROJECT_CAMEL_NAME}::
        DESTINATION "lib/cmake/${PROJECT_NAME}"
)
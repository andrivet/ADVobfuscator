cmake_minimum_required(VERSION 3.14)
project(advobfuscator VERSION 2.1.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(advobfuscator INTERFACE)

target_include_directories(advobfuscator INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS advobfuscator EXPORT advobfuscatorTargets)
install(EXPORT advobfuscatorTargets
        FILE advobfuscatorTargets.cmake
        NAMESPACE advobfuscator::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/advobfuscator
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/mylibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AdvobfuscatorConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/advobfuscatorConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/advobfuscator
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/advobfuscatorConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/advobfuscatorConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/advobfuscator
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

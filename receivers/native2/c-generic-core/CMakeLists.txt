# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)
project(zscript-generic-core
    VERSION     ${ZSCRIPT_CORE_VERSION}
    LANGUAGES   C
)

find_package(zscript-model-components REQUIRED)

###### Compiler configuration
add_library(zs_core_compiler_flags INTERFACE)
target_compile_features(zs_core_compiler_flags
    INTERFACE   c_std_11
)

###### Compile and Testing

# Create main target for this Project
add_library(ZscriptGenericCore INTERFACE)
add_subdirectory(src/main/c)

target_link_libraries(ZscriptGenericCore
    INTERFACE
        ZscriptSub
        zscript::model-components::ZscriptModelComponents
)

include(CTest)
add_subdirectory(src/test/c)

###### Installation

# Install the target (header-only library) and headers
install(TARGETS     ZscriptGenericCore
    EXPORT      zscript-generic-core-targets
    FILE_SET    HEADERS
    DESTINATION include
)

# Export targets and configuration file for downstream users
#install(EXPORT      zscript-generic-core-targets
#    FILE        zscript-generic-core-targets.cmake
#    NAMESPACE   zscript::generic-core::
#    DESTINATION lib/cmake/zscript-generic-core
#)

# Create the package configuration file
#include(CMakePackageConfigHelpers)

#configure_package_config_file(
#    ${CMAKE_CURRENT_LIST_DIR}/src/main/cmake/zscript-generic-core-config.cmake.in
#    ${CMAKE_BINARY_DIR}/zscript-generic-core-config.cmake
#    INSTALL_DESTINATION lib/cmake/zscript-generic-core
#)

#install(
#    FILES ${CMAKE_BINARY_DIR}/zscript-generic-core-config.cmake
#    DESTINATION lib/cmake/zscript-generic-core
#)

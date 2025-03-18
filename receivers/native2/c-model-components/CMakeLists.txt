# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)

project(zscript-model-components
    VERSION     ${ZSCRIPT_CORE_VERSION}
    LANGUAGES   C
)

###### Compiler configuration
add_library(zs_core_compiler_flags INTERFACE)
target_compile_features(zs_core_compiler_flags
    INTERFACE   c_std_11
)

###### Compile and Testing

add_library(ZscriptModelComponents INTERFACE)
target_include_directories(ZscriptModelComponents INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/target/generated-sources/c>
    $<INSTALL_INTERFACE:include>
)

file(GLOB_RECURSE GENERATED_HEADERS
    "${CMAKE_SOURCE_DIR}/target/generated-sources/c/*.h"
)

target_sources(ZscriptModelComponents INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_SOURCE_DIR}/target/generated-sources/c
    FILES ${GENERATED_HEADERS}
)

include(CTest)
add_subdirectory(src/test/c)

###### Installation

# Install the target (header-only library) and headers
install(TARGETS     ZscriptModelComponents
    EXPORT      zscript-model-components-targets
    FILE_SET    HEADERS
    DESTINATION include
)

# Export targets and configuration file for downstream users
install(EXPORT      zscript-model-components-targets
    FILE        zscript-model-components-targets.cmake
    NAMESPACE   zscript::model-components::
    DESTINATION lib/cmake/zscript-model-components
)

# Create the package configuration file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/src/main/cmake/zscript-model-components-config.cmake.in
    ${CMAKE_BINARY_DIR}/zscript-model-components-config.cmake
    INSTALL_DESTINATION lib/cmake/zscript-model-components
)

install(
    FILES ${CMAKE_BINARY_DIR}/zscript-model-components-config.cmake
    DESTINATION lib/cmake/zscript-model-components
)

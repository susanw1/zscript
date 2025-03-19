# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

add_library(ZscriptGenCoreImpl INTERFACE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/core-version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/zscript/sys/core-version.h @ONLY
)

target_include_directories(ZscriptGenCoreImpl INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>  # Add the binary dir to the INTERFACE include path to pick up core-version
        $<INSTALL_INTERFACE:include>
)

target_sources(ZscriptGenCoreImpl INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES zscript
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/zscript/sys/core-version.h
        DESTINATION include/zscript/sys
)

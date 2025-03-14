# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

add_executable(ZsTokenBufferTests tokenbuffer_tests.c)

target_compile_options(ZsTokenBufferTests
    PRIVATE     ${test_compiler_options}
)
target_link_libraries(ZsTokenBufferTests
    PRIVATE
        zs_core_compiler_flags
        ZscriptSub
        zscript::model-components::ZscriptModelComponents
)

add_test(NAME StandardUse COMMAND ZsTokenBufferTests)

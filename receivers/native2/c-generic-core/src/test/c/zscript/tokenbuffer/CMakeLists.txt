# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

set(link_libs
    zs_core_compiler_flags
    ZscriptGenCoreImpl
    zscript::model-components::ZscriptModelComponents
)

macro(invoke_test test_name)
    add_executable(${test_name} ${test_name}_tests.c)
    target_compile_options(${test_name}
        PRIVATE     ${test_compiler_options}
    )
    target_link_libraries(${test_name} PRIVATE ${link_libs})
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

invoke_test(Basics)
invoke_test(TokenRingBuffer)
invoke_test(TokenBufferFlags)

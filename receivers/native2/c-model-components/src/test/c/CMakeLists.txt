# Zscript Library - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
#
# SPDX-License-Identifier: MIT

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    set(test_compiler_options
        -std=c11
        -Werror
        -Wall -Wextra -Wpedantic
        -Wno-unused-variable
        -Wno-unused-function
)
elseif (MSVC)
    set(test_compiler_options
        /std:c11                 # MSVC C11 standard
        /W4                      # Equivalent of -Wall -Wextra -Wpedantic
        /WX                      # Equivalent of -Werror
#       /wd4100                  # Ignore unused parameter warnings (equivalent of -Wno-unused-parameter)
        /wd4505                  # Ignore unused function warnings (equivalent of -Wno-unused-function)
        /wd4189                  # Ignore unused variable warnings (equivalent of -Wno-unused-variable)
)
endif()

add_executable(ZsTests generated_code_test.c)

target_compile_options(ZsTests
    PRIVATE     ${test_compiler_options}
)

target_link_libraries(ZsTests
    PRIVATE     zs_core_compiler_flags
                ZscriptModelComponents
)

add_test(NAME GeneratedCodeTest COMMAND ZsTests)

# Zscript - Command System for Microcontrollers
# Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
# SPDX-License-Identifier: MIT

set(link_libs
        zs_core_compiler_flags
        ZscriptGenCoreImpl
        zscript::model-components::ZscriptModelComponents
)

# Fix the libs above, so we don't have to supply them in every test
macro(invoke_test test_name)
    invoke_test_with_libs("${test_name}" "${link_libs}")
endmacro()

invoke_test(Tokenizer)

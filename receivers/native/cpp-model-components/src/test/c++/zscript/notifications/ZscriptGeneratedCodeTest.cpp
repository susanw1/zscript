/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <cstdint>
#include <string>

#include <net/zscript/model/components/Zchars.hpp>
#include <net/zscript/model/components/ZscriptStatus.hpp>

#include <net/zscript/model/modules/base/CoreModule.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>

namespace Zscript {

template<typename S, typename T>
void assertEquals(std::string msg, S expected, T actual) {
    if (expected != actual) {
        std::string s = msg + ": Expecting " + std::to_string(expected) + " ('"+ (char) expected +"'), but actually got " + std::to_string(actual) + " ('"+ (char) actual +"')";
        throw s;
    }
}

void shouldFindZcharConstants() {
    assertEquals("Zchars::Z_CMD", 'Z', Zchars::Z_CMD);
    assertEquals("Z_OPENPAREN", '(', Zchars::Z_OPENPAREN);
    assertEquals("Z_NULL_CHAR", '\0', Zchars::Z_NULL_CHAR);
}

class P {};

void shouldValidateZcharConstants() {
    assertEquals("alwaysIgnore(Zchars::Z_NULL_CHAR)", true, ZcharsUtils<P>::alwaysIgnore(Zchars::Z_NULL_CHAR));
    assertEquals("alwaysIgnore('a')", false, ZcharsUtils<P>::alwaysIgnore('a'));
    assertEquals("alwaysIgnore(' ')", false, ZcharsUtils<P>::alwaysIgnore(' '));

    assertEquals("shouldIgnore(' ')", true, ZcharsUtils<P>::shouldIgnore(' '));
    assertEquals("shouldIgnore('\\t')", true, ZcharsUtils<P>::shouldIgnore('\t'));
    assertEquals("shouldIgnore('a')", false, ZcharsUtils<P>::shouldIgnore('a'));

    assertEquals("isSeparator('&')", true, ZcharsUtils<P>::isSeparator('&'));
    assertEquals("isSeparator('a')", false, ZcharsUtils<P>::isSeparator('a'));
    assertEquals("isSeparator('\\n')", true, ZcharsUtils<P>::isSeparator('\n'));
}

void shouldValidateZscriptStatus() {
    assertEquals("ResponseStatus::SUCCESS", 0, ResponseStatus::SUCCESS);
    assertEquals("ResponseStatus::COMMAND_FAIL", 1, ResponseStatus::COMMAND_FAIL);
    assertEquals("ResponseStatus::BUFFER_OVR_ERROR", 0x10, ResponseStatus::BUFFER_OVR_ERROR);

    typedef ResponseStatusUtils<P> RS;

    assertEquals("isSuccess(ResponseStatus::SUCCESS)", true, RS::isSuccess(ResponseStatus::SUCCESS));
    assertEquals("isSuccess(ResponseStatus::COMMAND_FAIL)", false, RS::isSuccess(ResponseStatus::COMMAND_FAIL));
    assertEquals("isSuccess(ResponseStatus::BUFFER_OVR_ERROR)", false, RS::isSuccess(ResponseStatus::BUFFER_OVR_ERROR));

    assertEquals("isFailure(ResponseStatus::SUCCESS)", false, RS::isFailure(ResponseStatus::SUCCESS));
    assertEquals("isFailure(ResponseStatus::COMMAND_FAIL)", true, RS::isFailure(ResponseStatus::COMMAND_FAIL));
    assertEquals("isFailure(ResponseStatus::BUFFER_OVR_ERROR)", false, RS::isFailure(ResponseStatus::BUFFER_OVR_ERROR));

    assertEquals("isError(ResponseStatus::SUCCESS)", false, RS::isError(ResponseStatus::SUCCESS));
    assertEquals("isError(ResponseStatus::COMMAND_FAIL)", false, RS::isError(ResponseStatus::COMMAND_FAIL));
    assertEquals("isError(ResponseStatus::BUFFER_OVR_ERROR)", true, RS::isError(ResponseStatus::BUFFER_OVR_ERROR));
}

}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    try {
        Zscript::shouldFindZcharConstants();
        Zscript::shouldValidateZcharConstants();
        Zscript::shouldValidateZscriptStatus();
    } catch (std::string s) {
        std::cerr << s << std::endl;
        return 1;
    }
    return 0;
}

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_COMMANDS_ZCODEIDENTIFYCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_ZCODEIDENTIFYCOMMAND_HPP_


#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"

class ZcodeIdentifyCommand: public ZcodeCommand {
private:
    const uint8_t code = 0x00;
public:

    void execute(ZcodeCommandSlot *slot, ZcodeCommandSequence *sequence,
            ZcodeOutStream *out);

    void setLocks(ZcodeCommandSlot *slot, ZcodeLockSet *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeIdentifyCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};
#include "ZcodeOutStream.hpp"
#include "parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_ZCODEIDENTIFYCOMMAND_HPP_ */

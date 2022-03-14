/*
 * ZcodeStoreMacAddressCommand.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_ZCODESTOREMACADDRESSCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_ZCODESTOREMACADDRESSCOMMAND_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeFlashPersistence.hpp"

class ZcodeStoreMacAddressCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x12;
    ZcodeFlashPersistence *persist;

public:
    ZcodeStoreMacAddressCommand(ZcodeFlashPersistence *persist) :
            persist(persist) {
    }

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence,
            ZcodeOutStream<ZcodeParameters> *out);

    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeStoreMacAddressCommand::code;
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

#endif /* SRC_TEST_CPP_COMMANDS_ZCODESTOREMACADDRESSCOMMAND_HPP_ */
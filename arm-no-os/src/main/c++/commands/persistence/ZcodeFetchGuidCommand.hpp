/*
 * ZcodeFetchGuidCommand.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_ZCODEFETCHGIUDCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_ZCODEFETCHGIUDCOMMAND_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeFlashPersistence.hpp"

class ZcodeFetchGuidCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x06;
    ZcodeFlashPersistence *persist;
    public:
    ZcodeFetchGuidCommand(ZcodeFlashPersistence *persist) :
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
        return length == 1 && code[0] == ZcodeFetchGuidCommand::code;
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

#endif /* SRC_TEST_CPP_COMMANDS_ZCODEFETCHGIUDCOMMAND_HPP_ */

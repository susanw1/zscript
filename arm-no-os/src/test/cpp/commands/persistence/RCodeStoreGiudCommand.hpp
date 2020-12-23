/*
 * RCodeStoreGiudCommand.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_RCODESTOREGIUDCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_RCODESTOREGIUDCOMMAND_HPP_
#include "../persistence/RCodeMbedFlashPersistence.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"

class RCodeStoreGiudCommand: public RCodeCommand {
private:
    const uint8_t code = 0x13;
    RCodeMbedFlashPersistence *persist;
public:
    RCodeStoreGiudCommand(RCodeMbedFlashPersistence *persist) :
            persist(persist) {
    }

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeStoreGiudCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};
#include "RCodeOutStream.hpp"
#include "parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_RCODESTOREGIUDCOMMAND_HPP_ */

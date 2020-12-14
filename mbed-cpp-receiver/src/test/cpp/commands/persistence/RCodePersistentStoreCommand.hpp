/*
 * RCodePersistentStoreCommand.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTSTORECOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTSTORECOMMAND_HPP_

#include "../persistence/RCodeMbedFlashPersistence.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"

class RCodePersistentStoreCommand: public RCodeCommand {
private:
    const uint8_t code = 0x10;
    RCodeMbedFlashPersistence *persist;
public:
    RCodePersistentStoreCommand(RCodeMbedFlashPersistence *persist) :
            persist(persist) {
    }
    void finish(RCodeCommandSlot *rCodeCommandSlot, RCodeOutStream *out) const {
    }

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    void setLocks(RCodeLockSet *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodePersistentStoreCommand::code;
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

#endif /* SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTSTORECOMMAND_HPP_ */

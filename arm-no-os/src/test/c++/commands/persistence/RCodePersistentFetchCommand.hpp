/*
 * RCodePersistentFetchCommand.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTFETCHCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTFETCHCOMMAND_HPP_

#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"
#include "RCodeFlashPersistence.hpp"

class RCodePersistentFetchCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code = 0x11;
    RCodeFlashPersistence *persist;
    public:
    RCodePersistentFetchCommand(RCodeFlashPersistence *persist) :
            persist(persist) {
    }
    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
            RCodeOutStream<RCodeParameters> *out);

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodePersistentFetchCommand::code;
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

#endif /* SRC_TEST_CPP_COMMANDS_RCODEPERSISTENTFETCHCOMMAND_HPP_ */
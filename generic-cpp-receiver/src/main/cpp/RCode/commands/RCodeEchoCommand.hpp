/*
 * RCodeEchoCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeCommand.hpp"

class RCodeEchoCommand: public RCodeCommand {
private:
    const uint8_t code = 0x01;
public:

    virtual void finish(RCodeCommandSlot *rCodeCommandSlot,
            RCodeOutStream *out) const {
    }

    virtual void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    virtual void setLocks(RCodeLockSet *locks) const {
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeEchoCommand::code;
    }

    virtual uint8_t getCodeLength() const {
        return 1;
    }

    virtual uint8_t const* getFullCode() const {
        return &code;
    }
};
#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_ */

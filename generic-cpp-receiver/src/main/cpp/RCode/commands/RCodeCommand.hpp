/*
 * RCodeCommand.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

class RCodeCommandChannel;
class RCodeCommandSlot;
class RCodeOutStream;
class RCodeCommandSequence;
class RCodeLockSet;

class RCodeCommand {
public:

    virtual void finish(RCodeCommandSlot *rCodeCommandSlot,
            RCodeOutStream *out) const {
    }

    virtual void moveAlong(RCodeCommandSlot *rCodeCommandSlot) const {
    }

    virtual void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out) = 0;

    virtual void setLocks(RCodeCommandSlot *slot,
            RCodeLockSet *locks) const = 0;

    virtual uint8_t getCode() const = 0;

    virtual bool matchesCode(uint8_t code[], uint8_t length) const = 0;

    virtual uint8_t getCodeLength() const = 0;

    virtual uint8_t const* getFullCode() const = 0;

    virtual ~RCodeCommand() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODECOMMAND_HPP_ */

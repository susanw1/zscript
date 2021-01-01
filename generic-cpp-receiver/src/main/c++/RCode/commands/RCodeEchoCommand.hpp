/*
 * RCodeEchoCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"

template <class RP>
class RCodeEchoCommand: public RCodeCommand<RP> {
private:
    const uint8_t code = 0x01;
public:

    virtual void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    virtual void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
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

template<class RP>
void RCodeEchoCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    if (!slot->getFields()->has('S')) {
        out->writeStatus(OK);
    } else {
        slot->fail("", (RCodeResponseStatus) slot->getFields()->get('S', 0));
        out->writeStatus((RCodeResponseStatus) slot->getFields()->get('S', 0));
    }
    slot->getFields()->copyTo(out);
    slot->getBigField()->copyTo(out);
    slot->setComplete(true);
}


#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEECHOCOMMAND_HPP_ */

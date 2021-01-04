/*
 * RCodeNotificationHostCommand.hpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeDebugOutput;

template<class RP>
class RCode;

template<class RP>
class RCodeNotificationHostCommand: public RCodeCommand<RP> {
private:
    const uint8_t code = 0x08;
    RCode<RP> *const rcode;
public:
    RCodeNotificationHostCommand(RCode<RP> *const rcode) :
            rcode(rcode) {
    }

    void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeNotificationHostCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class RP>
void RCodeNotificationHostCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    rcode->getNotificationManager()->setNotificationChannel(sequence->getChannel());
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../RCode.hpp"
#include "../RCodeOutStream.hpp"
#include "../RCodeNotificationManager.hpp"
#include "../parsing/RCodeCommandSlot.hpp"
#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_ */

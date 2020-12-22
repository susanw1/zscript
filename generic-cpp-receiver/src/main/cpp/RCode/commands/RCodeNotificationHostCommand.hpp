/*
 * RCodeNotificationHostCommand.hpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#ifdef NOTIFICATIONS
#include "RCodeCommand.hpp"

class RCodeOutStream;
class RCodeDebugOutput;
class RCode;

class RCodeNotificationHostCommand: public RCodeCommand {
private:
    const uint8_t code = 0x08;
    RCode *const rcode;
public:
    RCodeNotificationHostCommand(RCode *const rcode) :
            rcode(rcode) {
    }

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
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

#include "../RCode.hpp"
#include "../RCodeOutStream.hpp"
#include "../RCodeNotificationManager.hpp"
#include "../parsing/RCodeCommandSlot.hpp"
#endif
#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODENOTIFICATIONHOSTCOMMAND_HPP_ */

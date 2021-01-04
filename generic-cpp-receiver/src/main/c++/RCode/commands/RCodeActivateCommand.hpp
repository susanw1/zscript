/*
 * RCodeActivateCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeActivateCommand: public RCodeCommand<RP> {
private:
    static bool activated;
    const uint8_t code = 0x03;
public:
    static const int MAX_SYSTEM_CODE = 15;

    static bool isActivated() {
        return activated;
    }

    static void reset() {
        activated = false;
    }

    void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
        if (!activated) {
            out->writeField('A', 0);
            out->writeStatus(OK);
        } else {
            out->writeField('A', 1);
            out->writeStatus(OK);
        }
        activated = true;
        slot->setComplete(true);
    }

    void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeActivateCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class RP>
bool RCodeActivateCommand<RP>::activated = false;


#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_ */

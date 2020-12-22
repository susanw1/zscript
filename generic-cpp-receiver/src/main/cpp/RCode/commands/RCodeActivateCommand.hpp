/*
 * RCodeActivateCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeCommand.hpp"

class RCodeOutStream;
class RCodeActivateCommand: public RCodeCommand {
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

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence, RCodeOutStream *out);

    void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
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

#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEACTIVATECOMMAND_HPP_ */

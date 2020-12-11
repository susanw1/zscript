/*
 * RCodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeCommand.hpp"

class RCodeOutStream;
class RCode;
class RCodeCapabilitiesCommand: public RCodeCommand {
private:
    RCode *rcode;
    const uint8_t code = 0x02;
public:
    RCodeCapabilitiesCommand(RCode *rcode) :
            rcode(rcode) {

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
        return length == 1 && code[0] == RCodeCapabilitiesCommand::code;
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
#include "../parsing/RCodeCommandFinder.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_ */

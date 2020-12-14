/*
 * RCodeI2cSendCommand.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSENDCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSENDCOMMAND_HPP_

#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"
#include "RCodeI2cSubsystem.hpp"

class RCodeI2cSendCommand: public RCodeCommand {
private:
    RCodeI2cSubsystem *i2cSystem;
    const uint8_t code = 0x51;

public:

    void finish(RCodeCommandSlot *rCodeCommandSlot, RCodeOutStream *out) const;

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    void setLocks(RCodeLockSet *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeI2cSendCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t
    const* getFullCode() const {
        return &code;
    }
}
;

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSENDCOMMAND_HPP_ */

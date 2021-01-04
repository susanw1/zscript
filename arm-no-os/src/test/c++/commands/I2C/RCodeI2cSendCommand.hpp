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
#include "RCodeI2cBus.hpp"

class RCodeI2cSendCommand: public RCodeCommand {
private:
    const uint8_t code = 0x51;

    static void setAsFinished(I2cTerminationStatus status, RCodeCommandSlot *slot, uint8_t retries);
    public:
    void finish(RCodeCommandSlot *rCodeCommandSlot, RCodeOutStream *out) const;

    void moveAlong(RCodeCommandSlot *rCodeCommandSlot) const;

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence, RCodeOutStream *out);

    void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const;

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
};
#include "parsing/RCodeCommandSlot.hpp"
#include "RCodeOutStream.hpp"
#include "RCodeLockSet.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSENDCOMMAND_HPP_ */

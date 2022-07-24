/*
 * ZcodeI2cSendCommand.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CSENDCOMMAND_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CSENDCOMMAND_HPP_

#include "../../old-commands/old-I2C/ZcodeI2cBus.hpp"
#include "../old-I2C/ZcodeI2cSubsystem.hpp"
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"

class ZcodeI2cSendCommand: public ZcodeCommand<ZcodeParameters> {
    private:
        const uint8_t code = 0x51;

        static void setAsFinished(I2cTerminationStatus status, ZcodeCommandSlot<ZcodeParameters> *slot, uint8_t retries);

    public:
        void finish(ZcodeCommandSlot<ZcodeParameters> *zcodeCommandSlot, ZcodeOutStream<ZcodeParameters> *out) const;

        void moveAlong(ZcodeCommandSlot<ZcodeParameters> *zcodeCommandSlot) const;

        void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out);

        void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const;

        uint8_t getCode() const {
            return code;
        }

        bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeI2cSendCommand::code;
        }

        uint8_t getCodeLength() const {
            return 1;
        }

        uint8_t
        const* getFullCode() const {
            return &code;
        }
};
#include "parsing/ZcodeCommandSlot.hpp"
#include "ZcodeOutStream.hpp"
#include "ZcodeLockSet.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CSENDCOMMAND_HPP_ */

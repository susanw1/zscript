/*
 * RCodeUartReadCommand.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_RCODEUARTAVAILABLECOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_UART_RCODEUARTAVAILABLECOMMAND_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "commands/RCodeCommand.hpp"
#include "RCodeUartSubsystem.hpp"

class RCodeUartAvailableCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code = 0x73;

public:
    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
        slot->setComplete(true);
        uint8_t busCount = slot->getFields()->getByteCount('B');
        uint8_t bus = 0;
        if (busCount > 1) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Bus number must be within range");
            return;
        } else if (busCount == 1) {
            bus = slot->getFields()->get('B', 0);
            if (bus > GeneralHalSetup::uartCount) {
                slot->fail("", BAD_PARAM);
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("Bus number must be within range");
                return;
            }
        }
        out->writeStatus(OK);
        out->writeField('L', (uint16_t) UartManager::getUartById(bus)->available());
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeUartAvailableCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

#endif /* SRC_TEST_C___COMMANDS_UART_RCODEUARTAVAILABLECOMMAND_HPP_ */

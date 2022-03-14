/*
 * ZcodeUartReadCommand.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTREADCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTREADCOMMAND_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeUartSubsystem.hpp"

class ZcodeUartReadCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x72;

public:
    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
    }

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
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
        uint32_t length = 0;
        if (slot->getFields()->countFieldSections('L') != 0) {
            uint8_t count = slot->getFields()->getByteCount('L');
            for (int i = 0; i < count; i++) {
                length <<= 8;
                length |= slot->getFields()->getByte('L', i, 0);
            }
        } else {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Read length must be specified");
            return;
        }
        SerialError error = UartManager::getUartById(bus)->getError(length);
        bool failOnDataOut = slot->getFields()->countFieldSections('F') != 0;
        bool useBigString = slot->getFields()->countFieldSections('T') != 0;
        uint8_t data[length];
        uint16_t lengthRead = UartManager::getUartById(bus)->read(data, length);
        if ((lengthRead != length && failOnDataOut) || error != SerialNoError) {
            out->writeStatus(CMD_FAIL);
        } else {
            out->writeStatus(OK);
        }
        if (error != SerialNoError) {
            out->writeField('F', (uint8_t) error);
        }
        if (useBigString) {
            out->writeBigStringField(data, lengthRead);
        } else {
            out->writeBigHexField(data, lengthRead);
        }
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeUartReadCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTREADCOMMAND_HPP_ */

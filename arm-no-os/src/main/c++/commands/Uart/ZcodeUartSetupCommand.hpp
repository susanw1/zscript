/*
 * ZcodeUartSetupCommand.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTSETUPCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTSETUPCOMMAND_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeUartSubsystem.hpp"

class ZcodeUartSetupCommand: public ZcodeCommand<ZcodeParameters> {
private:
    const uint8_t code = 0x70;

public:
    void setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
    }

    void execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
        slot->setComplete(true);
        uint8_t busCount = slot->getFields()->countFieldSections('U');
        uint8_t bus = 0;
        if (busCount > 1) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Bus number must be within range");
            return;
        } else if (busCount == 1) {
            bus = slot->getFields()->get('U', 0);
            if (bus > GeneralHalSetup::uartCount) {
                slot->fail("", BAD_PARAM);
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("Bus number must be within range");
                return;
            }
        }
        uint32_t baud = 9600;
        if (slot->getFields()->getByteCount('B') != 0) {
            baud = 0;
            uint8_t count = slot->getFields()->getByteCount('B');
            for (int i = 0; i < count; i++) {
                baud <<= 8;
                baud |= slot->getFields()->getByte('B', i, 0);
            }
            if (baud == 0) {
                slot->fail("", BAD_PARAM);
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("Baud rate cannot be 0");
                return;
            }
        }
        if (slot->getFields()->getByteCount('C') > 1) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Binary capability byte can only be 1 byte");
            return;
        }
        bool singleNdoubleStop = (slot->getFields()->getByte('C', 0, 0) & 0x04) != 0;
        if (baud > ClockManager::getClock(SysClock)->getFreqKhz() * 1000 / 16) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Baud rate too large");
            return;
        }
        UartManager::getUartById(bus)->init(&ZcodeUartSubsystem::uartBufferOverflowCallback, baud, singleNdoubleStop);
        out->writeStatus(OK);
        out->writeField('N', (uint8_t) 0);
        out->writeField('C', (uint8_t) 0x04);
        out->writeField('B', (uint32_t) ClockManager::getClock(SysClock)->getFreqKhz() * 1000 / 16);
        out->writeField('U', (uint8_t) GeneralHalSetup::uartCount);
        out->writeField('T', (uint16_t) GeneralHalSetup::UartBufferTxSize);
        out->writeField('M', (uint16_t) GeneralHalSetup::UartBufferRxSize);
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeUartSetupCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTSETUPCOMMAND_HPP_ */

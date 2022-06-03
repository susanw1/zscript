/*
 * ZcodeUartReadCommand.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTSKIPCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTSKIPCOMMAND_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeUartSubsystem.hpp"

class ZcodeUartSkipCommand: public ZcodeCommand<ZcodeParameters> {
    private:
        const uint8_t code = 0x74;

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
                out->writeBigStringField("Skip length must be specified");
                return;
            }
            SerialError error = UartManager::getUartById(bus)->getError(length);
            uint16_t lengthSkipped = UartManager::getUartById(bus)->skip(length);
            out->writeStatus(OK);
            if (error != SerialNoError) {
                out->writeField('F', (uint8_t) error);
            }
            out->writeField('L', (uint16_t) lengthSkipped);
        }

        uint8_t getCode() const {
            return code;
        }

        bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeUartSkipCommand::code;
        }

        uint8_t getCodeLength() const {
            return 1;
        }

        uint8_t const* getFullCode() const {
            return &code;
        }
};

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTSKIPCOMMAND_HPP_ */
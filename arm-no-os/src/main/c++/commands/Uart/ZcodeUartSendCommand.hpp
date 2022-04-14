/*
 * ZcodeUartSendCommand.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTSENDCOMMAND_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTSENDCOMMAND_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "commands/ZcodeCommand.hpp"
#include "ZcodeUartSubsystem.hpp"

class ZcodeUartSendCommand: public ZcodeCommand<ZcodeParameters> {
    private:
        const uint8_t code = 0x71;

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
            bool worked = UartManager::getUartById(bus)->write(slot->getBigField()->getData(), slot->getBigField()->getLength());
            if (worked) {

                UartManager::getUartById(bus)->transmitWriteBuffer();
                out->writeStatus(OK);
            } else {
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("UART busy or message too long");
            }
        }

        uint8_t getCode() const {
            return code;
        }

        bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeUartSendCommand::code;
        }

        uint8_t getCodeLength() const {
            return 1;
        }

        uint8_t const* getFullCode() const {
            return &code;
        }
};

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTSENDCOMMAND_HPP_ */

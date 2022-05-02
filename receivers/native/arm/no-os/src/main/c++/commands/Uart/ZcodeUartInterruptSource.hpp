/*
 * ZcodeUartInterruptSource.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTINTERRUPTSOURCE_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTINTERRUPTSOURCE_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include <Zcode.hpp>

class ZcodeUartInterruptSource: public ZcodeBusInterruptSource<ZcodeParameters> {
    private:
        static uint8_t uartInterrupts;

    public:
        static void setInterrupt(uint8_t id) {
            uartInterrupts |= 1 << id;
        }

        int16_t takeUncheckedNotificationId() {
            if (uartInterrupts == 0) {
                return -1;
            }
            for (uint8_t i = 0; i < 8; i++) {
                if (uartInterrupts & (1 << i)) {
                    uartInterrupts &= ~(1 << i);
                    return i;
                }
            }
            return -1;
        }

        uint8_t getNotificationType(uint8_t id) {
            return 2;
        }

        uint8_t getNotificationBus(uint8_t id) {
            return id;
        }

        void clearNotification(uint8_t id) {
            uartInterrupts &= ~(1 << id);
        }

        bool findAddress(uint8_t id) {
            return false;
        }

        int16_t getFoundAddress(uint8_t id) {
            return -1;
        }

        bool hasFindableAddress(uint8_t id) {
            return false;
        }

        bool hasStartedAddressFind(uint8_t id) {
            return false;
        }

};

#include "ZcodeUartSubsystem.hpp"

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTINTERRUPTSOURCE_HPP_ */

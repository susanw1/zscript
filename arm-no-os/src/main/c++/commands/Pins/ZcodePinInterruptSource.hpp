/*
 * ZcodePinInterruptSource.hpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_PINS_ZCODEPININTERRUPTSOURCE_HPP_
#define SRC_TEST_C___COMMANDS_PINS_ZCODEPININTERRUPTSOURCE_HPP_

#include <ZcodeIncludes.hpp>
#include "ZcodeParameters.hpp"
#include <Zcode.hpp>
#include "ZcodePinSystem.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void EXTI0_IRQHandler();
void EXTI1_IRQHandler();

void EXTI2_IRQHandler();
void EXTI3_IRQHandler();

void EXTI4_IRQHandler();
void EXTI9_5_IRQHandler();

#ifdef __cplusplus
}
#endif

class ZcodePinInterruptSource: public ZcodeBusInterruptSource<ZcodeParameters> {
    private:
        static uint16_t interruptConditions;

    public:
        static void init() {
            NVIC_SetPriority(EXTI0_IRQn, 10);
            NVIC_EnableIRQ(EXTI0_IRQn);

            NVIC_SetPriority(EXTI1_IRQn, 10);
            NVIC_EnableIRQ(EXTI1_IRQn);

            NVIC_SetPriority(EXTI2_IRQn, 10);
            NVIC_EnableIRQ(EXTI2_IRQn);

            NVIC_SetPriority(EXTI3_IRQn, 10);
            NVIC_EnableIRQ(EXTI3_IRQn);

            NVIC_SetPriority(EXTI4_IRQn, 10);
            NVIC_EnableIRQ(EXTI4_IRQn);

            NVIC_SetPriority(EXTI9_5_IRQn, 10);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
        }

        static void interrupt() {
            uint16_t interrupts = EXTI->PR1;
            interruptConditions |= interrupts;
            EXTI->PR1 |= interrupts;
        }

        static void clearInterrupts() {
            EXTI->IMR1 &= ~0xFFFF;
            EXTI->RTSR1 &= ~0xFFFF;
            EXTI->FTSR1 &= ~0xFFFF;
            SYSCFG->EXTICR[0] = 0;
            SYSCFG->EXTICR[1] = 0;
            SYSCFG->EXTICR[2] = 0;
            SYSCFG->EXTICR[3] = 0;
            EXTI->PR1 |= 0xFFFF;
        }

        static bool setInterrupt(GpioPinName name, bool rising, bool falling) {
            if (EXTI->IMR1 & 1 << name.pin) {
                return false;
            }
            SYSCFG->EXTICR[name.pin / 4] |= name.port << ((name.pin % 4) * 4);
            if (rising) {
                EXTI->RTSR1 |= 1 << name.pin;
            }
            if (falling) {
                EXTI->FTSR1 |= 1 << name.pin;
            }
            EXTI->IMR1 |= 1 << name.pin;

            return true;
        }

        int16_t takeUncheckedNotificationId() {
            if (interruptConditions == 0) {
                return -1;
            }
            for (uint8_t i = 0; i < 16; i++) {
                if (interruptConditions & (1 << i)) {
                    interruptConditions &= ~(1 << i);
                    return i;
                }
            }
            return -1;
        }

        uint8_t getNotificationType(uint8_t id) {
            return 7;
        }

        uint8_t getNotificationBus(uint8_t id) {
            uint8_t port = SYSCFG->EXTICR[id / 4] & 0xF << (id % 4);
            GpioPinName pinName = { port, id };
            uint8_t num = ZcodePinSystem::getPinNumber(pinName);
            return num;
        }

        void clearNotification(uint8_t id) {
            interruptConditions &= ~(1 << id);
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

#endif /* SRC_TEST_C___COMMANDS_PINS_ZCODEPININTERRUPTSOURCE_HPP_ */

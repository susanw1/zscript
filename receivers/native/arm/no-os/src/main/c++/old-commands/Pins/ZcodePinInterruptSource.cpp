/*
 * ZcodePinInterruptSource.cpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#include "../../old-commands/Pins/ZcodePinInterruptSource.hpp"

uint16_t ZcodePinInterruptSource::interruptConditions = 0;

void EXTI0_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

void EXTI1_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

void EXTI2_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

void EXTI3_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

void EXTI4_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

void EXTI9_5_IRQHandler() {
    ZcodePinInterruptSource::interrupt();
}

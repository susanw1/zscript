/*
 * RCodePinInterruptSource.cpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#include "RCodePinInterruptSource.hpp"

uint16_t RCodePinInterruptSource::interruptConditions = 0;

void EXTI0_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}
void EXTI1_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}

void EXTI2_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}
void EXTI3_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}

void EXTI4_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}
void EXTI9_5_IRQHandler() {
    RCodePinInterruptSource::interrupt();
}

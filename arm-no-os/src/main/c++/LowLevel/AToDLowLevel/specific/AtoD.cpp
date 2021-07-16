/*
 * AtoD.cpp
 *
 *  Created on: 5 Jan 2021
 *      Author: robert
 */

#include "../AtoD.hpp"
#include "../../ClocksLowLevel/SystemMilliClock.hpp"

void AtoD::init() {
    if (!isInit) {
        isInit = true;
        adcRegs->CR = 0;
        adcRegs->CR |= 0x10000000;
        SystemMilliClock::blockDelayMillis(2);
        adcRegs->CR |= 0x00000001;
        while (!(adcRegs->ISR & 0x1))
            ;
        adcRegs->ISR = 1;
    }
}

uint16_t AtoD::performReading(uint8_t channelSource) {
    adcRegs->CFGR = 0x80000000;
    adcRegs->CFGR2 = 0x0;
    adcRegs->SMPR1 = 0x3FFFFFFF;
    adcRegs->SMPR2 = 0x07FFFFFF;
    adcRegs->SQR1 = channelSource << 6;
    adcRegs->ISR = 0x7FF;
    adcRegs->CR |= 0x4;
    while (!(adcRegs->ISR & 0x4))
        ;
    uint16_t result = adcRegs->DR;
    adcRegs->ISR |= 0x8;
    return result;
}

/*
 * AtoD.cpp
 *
 *  Created on: 5 Jan 2021
 *      Author: robert
 */

#include "../AtoD.hpp"
#include "../../ClocksLowLevel/SystemMilliClock.hpp"

void AtoD::init() {
    const uint32_t resetAtoD = 0;
    const uint32_t aToDRegulatorEnable = 0x10000000;
    const uint32_t aToDEnable = 0x00000001;
    const uint32_t aToDReady = 0x00000001;

    if (!isInit) {
        isInit = true;
        adcRegs->CR = resetAtoD;
        adcRegs->CR |= aToDRegulatorEnable;
        SystemMilliClock::blockDelayMillis(2);
        adcRegs->CR |= aToDEnable;
        while (!(adcRegs->ISR & aToDReady))
            ;
        adcRegs->ISR = aToDReady;
    }
}

uint16_t AtoD::performReading(uint8_t channelSource) {
    const uint32_t disableInjectedQueue = 0x80000000;

    const uint32_t SMPR1MaxSampleTime = 0x3FFFFFFF;
    const uint32_t SMPR2MaxSampleTime = 0x07FFFFFF;

    const uint32_t resetAtoDInterrupts = 0x7FF;
    const uint32_t aToDStart = 0x4;
    const uint32_t aToDComplete = 0x4;
    const uint32_t aToDClearSequenceComplete = 0x8;

    adcRegs->CFGR = disableInjectedQueue;
    adcRegs->CFGR2 = 0;
    adcRegs->SMPR1 = SMPR1MaxSampleTime;
    adcRegs->SMPR2 = SMPR2MaxSampleTime;
    adcRegs->SQR1 = channelSource << 6;
    adcRegs->ISR = resetAtoDInterrupts;
    adcRegs->CR |= aToDStart;
    while (!(adcRegs->ISR & aToDComplete))
        ;
    uint16_t result = adcRegs->DR;
    adcRegs->ISR |= aToDClearSequenceComplete;
    return result;
}

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_

#include <llIncludes.hpp>
#include <pins-ll/lowlevel/Gpio.hpp>
#include <pins-ll/lowlevel/GpioManager.hpp>

#include <serial-ll/lowlevel/Serial.hpp>
#include "../../../../../../../../clock/src/main/c++/clock-ll/ClockManager.hpp"

template<class LL>
class Uart;

template<class LL>
class UartInternal {
private:
    UartRegisters *registers;
    GpioPinName rx;
    PinAlternateFunction rxFunction;
    GpioPinName tx;
    PinAlternateFunction txFunction;
    SerialIdentifier id;

public:
    UartInternal(GpioPinName rx, PinAlternateFunction rxFunction, GpioPinName tx, PinAlternateFunction txFunction, UartRegisters *registers,
            SerialIdentifier id) :
            registers(registers), rx(rx), rxFunction(rxFunction), tx(tx), txFunction(txFunction), id(id) {
    }

    SerialIdentifier getId() {
        return id;
    }

    void init(uint32_t baud_rate, bool singleNdoubleStop) {
        const uint32_t uartEnable = 0x01;
        const uint32_t uartDoubleStopBit = 0x00002000;

        const uint32_t uart1RegisterClockEnable = 0x00004000;

        const uint32_t uart2RegisterClockEnable = 0x00020000;

        const uint32_t uartRxNEInterruptEnable = 0x20;
        const uint32_t uartIdleInterruptEnable = 0x10;
        const uint32_t uartTransmitEnable = 0x08;
        const uint32_t uartReceiveEnable = 0x04;

        const uint32_t uartTxDmaEnable = 0x80;

        registers->CR1 &= ~uartEnable; //disable the peripheral
        GpioManager<LL>::getPin(rx).init();
        GpioManager<LL>::getPin(tx).init();

        GpioManager<LL>::getPin(rx).setPullMode(NoPull);
        GpioManager<LL>::getPin(rx).setOutputMode(PushPull);
        GpioManager<LL>::getPin(rx).setAlternateFunction(rxFunction);
        GpioManager<LL>::getPin(rx).setOutputSpeed(VeryHighSpeed);
        GpioManager<LL>::getPin(rx).setMode(AlternateFunction);
        GpioManager<LL>::getPin(tx).setPullMode(NoPull);
        GpioManager<LL>::getPin(tx).setOutputMode(PushPull);
        GpioManager<LL>::getPin(tx).setAlternateFunction(txFunction);
        GpioManager<LL>::getPin(tx).setOutputSpeed(VeryHighSpeed);
        GpioManager<LL>::getPin(tx).setMode(AlternateFunction);
        //enable the peripheral clock, set source to SYSCLK
        if (id == 0) {
            RCC->APB2ENR |= uart1RegisterClockEnable;
        } else if (id == 1) {
            RCC->APB1ENR |= uart2RegisterClockEnable;
        }

        registers->CR1 = uartIdleInterruptEnable | uartTransmitEnable | uartReceiveEnable | uartRxNEInterruptEnable;
        registers->CR2 = singleNdoubleStop ? 0 : uartDoubleStopBit; // sets standard logic levels, and MSB first
        registers->CR3 = uartTxDmaEnable;
        uint32_t freq = ClockManager<LL>::getClock(PCLK)->getFreqKhz() * 1000;
        registers->BRR = freq / baud_rate;
        registers->CR1 |= uartEnable; //enable the peripheral
    }

    void clearFlags() {
        const uint32_t idle = 0x10;

        registers->ICR |= idle;
    }

    bool takeReceiverFullFlag() {
        const uint32_t rxNE = 0x20;

        if ((registers->ISR & rxNE) != 0) {
            return true;
        }
        return false;
    }

    bool takeIdleFlag() {
        const uint32_t idle = 0x10;

        if ((registers->ISR & idle) != 0) {
            registers->ICR |= idle;
            return true;
        }
        return false;
    }

    volatile uint8_t* getTransmitRegister() {
        return (volatile uint8_t*) &registers->TDR;
    }

    int16_t read();

    bool hasRxFifoData();
};

template<class LL>
int16_t UartInternal<LL>::read() {
    const uint32_t parityError = 0x01;
    const uint32_t framingError = 0x02;
    const uint32_t noiseError = 0x04;
    const uint32_t overflowError = 0x08;
    if ((registers->ISR & parityError) != 0) {
        registers->ICR |= parityError;
        return -SerialParityError;
    } else if ((registers->ISR & framingError) != 0) {
        registers->ICR |= framingError;
        registers->RDR;
        return -SerialFramingError;
    } else if ((registers->ISR & noiseError) != 0) {
        registers->ICR |= noiseError;
        return -SerialNoiseError;
    } else if ((registers->ISR & overflowError) != 0) {
        registers->ICR |= overflowError;
        return -SerialOverflowError;
    } else {
        return registers->RDR;
    }
}

template<class LL>
bool UartInternal<LL>::hasRxFifoData() {
    const uint32_t rxFifoNotEmpty = 0x20;
    return (registers->ISR & rxFifoNotEmpty) != 0;
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_ */

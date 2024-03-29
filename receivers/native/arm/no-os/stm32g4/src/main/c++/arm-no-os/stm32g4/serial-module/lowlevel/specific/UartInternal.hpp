/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>

#include <arm-no-os/serial-module/lowlevel/Serial.hpp>

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
    UartInternal() :
            registers(NULL), rx(PA_1), rxFunction(0), tx(PA_1), txFunction(0), id(0) {
    }

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
        const uint32_t uart1ClockSelectMask = 0x00000003;
        const uint32_t uart1ClockSelectSysclk = 0x00000001;

        const uint32_t uart2RegisterClockEnable = 0x00020000;
        const uint32_t uart2ClockSelectMask = 0x0000000C;
        const uint32_t uart2ClockSelectSysclk = 0x00000004;

        const uint32_t uart3RegisterClockEnable = 0x00040000;
        const uint32_t uart3ClockSelectMask = 0x00000030;
        const uint32_t uart3ClockSelectSysclk = 0x00000010;

        const uint32_t uart4RegisterClockEnable = 0x00080000;
        const uint32_t uart4ClockSelectMask = 0x000000C0;
        const uint32_t uart4ClockSelectSysclk = 0x00000040;

        const uint32_t uart5RegisterClockEnable = 0x00100000;
        const uint32_t uart5ClockSelectMask = 0x00000300;
        const uint32_t uart5ClockSelectSysclk = 0x00000100;

        const uint32_t uart6RegisterClockEnable = 0x00000001;
        const uint32_t uart6ClockSelectMask = 0x00000C00;
        const uint32_t uart6ClockSelectSysclk = 0x00000400;

        const uint32_t uartRxFifoFullInterruptEnable = 0x80000000;
        const uint32_t uartFifoEnable = 0x20000000;
        const uint32_t uartIdleInterruptEnable = 0x10;
        const uint32_t uartTransmitEnable = 0x08;
        const uint32_t uartReceiveEnable = 0x04;

        const uint32_t uartRxFifoThresholdInterruptEnable = 0x10000000;
        const uint32_t uartRxFifoThreshold1_4 = 0x06000000;
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
            RCC->CCIPR &= ~uart1ClockSelectMask;
            RCC->CCIPR |= uart1ClockSelectSysclk;
        } else if (id == 1) {
            RCC->APB1ENR1 |= uart2RegisterClockEnable;
            RCC->CCIPR &= ~uart2ClockSelectMask;
            RCC->CCIPR |= uart2ClockSelectSysclk;
        } else if (id == 2) {
            RCC->APB1ENR1 |= uart3RegisterClockEnable;
            RCC->CCIPR &= ~uart3ClockSelectMask;
            RCC->CCIPR |= uart3ClockSelectSysclk;
        } else if (id == 3) {
            RCC->APB1ENR1 |= uart4RegisterClockEnable;
            RCC->CCIPR &= ~uart4ClockSelectMask;
            RCC->CCIPR |= uart4ClockSelectSysclk;
        } else if (id == 4) {
            RCC->APB1ENR1 |= uart5RegisterClockEnable;
            RCC->CCIPR &= ~uart5ClockSelectMask;
            RCC->CCIPR |= uart5ClockSelectSysclk;
        } else if (id == 5) {
            RCC->APB1ENR2 |= uart6RegisterClockEnable;
            RCC->CCIPR &= ~uart6ClockSelectMask;
            RCC->CCIPR |= uart6ClockSelectSysclk;
        }

        registers->CR1 = uartRxFifoFullInterruptEnable | uartFifoEnable | uartIdleInterruptEnable | uartTransmitEnable | uartReceiveEnable;
        registers->CR2 = singleNdoubleStop ? 0 : uartDoubleStopBit; // sets standard logic levels, and MSB first
        registers->CR3 = uartRxFifoThresholdInterruptEnable | uartRxFifoThreshold1_4 | uartTxDmaEnable;
        registers->PRESC = 0;
        uint32_t freq = ClockManager<LL>::getClock(SysClock)->getFreqKhz() * 1000;
        uint32_t freqPresc = freq;
        if (baud_rate < 100000) { //avoid overflows
            while (baud_rate * 32768 <= freqPresc) { // if we have a really low baud rate, engage the prescaler to cope
                freqPresc /= 2;
                registers->PRESC++;
            }
        }
        registers->BRR = freqPresc / baud_rate;
        registers->CR1 |= uartEnable; //enable the peripheral
    }

    void clearFlags() {
        const uint32_t idle = 0x10;

        registers->ICR |= idle;
    }

    bool takeReceiverFullFlag() {
        const uint32_t rxFifoFull = 0x01000000;
        const uint32_t rxFifoTrigger = 0x04000000;

        if ((registers->ISR & (rxFifoFull | rxFifoTrigger)) != 0) {
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

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_ */

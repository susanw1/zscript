/*
 * UartInternal.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>
#include "UartRegisters.hpp"
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>

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

#include "../Uart.hpp"
#include "UartInternalcpp.hpp"

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_ */

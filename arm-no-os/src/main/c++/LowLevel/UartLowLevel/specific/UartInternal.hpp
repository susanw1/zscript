/*
 * UartInternal.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_
#include "../../GeneralLLSetup.hpp"
#include "../../ClocksLowLevel/ClockManager.hpp"
#include "UartRegisters.hpp"
#include "../../GpioLowLevel/Gpio.hpp"
#include "../../GpioLowLevel/GpioManager.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void USART1_IRQHandler();
void USART2_IRQHandler();
void USART3_IRQHandler();

void UART4_IRQHandler();
void UART5_IRQHandler();

void LPUART1_IRQHandler();
#ifdef __cplusplus
}
#endif

class Uart;

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
        registers->CR1 &= ~0x01; //disable the peripheral
        GpioManager::getPin(rx)->init();
        GpioManager::getPin(tx)->init();

        GpioManager::getPin(rx)->setPullMode(NoPull);
        GpioManager::getPin(rx)->setOutputMode(PushPull);
        GpioManager::getPin(rx)->setAlternateFunction(rxFunction);
        GpioManager::getPin(rx)->setOutputSpeed(VeryHighSpeed);
        GpioManager::getPin(rx)->setMode(AlternateFunction);
        GpioManager::getPin(tx)->setPullMode(NoPull);
        GpioManager::getPin(tx)->setOutputMode(PushPull);
        GpioManager::getPin(tx)->setAlternateFunction(txFunction);
        GpioManager::getPin(tx)->setOutputSpeed(VeryHighSpeed);
        GpioManager::getPin(tx)->setMode(AlternateFunction);
        //enable the peripheral clock, set source to SYSCLK
        if (id == 0) {
            RCC->APB2ENR |= 0x00004000;
            RCC->CCIPR &= ~0x00000003;
            RCC->CCIPR |= 0x00000001;
        } else if (id == 1) {
            RCC->APB1ENR1 |= 0x00020000;
            RCC->CCIPR &= ~0x0000000C;
            RCC->CCIPR |= 0x00000004;
        } else if (id == 2) {
            RCC->APB1ENR1 |= 0x00040000;
            RCC->CCIPR &= ~0x00000030;
            RCC->CCIPR |= 0x00000010;
        } else if (id == 3) {
            RCC->APB1ENR1 |= 0x00080000;
            RCC->CCIPR &= ~0x000000C0;
            RCC->CCIPR |= 0x00000040;
        } else if (id == 4) {
            RCC->APB1ENR1 |= 0x00100000;
            RCC->CCIPR &= ~0x00000300;
            RCC->CCIPR |= 0x00000100;
        } else if (id == 5) {
            RCC->APB1ENR2 |= 0x00000001;
            RCC->CCIPR &= ~0x00000C00;
            RCC->CCIPR |= 0x00000400;
        }

        registers->CR1 = 0xA000001C; // enable RX FIFO full and IDLE interrupts. Enables transmitter, receiver, and FIFOs
        registers->CR2 = singleNdoubleStop ? 0x00000000 : 0x00002000; // sets standard logic levels, and MSB first
        registers->CR3 = 0x16000080; // sets the FIFO to interrupt on 3/4 full (6 bytes) and sets DMA for tx
        registers->PRESC = 0;
        uint32_t freq = ClockManager::getClock(SysClock)->getFreqKhz() * 1000;
        uint32_t freqPresc = freq;
        if (baud_rate < 100000) { //avoid overflows
            while (baud_rate * 32768 <= freqPresc) { // if we have a really low baud rate, engage the prescaler to cope
                freqPresc /= 2;
                registers->PRESC++;
            }
        }
        registers->BRR = freqPresc / baud_rate;
        registers->CR1 |= 0x01; //enable the peripheral
    }

    void clearFlags() {
        registers->ICR |= 0x10;
    }
    bool takeReceiverFullFlag() {
        if ((registers->ISR & 0x05000000) != 0) {
            return true;
        }
        return false;
    }

    bool takeIdleFlag() {
        if ((registers->ISR & 0x10) != 0) {
            registers->ICR |= 0x10;
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

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNAL_HPP_ */

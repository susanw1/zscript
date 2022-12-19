/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CINTERNAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CINTERNAL_HPP_

#include <i2c-ll/I2cLLInterfaceInclude.hpp>

#include <llIncludes.hpp>
#include <pins-ll/lowlevel/Gpio.hpp>
#include <pins-ll/lowlevel/GpioManager.hpp>
#include "../../../../../../../../clock/src/main/c++/clock-ll/SystemMilliClock.hpp"
#include "../../../../../../../../clock-ll/src/main/c++/clock-ll/ClockManager.hpp"
#include "I2cRegisters.hpp"

struct I2cState {
    uint16_t repeatCount :10;
    bool hasTx :1;
    bool hasRx :1;
    bool hasTxRx :1;
    bool txDone :1;
    bool useDmas :1;
    bool lockBool :1;
    bool tenBit :1;
};
template<class LL>
class I2cInternal {
private:
    GpioPinName sda;
    PinAlternateFunction sdaFunction;
    GpioPinName scl;
    PinAlternateFunction sclFunction;
    I2cRegisters *registers;

    void setupCr1(bool setupDmas, bool setupInterrupts) {
        const uint32_t enableI2cRxDma = 0x8000;
        const uint32_t enableI2cTxDma = 0x4000;
        const uint32_t enableI2cRxInterrupt = 0x04;
        const uint32_t enableI2cTxInterrupt = 0x02;
        const uint32_t enableI2cStopInterrupt = 0x20;
        const uint32_t enableI2cNackInterrupt = 0x10;
        const uint32_t enableI2cTransmitCompleteInterrupt = 0x40;
        const uint32_t enableI2cErrorInterrupt = 0x80;
        uint32_t cr1r = registers->CR1;
        cr1r &= ~(enableI2cRxDma | enableI2cTxDma |
                enableI2cRxInterrupt | enableI2cTxInterrupt |
                enableI2cErrorInterrupt | enableI2cTransmitCompleteInterrupt | enableI2cStopInterrupt | enableI2cNackInterrupt);
        if (setupInterrupts) {
            cr1r |= enableI2cErrorInterrupt | enableI2cTransmitCompleteInterrupt | enableI2cStopInterrupt
                    | enableI2cNackInterrupt;
            if (setupDmas) {
                cr1r |= enableI2cRxDma | enableI2cTxDma;
            } else {
                cr1r |= enableI2cRxInterrupt | enableI2cTxInterrupt;
            }
        }
        registers->CR1 = cr1r;
    }
public:
    I2cInternal(GpioPinName sda, PinAlternateFunction sdaFunction, GpioPinName scl, PinAlternateFunction sclFunction, I2cRegisters *registers) :
            sda(sda), sdaFunction(sdaFunction), scl(scl), sclFunction(sclFunction), registers(registers) {
    }
    void activateClock(I2cIdentifier id);
    void activatePins();
    bool recoverSdaJam();

    void setFrequency(Clock<LL> *clock, I2cFrequency freq);

    void enablePeripheral() {
        const uint32_t enableI2c = 0x1;
        if (registers != NULL) {
            setupErrorInterrupt();
            registers->CR1 |= enableI2c;
        }
    }

    void disablePeripheral() {
        const uint32_t enableI2c = 0x1;
        if (registers != NULL) {
            setupCr1(false, false);
            registers->CR1 &= ~enableI2c;
        }
    }

    void setStop() {
        const uint32_t i2cStopGenerate = 0x4000;
        registers->CR2 |= i2cStopGenerate;
    }
    void setNackAndStop() {
        const uint32_t i2cNackGenerate = 0x8000;
        const uint32_t i2cStopGenerate = 0x4000;
        registers->CR2 |= i2cNackGenerate | i2cStopGenerate;
    }

    void doBasicSetup() {
        const uint32_t enableI2c = 0x1;

        const uint32_t enableI2c10BitAddress = 0x00001000;

        const uint32_t enableI2cSclLowTimeout = 0x8000;
        const uint32_t sclLowTimeout3564Clock = 0x0600;

        uint32_t ccr1 = enableI2c;

        uint32_t ccr2 = enableI2c10BitAddress;

        if (registers != NULL) {
            registers->TIMEOUTR = enableI2cSclLowTimeout | sclLowTimeout3564Clock;  // disable timeout on clock stretch,
            // Enable SCL low timeout - with 50ms of timeout.

            registers->CR2 = ccr2;
            registers->CR1 = ccr1; // turn on peripheral
        }
    }

    bool hasReadDataInt() {
        const uint32_t rxNotEmpty = 0x04;
        return registers->ISR & rxNotEmpty;
    }
    uint8_t readData() {
        uint8_t data = registers->RXDR;
        return data;
    }
    bool isSetUp() {
        return registers != NULL;
    }

    bool hasNoSendDataInt() {
        const uint32_t txEmpty = 0x02;
        return registers->ISR & txEmpty;
    }
    void sendData(uint8_t data) {
        registers->TXDR = data;
    }

    bool hasBusErrorInt() {
        const uint32_t i2cBusError = 0x100;
        return registers->ISR & i2cBusError;
    }
    void clearBusErrorInt() {
        const uint32_t i2cBusError = 0x100;
        registers->ICR |= i2cBusError;
    }

    bool hasArbitrationLossInt() {
        const uint32_t i2cArbitrationLoss = 0x200;
        return registers->ISR & i2cArbitrationLoss;
    }
    void clearArbitrationLossInt() {
        const uint32_t i2cArbitrationLoss = 0x200;
        registers->ICR |= i2cArbitrationLoss;
    }

    bool hasTimeoutInt() {
        const uint32_t i2cTimeout = 0x1000;
        return registers->ISR & i2cTimeout;
    }
    void clearTimeoutInt() {
        const uint32_t i2cTimeout = 0x1000;
        registers->ICR |= i2cTimeout;
    }

    bool hasNackInt() {
        const uint32_t i2cNack = 0x10;
        return registers->ISR & i2cNack;
    }
    void clearNackInt() {
        const uint32_t i2cNack = 0x10;
        registers->ICR |= i2cNack;
    }

    bool hasStopInt() {
        const uint32_t i2cStop = 0x20;
        return registers->ISR & i2cStop;
    }
    void clearStopInt() {
        const uint32_t i2cStop = 0x20;
        registers->ICR |= i2cStop;
    }

    bool hasTransferCompleteInt() {
        const uint32_t i2cTransferComplete = 0x40;
        return registers->ISR & i2cTransferComplete;
    }

    bool hasReloadInt() {
        const uint32_t i2cReload = 0x80;
        return registers->ISR & i2cReload;
    }
    void setFullReload() {
        registers->CR2 |= 0x1FF << 16;
    }
    void loadSize(uint8_t length) {
        registers->CR2 &= ~(0x1FF << 16);
        registers->CR2 |= length << 16;
    }

    bool isBusy() {
        const uint32_t i2cBusy = 0x8000;
        return registers->ISR & i2cBusy;
    }

    uint8_t* rxRegAddr() {
        return (uint8_t*) &registers->RXDR;
    }
    uint8_t* txRegAddr() {
        return (uint8_t*) &registers->TXDR;
    }
    void setupTransmit(bool setupDmas, bool setupInterrupts, uint16_t addr, bool tenBit, uint16_t txLen) {
        const uint32_t enableI2c10BitAddress = 0x00001000;
        const uint32_t enableI2cAutoEnd = 0x02000000;
        const uint32_t enableI2c10BitAddressing = 0x0800;
        const uint32_t i2cStart = 0x2000;

        if (hasStopInt()) {
            // STOP
            clearStopInt();
        }
        uint32_t cr2r = enableI2c10BitAddress; //we always want 10 bit addressing to be done properly, and we want to write
        cr2r |= enableI2cAutoEnd; // set autoend
        if (tenBit) {
            cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
        } else {
            addr <<= 1;
        }
        cr2r |= addr;
        uint16_t chunking;
        if (txLen > 255) {
            chunking = 0x1FF;
        } else {
            chunking = txLen;
        }
        cr2r |= chunking << 16;
        setupCr1(setupDmas, setupInterrupts);
        registers->CR2 = cr2r;
        registers->CR2 |= i2cStart;
    }
    void setupReceive(bool setupDmas, bool setupInterrupts, uint16_t addr, bool tenBit, uint16_t rxLen) {
        const uint32_t enableI2c10BitAddress = 0x00001000;
        const uint32_t enableI2cAutoEnd = 0x02000000;
        const uint32_t enableI2c10BitAddressing = 0x0800;
        const uint32_t i2cStart = 0x2000;
        const uint32_t i2cRxNTx = 0x0400;

        if (hasStopInt()) {
            // STOP
            clearStopInt();
        }
        uint32_t cr2r = enableI2c10BitAddress | i2cRxNTx; //and want 10 bit addressing to be done properly, and we want to read
        cr2r |= enableI2cAutoEnd; // set autoend
        if (tenBit) {
            cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
        } else {
            addr <<= 1;
        }
        cr2r |= addr;
        uint16_t chunking;
        if (rxLen > 255) {
            chunking = 0x1FF;
        } else {
            chunking = rxLen;
        }
        cr2r |= chunking << 16;
        setupCr1(setupDmas, setupInterrupts);
        registers->CR2 = cr2r;
        registers->CR2 |= i2cStart;
    }
    void setupErrorInterrupt() {
        const uint32_t enableI2cErrorInterrupt = 0x80;
        registers->CR1 |= enableI2cErrorInterrupt;
    }

};

template<class LL>
void I2cInternal<LL>::activateClock(I2cIdentifier id) {
    const uint32_t enableI2c1Registers = 0x00200000;
    const uint32_t enableI2c2Registers = 0x00400000;
    const uint32_t enableI2c3Registers = 0x40000000;
    const uint32_t enableI2c4Registers = 0x00000002;

    const uint32_t i2c1ClockMask = 0x00003000;
    const uint32_t i2c1ClockHSI16 = 0x00002000;

    const uint32_t i2c2ClockMask = 0x0000C000;
    const uint32_t i2c2ClockHSI16 = 0x00008000;

    const uint32_t i2c3ClockMask = 0x00030000;
    const uint32_t i2c3ClockHSI16 = 0x00020000;

    const uint32_t i2c4ClockMask = 0x00000003;
    const uint32_t i2c4ClockHSI16 = 0x00000002;

    GpioManager<LL>::getPin(scl).init();
    GpioManager<LL>::getPin(sda).init();
    if (id == 0) {
        RCC->APB1ENR1 |= enableI2c1Registers;
        RCC->CCIPR &= ~i2c1ClockMask;
        RCC->CCIPR |= i2c1ClockHSI16;
    } else if (id == 1) {
        RCC->APB1ENR1 |= enableI2c2Registers;
        RCC->CCIPR &= ~i2c2ClockMask;
        RCC->CCIPR |= i2c2ClockHSI16;
    } else if (id == 2) {
        RCC->APB1ENR1 |= enableI2c3Registers;
        RCC->CCIPR &= ~i2c3ClockMask;
        RCC->CCIPR |= i2c3ClockHSI16;
    } else {
        RCC->APB1ENR2 |= enableI2c4Registers;
        RCC->CCIPR2 &= ~i2c4ClockMask;
        RCC->CCIPR2 |= i2c4ClockHSI16;
    }
}

template<class LL>
void I2cInternal<LL>::activatePins() {
    GpioManager<LL>::getPin(scl).setPullMode(NoPull);
    GpioManager<LL>::getPin(scl).setOutputMode(OpenDrain);
    GpioManager<LL>::getPin(scl).setAlternateFunction(sclFunction);
    GpioManager<LL>::getPin(scl).setOutputSpeed(VeryHighSpeed);
    GpioManager<LL>::getPin(scl).setMode(AlternateFunction);
    GpioManager<LL>::getPin(sda).setPullMode(NoPull);
    GpioManager<LL>::getPin(sda).setOutputMode(OpenDrain);
    GpioManager<LL>::getPin(sda).setAlternateFunction(sdaFunction);
    GpioManager<LL>::getPin(sda).setOutputSpeed(VeryHighSpeed);
    GpioManager<LL>::getPin(sda).setMode(AlternateFunction);
}

template<class LL>
bool I2cInternal<LL>::recoverSdaJam() {
    int attempts = 18;
    GpioPin<LL> sdaPin = GpioManager<LL>::getPin(sda);
    GpioPin<LL> sclPin = GpioManager<LL>::getPin(scl);
    sdaPin.setPullMode(NoPull);
    sdaPin.setOutputMode(OpenDrain);
    sdaPin.setOutputSpeed(VeryHighSpeed);
    sclPin.setPullMode(NoPull);
    sclPin.setOutputMode(OpenDrain);
    sclPin.setOutputSpeed(VeryHighSpeed);
    sdaPin.set();
    sclPin.set();
    sdaPin.setMode(Output);
    sclPin.setMode(Output);
    sdaPin.set();
    if (sdaPin.read()) {
        activatePins();
        return true;
    }
    sclPin.set();
    SystemMilliClock<LL>::blockDelayMillis(10);
    if (!sclPin.read()) {
        activatePins();
        return false;
    }
    while (!sdaPin.read() && attempts > 0) {
        sclPin.set();
        SystemMilliClock<LL>::blockDelayMillis(10);
        sclPin.reset();
        SystemMilliClock<LL>::blockDelayMillis(10);
        attempts--;
    }
    sdaPin.reset();
    sclPin.set();
    sdaPin.set();
    sdaPin.setMode(AlternateFunction);
    sclPin.setMode(AlternateFunction);
    activatePins();
    return sdaPin.read();
}

template<class LL>
void I2cInternal<LL>::setFrequency(Clock<LL> *clock, I2cFrequency freq) {
    const uint32_t enableI2c = 0x1;

    const uint32_t sclDelay3 = 0x00200000;
    const uint32_t sclDelay4 = 0x00300000;
    const uint32_t sclDelay5 = 0x00400000;

    const uint32_t sdaDelay0 = 0x00000000;
    const uint32_t sdaDelay2 = 0x00020000;
    const uint32_t sdaDelay3 = 0x00030000;

    const uint32_t sclHigh2 = 0x0100;
    const uint32_t sclHigh4 = 0x0300;
    const uint32_t sclHigh16 = 0x0F00;
    const uint32_t sclHigh184 = 0xC300;

    const uint32_t sclLow4 = 0x03;
    const uint32_t sclLow10 = 0x09;
    const uint32_t sclLow20 = 0x13;
    const uint32_t sclLow200 = 0xC7;

    if (registers == NULL) {
        return;
    }
    registers->CR1 &= ~enableI2c; // turn off peripheral
    // Always uses PCLK_1
    if (freq == kHz10) {
        uint8_t scale = clock->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        registers->TIMINGR = sclDelay5 | sdaDelay2 | sclHigh184 | sclLow200 | (scale << 28);
    } else if (freq == kHz100) {
        uint8_t scale = clock->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        registers->TIMINGR = sclDelay5 | sdaDelay2 | sclHigh16 | sclLow20 | (scale << 28);
    } else if (freq == kHz400) {
        uint8_t scale = clock->getDivider(8000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        registers->TIMINGR = sclDelay4 | sdaDelay3 | sclHigh4 | sclLow10 | (scale << 28);
    } else if (freq == kHz1000) {
        uint8_t scale = clock->getDivider(16000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        registers->TIMINGR = sclDelay3 | sdaDelay0 | sclHigh2 | sclLow4 | (scale << 28);
    }
    registers->CR1 |= enableI2c; // turn on peripheral
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CINTERNAL_HPP_ */

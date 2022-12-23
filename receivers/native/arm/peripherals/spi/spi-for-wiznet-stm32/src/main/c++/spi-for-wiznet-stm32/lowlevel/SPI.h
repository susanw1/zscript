/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_UDP_MODULE_LOWLEVEL_SPI_H_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_UDP_MODULE_LOWLEVEL_SPI_H_

#include <llIncludes.hpp>

#include <pins-ll/lowlevel/GpioManager.hpp>
#include <pins-ll/lowlevel/Gpio.hpp>

#define SPI_SCK_Inner(SPI,PIN) SPI##_SCK_##PIN
#define SPI_SCK(SPI,PIN) SPI_SCK_Inner(SPI, PIN)

#define SPI_MISO_Inner(SPI,PIN) SPI##_MISO_##PIN
#define SPI_MISO(SPI,PIN) SPI_MISO_Inner(SPI, PIN)

#define SPI_MOSI_Inner(SPI,PIN) SPI##_MOSI_##PIN
#define SPI_MOSI(SPI,PIN) SPI_MOSI_Inner(SPI, PIN)

#ifndef USE_SPI_1
#error SPI_1 is needed for udp comunications...
#endif

template<class LL>
class SPIClass {
    static void setupPin(GpioPinName name, uint8_t af) {
        GpioPin<LL> pin = GpioManager<LL>::getPin(name);
        pin.init();
        pin.setAlternateFunction(af);
        pin.setOutputSpeed(VeryHighSpeed);
        pin.setMode(AlternateFunction);
        pin.setOutputMode(PushPull);
        pin.setPullMode(NoPull);
    }
public:
    // Initialize the SPI library
    static void begin();

    static void beginTransaction();

    static uint8_t transfer(uint8_t data);

    static void transfer(uint8_t *buf, uint16_t count);

    static void endTransaction(void);

    static void end();
};

template<class LL>
extern SPIClass<LL> SPI;

template<class LL>
void SPIClass<LL>::begin() {
    const uint16_t enableSpiRegisterClock = 0x1000;
    const uint16_t setFifo1_4Full = 0x1000;
    const uint16_t dataSize8bit = 0x0700;
    const uint16_t setSoftwareSlaveManagement = 0x0200;
    const uint16_t setInternalSlaveSelect = 0x0100;
    const uint16_t setAsMaster = 0x0004;
    const uint16_t enableSpi = 0x0040;

    RCC->APB2ENR |= enableSpiRegisterClock;
    SPI1->CR2 = setFifo1_4Full | dataSize8bit;
    SPI1->CR1 = setAsMaster | setSoftwareSlaveManagement | setInternalSlaveSelect;

    setupPin (SPI_SCK(SPI_1, SPI_1_SCK));
    setupPin (SPI_MISO(SPI_1, SPI_1_MISO));
    setupPin (SPI_MOSI(SPI_1, SPI_1_MOSI));

    SPI1->CR1 |= enableSpi;
}

template<class LL>
void SPIClass<LL>::beginTransaction() {
}

#pragma GCC push_options
#pragma GCC optimize ("-O0")
template<class LL>
uint8_t SPIClass<LL>::transfer(uint8_t data) {
    const uint16_t fifoLevelMask = 0x0001;

    // Forces 8 bit read/write, to allow 1 byte at a time
    *((volatile uint8_t*) &(SPI1->DR)) = data;
    volatile bool b = true;
    volatile uint16_t *sr = (volatile uint16_t*) &SPI1->SR;
    while (b) {
        b = ((*sr) & fifoLevelMask) == 0;
    }
    return *((volatile uint8_t*) &(SPI1->DR));
}
#pragma GCC pop_options

template<class LL>
void SPIClass<LL>::transfer(uint8_t *buf, uint16_t count) {
    for (int i = 0; i < count; ++i) {
        buf[i] = transfer(buf[i]);
    }
}

template<class LL>
void SPIClass<LL>::endTransaction(void) {
}

template<class LL>
void SPIClass<LL>::end() {
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_UDP_MODULE_LOWLEVEL_SPI_H_ */

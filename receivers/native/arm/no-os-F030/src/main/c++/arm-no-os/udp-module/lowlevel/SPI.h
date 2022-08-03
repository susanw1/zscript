/*
 * SPI.h
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#define SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#include <arm-no-os/llIncludes.hpp>

#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>

template<class LL>
class SPIClass {
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

    GpioPin<LL> sck = GpioManager<LL>::getPin(PA_5);
    sck.init();
    sck.setAlternateFunction(0);
    sck.setOutputSpeed(VeryHighSpeed);
    sck.setMode(AlternateFunction);
    sck.setOutputMode(PushPull);
    sck.setPullMode(NoPull);

    GpioPin<LL> miso = GpioManager<LL>::getPin(PA_6);
    miso.init();
    miso.setAlternateFunction(0);
    miso.setOutputSpeed(VeryHighSpeed);
    miso.setMode(AlternateFunction);
    miso.setOutputMode(PushPull);
    miso.setPullMode(NoPull);

    GpioPin<LL> mosi = GpioManager<LL>::getPin(PA_7);
    mosi.init();
    mosi.setAlternateFunction(0);
    mosi.setOutputSpeed(VeryHighSpeed);
    mosi.setMode(AlternateFunction);
    mosi.setOutputMode(PushPull);
    mosi.setPullMode(NoPull);
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

#endif /* SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_ */

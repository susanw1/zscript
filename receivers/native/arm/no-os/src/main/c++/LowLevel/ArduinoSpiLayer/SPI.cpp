/*
 * SPI.hpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#include "SPI.h"

void SPIClass::begin() {
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

    GpioPin<GeneralHalSetup> *sck = GpioManager<GeneralHalSetup>::getPin(PA_5);
    sck->init();
    sck->setAlternateFunction(5);
    sck->setOutputSpeed(VeryHighSpeed);
    sck->setMode(AlternateFunction);
    sck->setOutputMode(PushPull);
    sck->setPullMode(NoPull);

    GpioPin<GeneralHalSetup> *miso = GpioManager<GeneralHalSetup>::getPin(PA_6);
    miso->init();
    miso->setAlternateFunction(5);
    miso->setOutputSpeed(VeryHighSpeed);
    miso->setMode(AlternateFunction);
    miso->setOutputMode(PushPull);
    miso->setPullMode(NoPull);

    GpioPin<GeneralHalSetup> *mosi = GpioManager<GeneralHalSetup>::getPin(PA_7);
    mosi->init();
    mosi->setAlternateFunction(5);
    mosi->setOutputSpeed(VeryHighSpeed);
    mosi->setMode(AlternateFunction);
    mosi->setOutputMode(PushPull);
    mosi->setPullMode(NoPull);
    SPI1->CR1 |= enableSpi;
}

void SPIClass::beginTransaction() {
}

#pragma GCC optimize ("-O0")
uint8_t SPIClass::transfer(uint8_t data) {
    const uint16_t fifoLevelMask = 0x0600;

    // Forces 8 bit read/write, to allow 1 byte at a time
    *((volatile uint8_t*) &(SPI1->DR)) = data;
    while (!((*((volatile uint16_t*) &SPI1->SR)) & fifoLevelMask))
        ;
    return *((volatile uint8_t*) &(SPI1->DR));
}

void SPIClass::transfer(uint8_t *buf, uint16_t count) {
    for (int i = 0; i < count; ++i) {
        buf[i] = transfer(buf[i]);
    }
}

void SPIClass::endTransaction(void) {
}

void SPIClass::end() {
}

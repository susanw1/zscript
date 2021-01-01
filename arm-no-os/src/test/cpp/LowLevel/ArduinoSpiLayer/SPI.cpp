/*
 * SPI.hpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#include "SPI.h"

void SPIClass::begin() {
    RCC->APB2ENR |= 0x1000;
    SPI1->CR2 = 0x1700;
    SPI1->CR1 = 0x033C;
    GpioPin *sck = GpioManager::getPin(PA_5);
    sck->init();
    sck->setAlternateFunction(5);
    sck->setOutputSpeed(VeryHighSpeed);
    sck->setMode(AlternateFunction);
    sck->setOutputMode(PushPull);
    sck->setPullMode(NoPull);

    GpioPin *miso = GpioManager::getPin(PA_6);
    miso->init();
    miso->setAlternateFunction(5);
    miso->setOutputSpeed(VeryHighSpeed);
    miso->setMode(AlternateFunction);
    miso->setOutputMode(PushPull);
    miso->setPullMode(NoPull);

    GpioPin *mosi = GpioManager::getPin(PA_7);
    mosi->init();
    mosi->setAlternateFunction(5);
    mosi->setOutputSpeed(VeryHighSpeed);
    mosi->setMode(AlternateFunction);
    mosi->setOutputMode(PushPull);
    mosi->setPullMode(NoPull);
    SPI1->CR1 |= 0x040;
}

void SPIClass::beginTransaction() {
}

uint8_t SPIClass::transfer(uint8_t data) {
    // Forces 8 bit read/write, to allow 1 byte at a time
    *((volatile uint8_t*) &(SPI1->DR)) = data;
    while (!(SPI1->SR & 0x0600))
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

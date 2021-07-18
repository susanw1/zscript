/*
 * I2cInternal.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../../I2cLowLevel/specific/I2cInternal.hpp"

void I2cInternal::activateClock(I2cIdentifier id) {
    GpioManager::getPin(scl)->init();
    GpioManager::getPin(sda)->init();
    if (id == 0) {
        RCC->APB1ENR1 |= 0x00200000;
        RCC->CCIPR &= ~0x00003000;
        RCC->CCIPR |= 0x00002000;
    } else if (id == 1) {
        RCC->APB1ENR1 |= 0x00400000;
        RCC->CCIPR &= ~0x0000C000;
        RCC->CCIPR |= 0x00008000;
    } else if (id == 2) {
        RCC->APB1ENR1 |= 0x40000000;
        RCC->CCIPR &= ~0x00030000;
        RCC->CCIPR |= 0x00020000;
    } else {
        RCC->APB1ENR2 |= 0x00000002;
        RCC->CCIPR2 &= ~0x00000003;
        RCC->CCIPR2 |= 0x00000002;
    }
}

void I2cInternal::activatePins() {
    GpioManager::getPin(scl)->setPullMode(NoPull);
    GpioManager::getPin(scl)->setOutputMode(OpenDrain);
    GpioManager::getPin(scl)->setAlternateFunction(sclFunction);
    GpioManager::getPin(scl)->setOutputSpeed(VeryHighSpeed);
    GpioManager::getPin(scl)->setMode(AlternateFunction);
    GpioManager::getPin(sda)->setPullMode(NoPull);
    GpioManager::getPin(sda)->setOutputMode(OpenDrain);
    GpioManager::getPin(sda)->setAlternateFunction(sdaFunction);
    GpioManager::getPin(sda)->setOutputSpeed(VeryHighSpeed);
    GpioManager::getPin(sda)->setMode(AlternateFunction);
}
bool I2cInternal::recoverSdaJam() {
    int attempts = 18;
    GpioPin *sdaPin = GpioManager::getPin(sda);
    GpioPin *sclPin = GpioManager::getPin(scl);
    sdaPin->setPullMode(NoPull);
    sdaPin->setOutputMode(OpenDrain);
    sdaPin->setOutputSpeed(VeryHighSpeed);
    sclPin->setPullMode(NoPull);
    sclPin->setOutputMode(OpenDrain);
    sclPin->setOutputSpeed(VeryHighSpeed);
    sdaPin->set();
    sclPin->set();
    sdaPin->setMode(Output);
    sclPin->setMode(Output);
    sdaPin->set();
    if (sdaPin->read()) {
        return true;
    }
    sclPin->set();
    for (int i = 0; i < 0x1000; ++i)
        ;
    if (!sclPin->read()) {
        return false;
    }
    while (!sdaPin->read() && attempts > 0) {
        sclPin->set();
        for (volatile uint32_t i = 0; i < 0x1000; ++i)
            ;
        sclPin->reset();
        for (volatile uint32_t i = 0; i < 0x1000; ++i)
            ;
        attempts++;
    }
    sdaPin->reset();
    sclPin->set();
    sdaPin->set();
    sdaPin->setMode(AlternateFunction);
    sclPin->setMode(AlternateFunction);
    return sdaPin->read();
}

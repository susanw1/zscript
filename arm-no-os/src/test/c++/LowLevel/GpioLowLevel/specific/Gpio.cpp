/*
 * Gpio.cpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */
#include "../Gpio.hpp"

void GpioPin::init() {
    GpioManager::activateClock(pin);
}
void GpioPin::write(bool value) {
    if (value) {
        port->setPin(pin);
    } else {
        port->resetPin(pin);
    }
}

void GpioPin::set() {
    port->setPin(pin);
}

void GpioPin::reset() {
    port->resetPin(pin);
}

bool GpioPin::read() {
    return port->getPinValue(pin);
}

void GpioPin::setOutputMode(OutputMode mode) {
    port->setOutputType(pin, mode);
}

void GpioPin::setPullMode(PullMode mode) {
    port->setPullMode(pin, mode);
}

void GpioPin::setMode(PinMode mode) {
    port->setMode(pin, mode);
}

void GpioPin::setOutputSpeed(PinSpeed speed) {
    port->setOutputSpeed(pin, speed);
}
void GpioPin::setAlternateFunction(uint8_t function) {
    port->setAlternateFunction(pin, function);
}

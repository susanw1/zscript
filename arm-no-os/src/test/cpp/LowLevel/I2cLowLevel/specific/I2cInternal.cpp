/*
 * I2cInternal.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../../I2cLowLevel/specific/I2cInternal.hpp"

void I2cInternal::activateClock(I2cIdentifier id) {
    if (id == 0) {
        __HAL_RCC_I2C1_CLK_ENABLE();
    } else if (id == 0) {
        __HAL_RCC_I2C2_CLK_ENABLE();
    } else if (id == 0) {
        __HAL_RCC_I2C3_CLK_ENABLE();
    } else {
        __HAL_RCC_I2C4_CLK_ENABLE();
    }
}
void I2cInternal::activatePins() {
    // the constants are used by mbed: speed max, no pull up/down, open drain, alternate function
    pin_function(sda, (((uint32_t) sdaFunction) << 10) | 0x000003CA);
    pin_function(scl, (((uint32_t) sclFunction) << 10) | 0x000003CA);
}
bool I2cInternal::recoverSdaJam() {
    int attempts = 18;
    mbed::DigitalInOut pin_sda(sda, PIN_INPUT, OpenDrain, 1);
    mbed::DigitalInOut pin_scl(scl, PIN_INPUT, OpenDrain, 1);
    pin_sda.input();
    pin_scl.input();
    if (pin_sda == 1) {
        return true;
    }
    pin_scl = 1;
    wait_us(100);
    if (pin_scl == 0) {
        return false;
    }
    pin_scl.output();
    pin_sda.input();
    while (pin_sda == 0 && attempts > 0) {
        pin_scl = 1;
        wait_us(100);
        pin_scl = 0;
        wait_us(100);
        attempts++;
    }
    pin_sda.output();
    pin_sda = 0;
    pin_scl = 1;
    pin_sda = 1;
    pin_sda.input();
    return pin_sda == 1;
}

/*
 * I2cInternal.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2CINTERNAL_HPP_
#define SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2CINTERNAL_HPP_

#include "../../GeneralLLSetup.hpp"
#include "../../GpioLowLevel/Gpio.hpp"
#include "../../GpioLowLevel/GpioManager.hpp"
#include "I2cRegisters.hpp"

struct I2cState {
    uint16_t repeatCount :10;
    bool hasTx :1;
    bool hasRx :1;
    bool hasTxRx :1;
    bool txDone :1;
};
class I2cInternal {
private:
    GpioPinName sda;
    PinAlternateFunction sdaFunction;
    GpioPinName scl;
    PinAlternateFunction sclFunction;
    I2cRegisters *registers;

public:
    I2cInternal() :
            sda(PA_1), sdaFunction(0), scl(PA_1), sclFunction(0), registers(NULL) {
    }
    I2cInternal(GpioPinName sda, PinAlternateFunction sdaFunction, GpioPinName scl, PinAlternateFunction sclFunction, I2cRegisters *registers) :
            sda(sda), sdaFunction(sdaFunction), scl(scl), sclFunction(sclFunction), registers(registers) {
    }
    void operator=(const I2cInternal &other) {
        sda = other.sda;
        sdaFunction = other.sdaFunction;
        scl = other.scl;
        sclFunction = other.sclFunction;
        registers = other.registers;
    }

    I2cRegisters* getRegisters() {
        return registers;
    }
    void activateClock(I2cIdentifier id);
    void activatePins();
    bool recoverSdaJam();
};

#endif /* SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2CINTERNAL_HPP_ */

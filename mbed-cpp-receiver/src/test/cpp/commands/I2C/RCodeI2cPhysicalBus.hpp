/*
 * RCodeI2cPhysicalBus.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CPHYSICALBUS_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CPHYSICALBUS_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "I2C.h"
#include "Callback.h"

#ifndef USE_I2C_STATIC_READ_BUFFERS
#include <mbed.h>
#endif

class RCodeI2cPhysicalBus {
private:
    mbed::I2C i2c;
#ifdef USE_I2C_STATIC_READ_BUFFERS
    uint8_t readBuffer[RCodeI2cParameters::i2cReadMaximum];
#else
    uint8_t *readBuffer = NULL;
#endif
public:
    RCodeI2cPhysicalBus(PinName sda, PinName scl) :
            i2c(sda, scl) {
    }
    int8_t transfer(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen,
            uint8_t rxLen, mbed::Callback<void(int)> callback, bool repeat) {
#ifndef USE_I2C_STATIC_READ_BUFFERS
        readBuffer = (uint8_t*) malloc(rxLen);
#endif
        return i2c.transfer(addr, (const char*) txBuffer, txLen,
                (char*) readBuffer, rxLen, callback, I2C_EVENT_ALL, repeat);
    }
    uint8_t* getReadBuffer() {
        return readBuffer;
    }
    void freeReadBuffer() {
#ifndef USE_I2C_STATIC_READ_BUFFERS
        free(readBuffer);
#endif
    }
    void setFrequency(int hz) {
        i2c.frequency(hz);
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CPHYSICALBUS_HPP_ */

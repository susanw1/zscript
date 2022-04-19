/*
 * ZcodeI2cPhysicalBus.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CPHYSICALBUS_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CPHYSICALBUS_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "../LowLevel/I2cLowLevel/I2c.hpp"
#include <parsing/ZcodeCommandSlot.hpp>

class ZcodeI2cPhysicalBus;
class ZcodeI2cSubsystem;

class ZcodeI2cPhysicalBus {
    private:
        I2c *i2c;

#ifdef USE_I2C_STATIC_READ_BUFFERS
        uint8_t readBuffer[ZcodeI2cParameters::i2cReadMaximum];
#else
        uint8_t *readBuffer = NULL;
        #endif
        ZcodeCommandSlot<ZcodeParameters> *callbackSlot = NULL;
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t) = NULL;
        I2cTerminationStatus status;
        uint8_t callbackData = 0;
        uint8_t currentBus = 0;

        friend ZcodeI2cSubsystem;

    public:
        ZcodeI2cPhysicalBus(I2c *i2c) :
                i2c(i2c), status(Complete) {
            i2c->init();
        }
        void asyncTransmit(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
                void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData);

        void asyncReceive(uint16_t addr, bool tenBit, uint8_t rxLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
                void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t),
                uint8_t callbackData);

        void asyncTransmitReceive(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen,
                ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
                void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData);

        uint8_t* getReadBuffer() {
            return readBuffer;
        }
        void freeReadBuffer() {
#ifndef USE_I2C_STATIC_READ_BUFFERS
            if (readBuffer != NULL) {
                free(readBuffer);
                readBuffer = NULL;
            }
#endif
        }
        void setFrequency(I2cFrequency freq) {
            i2c->setFrequency(freq);
        }

        void activateSwitchedBus(uint8_t busNum) {
            if (busNum == currentBus) {
                return;
            } else {
                //TODO: implement bus switching
            }
        }

        I2cTerminationStatus getStatus() {
            return status;
        }

        uint8_t getCallbackData() {
            return callbackData;
        }

        I2c* getI2c() {
            return i2c;
        }
};

#include "ZcodeI2cSubsystem.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_I2C_ZCODEI2CPHYSICALBUS_HPP_ */

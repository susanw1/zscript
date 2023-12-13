/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */




template<class LL>
class I2cManager;

// All methods guarantee that the callback is called. All methods use DMAs if the device supports them.
template<class LL>
class I2c {
private:
    I2cInternal<LL> i2c;
    I2cIdentifier id;

    uint16_t address = 0;
    uint16_t position = 0;
    uint16_t rxLen = 0;
    uint16_t txLen = 0;
    const uint8_t *txData = NULL;
    uint8_t *rxData = NULL;
    volatile I2cState state;

    void finish();

    I2cTerminationStatus transmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, bool stop);

public:

    I2c(I2cInternal<LL> i2c, I2cIdentifier id) :
            i2c(i2c), id(id), state(){
    }

    bool init();

    void setFrequency(I2cFrequency freq);

    I2cTerminationStatus transmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen) {
        return transmit10(address, tenBit, txData, txLen, true);
    }
    I2cTerminationStatus receive10(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen);

    I2cTerminationStatus transmitReceive10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen) {
        I2cTerminationStatus result = transmit10(address, tenBit, txData, txLen, false);
        if (result == Complete) {
            result = receive10(address, tenBit, rxData, rxLen);
            if (result == AddressNack) {
                return Address2Nack;
            }
            return result;
        } else {
            return result;
        }
    }
    I2cTerminationStatus transmit(uint8_t address, const uint8_t *txData, uint16_t txLen) {
        return transmit10(address, false, txData, txLen);
    }
    I2cTerminationStatus receive(uint8_t address, uint8_t *rxData, uint16_t rxLen) {
        return receive10(address, false, rxData, rxLen);
    }
    I2cTerminationStatus transmitReceive(uint8_t address, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen) {
        return transmitReceive10(address, false, txData, txLen, rxData, rxLen);
    }

    bool isSetUp() {
        return i2c.isSetUp();
    }

    bool lock() {
        if (!state.lockBool) {
            state.lockBool = true;
            return true;
        }
        return false;
    }

    bool isLocked() {
        return state.lockBool;
    }

    void unlock() {
        state.lockBool = false;
    }
};
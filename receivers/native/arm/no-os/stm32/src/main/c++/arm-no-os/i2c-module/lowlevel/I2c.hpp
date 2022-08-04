/*
 * I2c.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_
#define SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_

#include <arm-no-os/llIncludes.hpp>

#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/system/dma/DmaManager.hpp>

enum I2cFrequency {
    kHz10, kHz100, kHz400, kHz1000
};
#include "specific/I2cInternal.hpp"

enum I2cTerminationStatus {
    Complete,       // Successful completion - stopped when expected
    AddressNack,    // No response to address - device missing or busy
    DataNack,       // No response to a data byte - device stopped ack'ing. Probably bad.
    Address2Nack,   // No response to second address in send+receive pair. Eg bus noise, or device reset?
    BusError,       // Bus had a (bad) failure, eg jammed, or a STOP at wrong time, but recovered to usable state
    BusJammed,      // Bus jammed and recovery failed. Probably very bad.
    BusBusy,        // MultiMaster, so bus unavailable (MM not supported well at this time)
    MemoryError,    // DMA error - probably indicates a bug! Might imply buffer error?
    OtherError      // eg setup error, or unknown error - probably a bug.
};

template<class LL>
class I2cManager;

// All methods guarantee that the callback is called. All methods use DMAs if the device supports them.
template<class LL>
class I2c {
private:
    I2cInternal<LL> i2c;
    I2cIdentifier id;

#ifdef I2C_DMA
    Dma<LL> *dma = NULL;
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    DmaMuxRequest requestRx = DMAMUX_NO_MUX;

    void dmaInterrupt(DmaTerminationStatus status);
    #endif
    uint16_t address = 0;
    uint16_t position = 0;
    uint16_t rxLen = 0;
    uint16_t txLen = 0;
    const uint8_t *txData = NULL;
    uint8_t *rxData = NULL;
    volatile I2cState state;
    void (*volatile callback)(I2c*, I2cTerminationStatus);

    friend class I2cManager<LL> ;

    void interrupt();

    void finish();

    void restartReceive();

#ifdef I2C_DMA
    void setI2c(I2cInternal<LL> i2c, I2cIdentifier id, DmaMuxRequest requestTx, DmaMuxRequest requestRx) {
        this->i2c = i2c;
        this->id = id;
        this->requestTx = requestTx;
        this->requestRx = requestRx;
    }
#endif
    void setI2c(I2cInternal<LL> i2c, I2cIdentifier id) {
        this->i2c = i2c;
        this->id = id;

#ifdef I2C_DMA
        this->requestTx = DMAMUX_NO_MUX;
        this->requestRx = DMAMUX_NO_MUX;
#endif
    }
    I2cTerminationStatus transmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, bool stop);

public:

    I2c();

    void setDma(Dma<LL> *dma) {
        this->dma = dma;
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

    void asyncTransmit10(PeripheralOperationMode mode, uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen,
            void (*callback)(I2c*, I2cTerminationStatus));
    void asyncReceive10(PeripheralOperationMode mode, uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen,
            void (*callback)(I2c*, I2cTerminationStatus));
    void asyncTransmitReceive10(PeripheralOperationMode mode, uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData,
            uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus));

    void asyncTransmit(PeripheralOperationMode mode, uint8_t address, const uint8_t *txData, uint16_t txLen, void (*callback)(I2c*, I2cTerminationStatus)) {
        asyncTransmit10(mode, address, false, txData, txLen, callback);
    }
    void asyncReceive(PeripheralOperationMode mode, uint8_t address, uint8_t *rxData, uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus)) {
        asyncReceive10(mode, address, false, rxData, rxLen, callback);
    }
    void asyncTransmitReceive(PeripheralOperationMode mode, uint8_t address, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen,
            void (*callback)(I2c*, I2cTerminationStatus)) {
        asyncTransmitReceive10(mode, address, false, txData, txLen, rxData, rxLen, callback);
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

#include "specific/I2ccpp.hpp"
#include "I2cManager.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_ */

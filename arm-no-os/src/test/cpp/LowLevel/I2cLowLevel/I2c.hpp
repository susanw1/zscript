/*
 * I2c.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_
#define SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_
#include "../GpioLowLevel/Gpio.hpp"
#include "../GpioLowLevel/GpioManager.hpp"
#include "specific/I2cInternal.hpp"
#include "../DmaLowLevel/Dma.hpp"
#include "../DmaLowLevel/DmaManager.hpp"
#include "../GeneralLLSetup.hpp"
#include "specific/I2cInternal.hpp"

enum I2cTerminationStatus {
    Complete, AddressNack, DataNack, Address2Nack, BusError, BusJammed, BusBusy, MemoryError, OtherError
};

enum I2cFrequency {
    kHz10, kHz100, kHz400, kHz1000
};
class I2cManager;
class I2cInterruptManager;
// All methods guarantee that the callback is called. All methods use DMAs if the device supports them.
class I2c {
private:
    I2cInternal i2c;
    I2cIdentifier id;
    Dma *dma = DmaManager::getDmaById(0);
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    DmaMuxRequest requestRx = DMAMUX_NO_MUX;
    volatile bool lockBool = false;
    uint16_t address = 0;
    uint16_t txLen = 0;
    const uint8_t *txData = NULL;
    uint16_t rxLen = 0;
    uint8_t *rxData = NULL;
    volatile I2cState state;
    void (*volatile callback)(I2c*, I2cTerminationStatus);

    friend void I2cDmaCallback(Dma*, DmaTerminationStatus);
    friend I2cManager;
    friend I2cInterruptManager;

    void interrupt();

    void dmaInterrupt(DmaTerminationStatus status);

    void restartReceive();

    void setI2c(I2cInternal i2c, I2cIdentifier id, DmaMuxRequest requestTx, DmaMuxRequest requestRx) {
        this->i2c = i2c;
        this->id = id;
        this->requestTx = requestTx;
        this->requestRx = requestRx;
        this->dma = DmaManager::getDmaById(id);
    }
public:
    I2c(I2c &&i2c);
    I2c();

    bool init();

    void setFrequency(I2cFrequency freq);

    void asyncTransmit(uint16_t address, const uint8_t *txData, uint16_t txLen, void (*callback)(I2c*, I2cTerminationStatus));

    void asyncReceive(uint16_t address, uint8_t *rxData, uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus));

    void asyncTransmitReceive(uint16_t address, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen,
            void (*callback)(I2c*, I2cTerminationStatus));

    bool lock() {
        if (!lockBool) {
            lockBool = true;
            return true;
        }
        return false;
    }
    bool isLocked() {
        return lockBool;
    }
    void unlock() {
        lockBool = false;
    }
};

#include "../I2cLowLevel/I2cManager.hpp"
#endif /* SRC_TEST_CPP_COMMANDS_I2CNEWHAL_I2C_HPP_ */

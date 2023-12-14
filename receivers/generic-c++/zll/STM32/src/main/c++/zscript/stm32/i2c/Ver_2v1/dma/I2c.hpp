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

    Dma <LL> *dma = NULL;
    DmaRequest requestTx = DMAMUX_NO_MUX;
    DmaRequest requestRx = DMAMUX_NO_MUX;

    void dmaInterrupt(DmaTerminationStatus status);

    uint16_t address = 0;
    uint16_t position = 0;
    uint16_t rxLen = 0;
    uint16_t txLen = 0;
    const uint8_t *txData = NULL;
    uint8_t *rxData = NULL;
    volatile I2cState state;

    void (*volatile callback)(I2c *, I2cTerminationStatus);

    friend class I2cManager<LL>;

    void interrupt();

    void finish();

    void restartReceive();

public:

    I2c(I2cInternal<LL> i2c, I2cIdentifier id, DmaRequest requestTx, DmaRequest requestRx) :
            i2c(i2c), id(id), requestTx(requestTx), requestRx(requestRx), state(), callback(NULL) {
    }

    bool init();

    void setFrequency(I2cFrequency freq);

    void asyncTransmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen,
                         void (*callback)(I2c *, I2cTerminationStatus));

    void asyncReceive10(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen,
                        void (*callback)(I2c *, I2cTerminationStatus));

    void asyncTransmitReceive10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData,
                                uint16_t rxLen, void (*callback)(I2c *, I2cTerminationStatus));

    void asyncTransmit(uint8_t address, const uint8_t *txData, uint16_t txLen, void (*callback)(I2c *, I2cTerminationStatus)) {
        asyncTransmit10(address, false, txData, txLen, callback);
    }

    void asyncReceive(uint8_t address, uint8_t *rxData, uint16_t rxLen, void (*callback)(I2c *, I2cTerminationStatus)) {
        asyncReceive10(address, false, rxData, rxLen, callback);
    }

    void asyncTransmitReceive(uint8_t address, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen,
                              void (*callback)(I2c *, I2cTerminationStatus)) {
        asyncTransmitReceive10(address, false, txData, txLen, rxData, rxLen, callback);
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
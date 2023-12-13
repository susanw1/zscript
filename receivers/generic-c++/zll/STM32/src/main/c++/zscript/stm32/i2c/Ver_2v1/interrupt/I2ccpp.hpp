/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

template<class LL>
bool I2c<LL>::init() {
    state.hasRx = false;
    state.hasTx = false;
    state.hasTxRx = false;
    state.txDone = false;
    state.lockBool = false;
    i2c.activateClock(id);
    i2c.disablePeripheral();
    setFrequency(kHz100);
    i2c.doBasicSetup();
    bool worked = i2c.recoverSdaJam(); // just make sure no stupid states
    i2c.activatePins();
    i2c.enablePeripheral();
    return worked;
}

template<class LL>
void I2c<LL>::setFrequency(I2cFrequency freq) {
    i2c.disablePeripheral();
    i2c.setFrequency(ClockManager<LL>::getClock(HSI), freq);
    i2c.enablePeripheral();
}

template<class LL>
void I2c<LL>::finish() {
    state.hasRx = false;
    state.hasTx = false;
    state.hasTxRx = false;
    state.txDone = false;
    txLen = 0;
    txData = NULL;
    rxLen = 0;
    rxData = NULL;
    position = 0;
}

template<class LL>
void I2c<LL>::interrupt() {
    if (i2c.hasReadDataInt() && position != rxLen) {
        if (position == rxLen) {
            i2c.setNackAndStop();
        } else {
            rxData[position++] = i2c.readData();
        }
    } else if (i2c.hasNoSendDataInt() && position != txLen) {
        if (position == txLen) {
            i2c.setNackAndStop();
        } else {
            i2c.sendData(txData[position++]);
        }
    } else if (i2c.hasBusErrorInt()) {
        // BERR
        if (callback != NULL) {
            callback(this, BusError);
        }
        finish();
        i2c.clearBusErrorInt();
    } else if (i2c.hasArbitrationLossInt()) {
        // ARLO
        bool worked = init();
        if (callback != NULL) {
            if (worked) {
                callback(this, BusError);
            } else {
                callback(this, BusJammed);
            }
        }
        finish();
        i2c.clearArbitrationLossInt();
    } else if (i2c.hasTimeoutInt()) {
        bool worked = init();
        if (callback != NULL) {
            // TIMEOUT
            if (worked) {
                callback(this, BusError);
            } else {
                callback(this, BusJammed);
            }
        }
        finish();
        i2c.clearTimeoutInt();
    } else if (i2c.hasNackInt()) {
        // NACK
        if ((!state.hasTxRx && state.hasRx && position == 0)
            || (state.hasTx && (txLen == 0 || position == 0))) {
            callback(this, AddressNack);
        } else if (state.hasTxRx && state.txDone && position == 0) {
            callback(this, Address2Nack);
        } else {
            callback(this, DataNack);
        }
        finish();
        i2c.clearNackInt();
    } else if (i2c.hasStopInt()) {
        // STOP
        if (state.hasRx || state.hasTx || state.hasTxRx) {
            callback(this, Complete);
        }
        finish();
        i2c.clearStopInt();
    } else if (i2c.hasTransferCompleteInt()) {
        // TC - bidirectional
        if (!state.txDone) {
            restartReceive();
        } else {
            i2c.setNackAndStop();
            finish();
            callback(this, Complete);
        }
    } else if (i2c.hasReloadInt()) {
        // TCR - reload
        uint16_t length = 0;
        if (state.hasTx && !state.txDone) {
            length = txLen;
        } else if (state.hasRx) {
            length = rxLen;
        } // umm, what if neither - obviously an illegal state, but still
        length -= state.repeatCount * 0xFF;
        if (length > 255) {
            i2c.setFullReload();
        } else {
            i2c.loadSize(length);
        }
        state.repeatCount++;
    }
}

template<class LL>
void I2c<LL>::asyncTransmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen,
                              void (*callback)(I2c*, I2cTerminationStatus)) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasTx = true;
    this->txData = txData;
    this->txLen = txLen;
    this->address = address;
    state.tenBit = tenBit;
    i2c.setupTransmit(false, true, address, tenBit, txLen);
}

template<class LL>
void I2c<LL>::asyncReceive10(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen,
                             void (*callback)(I2c*, I2cTerminationStatus)) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    this->address = address;
    state.tenBit = tenBit;
    i2c.setupReceive(false, true, address, tenBit, rxLen);
}

template<class LL>
void I2c<LL>::asyncTransmitReceive10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData,
                                     uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus)) {
    state.hasTx = true;
    state.hasTxRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    asyncTransmit10(address, tenBit, txData, txLen, callback);
}

template<class LL>
void I2c<LL>::restartReceive() {
    if (rxLen == 0) {
        callback(this, OtherError);
        i2c.setStop();
        return;
    }
    if (address >= 1024 || (address >= 128 && !state.tenBit)) {
        callback(this, OtherError);
        i2c.setStop();
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.txDone = true;
    i2c.setupReceive(false, true, address, state.tenBit, rxLen);
}
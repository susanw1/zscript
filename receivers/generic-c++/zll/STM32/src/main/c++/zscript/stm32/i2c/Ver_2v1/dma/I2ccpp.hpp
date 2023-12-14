/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
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
    setFrequency(I2cFrequency::kHz100);
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
void I2c<LL>::dmaInterrupt(DmaTerminationStatus status) {
    if (status == SetupError) {
        callback(this, I2cTerminationStatus::OtherError);
    } else if (status == DmaError) {
        callback(this, I2cTerminationStatus::MemoryError);
    } else if (status == DataTransferComplete) {
        if (state.hasTxRx && state.txDone) {
            i2c.setStop();
        } else {
            callback(this, I2cTerminationStatus::OtherError); // as we set the DMA to be able to transfer 1 extra byte
        }
    }
}

template<class LL>
void I2c<LL>::finish() {
    if ((state.hasRx && (!state.hasTxRx || state.txDone)) || txLen != 0) {
        dma->halt();
        dma->unlock();
    }
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
        callback(this, I2cTerminationStatus::OtherError);
    } else if (i2c.hasNoSendDataInt() && position != txLen) {
        callback(this, I2cTerminationStatus::OtherError);
    } else if (i2c.hasBusErrorInt()) {
        // BERR
        if (callback != NULL) {
            callback(this, I2cTerminationStatus::BusError);
        }
        finish();
        i2c.clearBusErrorInt();
    } else if (i2c.hasArbitrationLossInt()) {
        // ARLO
        if ((state.hasRx && (!state.hasTxRx || state.txDone)) || txLen != 0) {
            dma->halt();
            dma->unlock();
        }
        bool worked = init();
        if (callback != NULL) {
            if (worked) {
                callback(this, I2cTerminationStatus::BusError);
            } else {
                callback(this, I2cTerminationStatus::BusJammed);
            }
        }
        finish();
        i2c.clearArbitrationLossInt();
    } else if (i2c.hasTimeoutInt()) {
        bool worked = init();
        if (callback != NULL) {
            // TIMEOUT
            if (worked) {
                callback(this, I2cTerminationStatus::BusError);
            } else {
                callback(this, I2cTerminationStatus::BusJammed);
            }
        }
        finish();
        i2c.clearTimeoutInt();
    } else if (i2c.hasNackInt()) {
        // NACK
        if ((!state.hasTxRx && state.hasRx && dma->fetchRemainingTransferLength() == rxLen)
            || (state.hasTx && (txLen == 0 || dma->fetchRemainingTransferLength() == txLen))) {
            callback(this, I2cTerminationStatus::AddressNack);
        } else if (state.hasTxRx && state.txDone && dma->fetchRemainingTransferLength() == rxLen) {
            callback(this, I2cTerminationStatus::Address2Nack);
        } else {
            callback(this, I2cTerminationStatus::DataNack);
        }
        finish();
        i2c.clearNackInt();
    } else if (i2c.hasStopInt()) {
        // STOP
        if (state.hasRx || state.hasTx || state.hasTxRx) {
            callback(this, I2cTerminationStatus::Complete);
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
            callback(this, I2cTerminationStatus::Complete);
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
                              void (*callback)(I2c *, I2cTerminationStatus)) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, I2cTerminationStatus::BusBusy);
        return;
    }
    if (txLen != 0 && !dma->lock()) {
        callback(this, I2cTerminationStatus::BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, I2cTerminationStatus::OtherError);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasTx = true;
    this->txData = txData;
    this->txLen = txLen;
    this->address = address;
    state.tenBit = tenBit;
    if (txLen != 0) {
        dma->peripheralWrite(requestTx, txData, true, i2c.txRegAddr(), false, txLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    }
    i2c.setupTransmit(true, true, address, tenBit, txLen);
}

template<class LL>
void I2c<LL>::asyncReceive10(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen,
                             void (*callback)(I2c *, I2cTerminationStatus)) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, I2cTerminationStatus::BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, I2cTerminationStatus::OtherError);
        return;
    }
    if (rxLen == 0) {
        callback(this, I2cTerminationStatus::OtherError);
        return;
    }
    if (!dma->lock()) {
        callback(this, I2cTerminationStatus::BusBusy);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    this->address = address;
    state.tenBit = tenBit;
    dma->peripheralRead(requestRx, i2c.rxRegAddr(), false, rxData, true, rxLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    i2c.setupReceive(true, true, address, tenBit, rxLen);
}

template<class LL>
void I2c<LL>::asyncTransmitReceive10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData,
                                     uint16_t rxLen, void (*callback)(I2c *, I2cTerminationStatus)) {
    state.hasTx = true;
    state.hasTxRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    asyncTransmit10(address, tenBit, txData, txLen, callback);
}

template<class LL>
void I2c<LL>::restartReceive() {
    if (txLen == 0 && !dma->lock()) {
        callback(this, I2cTerminationStatus::BusBusy);
        i2c.setStop();
        return;
    }
    dma->halt();
    if (rxLen == 0) {
        callback(this, I2cTerminationStatus::OtherError);
        i2c.setStop();
        dma->unlock();
        return;
    }
    if (address >= 1024 || (address >= 128 && !state.tenBit)) {
        callback(this, I2cTerminationStatus::OtherError);
        i2c.setStop();
        dma->unlock();
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.txDone = true;
    dma->peripheralRead(requestRx, i2c.rxRegAddr(), false, rxData, true, rxLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    i2c.setupReceive(true, true, address, state.tenBit, rxLen);
}
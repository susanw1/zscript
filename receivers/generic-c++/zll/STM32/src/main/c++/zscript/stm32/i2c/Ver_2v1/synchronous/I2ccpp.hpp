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
I2cTerminationStatus I2c<LL>::transmit10(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, bool stop) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        return BusBusy;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        return OtherError;
    }
    state.repeatCount = 0;
    state.hasRx = false;
    state.hasTx = false;
    state.txDone = false;
    state.hasTxRx = false;
    position = 0;
    i2c.setupTransmit(false, false, address, tenBit, txLen);
    while (true) {
        if (i2c.hasNoSendDataInt()) {
            if (position == txLen) {
                i2c.setNackAndStop();
            } else {
                i2c.sendData(txData[position++]);
            }
        } else if (i2c.hasBusErrorInt()) {
            finish();
            i2c.clearBusErrorInt();
            i2c.setupErrorInterrupt();
            return BusError;
        } else if (i2c.hasArbitrationLossInt()) {
            // ARLO
            bool worked = init();
            finish();
            i2c.clearArbitrationLossInt();
            i2c.setupErrorInterrupt();
            if (worked) {
                return BusError;
            } else {
                return BusJammed;
            }
        } else if (i2c.hasTimeoutInt()) {
            // TIMEOUT
            finish();
            i2c.clearTimeoutInt();
            i2c.setupErrorInterrupt();
            return BusJammed;
        } else if (i2c.hasNackInt()) {
            // NACK
            finish();
            i2c.clearNackInt();
            i2c.setupErrorInterrupt();
            if (position == 0) {
                return AddressNack;
            } else {
                return DataNack;
            }
        } else if (i2c.hasStopInt()) {
            // STOP
            finish();
            i2c.clearStopInt();
            i2c.setupErrorInterrupt();
            return Complete;
        } else if (i2c.hasTransferCompleteInt()) {
            // TC - bidirectional
            if (stop) {
                finish();
                i2c.setNackAndStop();
                i2c.setupErrorInterrupt();
                return Complete;
            } else {
                return Complete;
            }
        } else if (i2c.hasReloadInt()) {
            // TCR - reload
            uint16_t length = txLen;
            length -= state.repeatCount * 0xFF;
            if (length > 255) {
                i2c.setFullReload();
            } else {
                i2c.loadSize(length);
            }
            state.repeatCount++;
        }
    }
}
template<class LL>
I2cTerminationStatus I2c<LL>::receive10(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        return BusBusy;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        return OtherError;
    }
    state.repeatCount = 0;
    state.hasRx = false;
    state.hasTx = false;
    state.txDone = false;
    state.hasTxRx = false;
    position = 0;
    i2c.setupReceive(false, false, address, state.tenBit, rxLen);
    while (true) {
        if (i2c.hasReadDataInt()) {
            if (position == rxLen) {
                i2c.setNackAndStop();
                volatile uint8_t v = i2c.readData();
                (void) v;
                i2c.setupErrorInterrupt();
                return Complete;
            } else {
                rxData[position++] = i2c.readData();
            }
        } else if (i2c.hasBusErrorInt()) {
            finish();
            i2c.clearBusErrorInt();
            i2c.setupErrorInterrupt();
            return BusError;
        } else if (i2c.hasArbitrationLossInt()) {
            // ARLO
            bool worked = init();
            finish();
            i2c.clearArbitrationLossInt();
            i2c.setupErrorInterrupt();
            if (worked) {
                return BusError;
            } else {
                return BusJammed;
            }
        } else if (i2c.hasTimeoutInt()) {
            // TIMEOUT
            finish();
            i2c.clearTimeoutInt();
            i2c.setupErrorInterrupt();
            return BusJammed;
        } else if (i2c.hasNackInt()) {
            // NACK
            finish();
            i2c.clearNackInt();
            i2c.setupErrorInterrupt();
            if (position == 0) {
                return AddressNack;
            } else {
                return DataNack;
            }
        } else if (i2c.hasStopInt()) {
            // STOP
            finish();
            i2c.clearStopInt();
            i2c.setupErrorInterrupt();
            return Complete;
        } else if (i2c.hasTransferCompleteInt()) {
            // TC - bidirectional
            finish();
            i2c.setNackAndStop();
            i2c.setupErrorInterrupt();
            return Complete;
        } else if (i2c.hasReloadInt()) {
            // TCR - reload
            uint16_t length = rxLen;
            length -= state.repeatCount * 0xFF;
            if (length > 255) {
                i2c.setFullReload();
            } else {
                i2c.loadSize(length);
            }
            state.repeatCount++;
        }
    }
}

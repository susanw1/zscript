/*
 * I2ccpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CCPP_HPP_

#include <arm-no-os/system/clock/ClockManager.hpp>
#include "../I2c.hpp"

template<class LL>
I2c<LL>::I2c() :
        i2c(), id(0), state( { 0, false, false, false, false, false, false, false }), callback(NULL) {
}

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
void I2c<LL>::dmaInterrupt(DmaTerminationStatus status) {
    if (status == SetupError) {
        callback(this, OtherError);
    } else if (status == DmaError) {
        callback(this, MemoryError);
    } else if (status == DataTransferComplete) {
        if (state.hasTxRx && state.txDone) {
            i2c.setStop();
        } else {
            callback(this, OtherError); // as we set the DMA to be able to transfer 1 extra byte
        }
    }
}

template<class LL>
void I2c<LL>::finish() {
    if (((state.hasRx && (!state.hasTxRx || state.txDone)) || txLen != 0) && state.useDmas) {
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
        callback(this, BusError);
        finish();
        i2c.clearBusErrorInt();
    } else if (i2c.hasArbitrationLossInt()) {
        // ARLO
        if ((state.hasRx && (!state.hasTxRx || state.txDone)) || txLen != 0) {
            dma->halt();
            dma->unlock();
        }
        bool worked = init();
        if (worked) {
            callback(this, BusError);
        } else {
            callback(this, BusJammed);
        }
        finish();
        i2c.clearArbitrationLossInt();
    } else if (i2c.hasTimeoutInt()) {
        // TIMEOUT
        callback(this, BusJammed);
        finish();
        i2c.clearTimeoutInt();
    } else if (i2c.hasNackInt()) {
        // NACK
        if ((!state.hasTxRx && state.hasRx && ((state.useDmas && dma->fetchRemainingTransferLength() == rxLen) || (!state.useDmas && position == 0)))
                || (state.hasTx && (txLen == 0 || ((state.useDmas && dma->fetchRemainingTransferLength() == txLen) || (!state.useDmas && position == 0))))) {
            callback(this, AddressNack);
        } else if (state.hasTxRx && state.txDone && ((state.useDmas && dma->fetchRemainingTransferLength() == rxLen) || (!state.useDmas && position == 0))) {
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
void I2c<LL>::asyncTransmit10(PeripheralOperationMode mode, uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen,
        void (*callback)(I2c*, I2cTerminationStatus)) {
    if (mode == DMA && dma == NULL) {
        mode = INTERRUPT;
    }
    if (mode == SYNCHRONOUS) {
        callback(this, transmit10(address, tenBit, txData, txLen));
        return;
    }
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, BusBusy);
        return;
    }
    if (txLen != 0 && mode == DMA && !dma->lock()) {
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
    state.useDmas = mode == DMA;
    if (txLen != 0 && mode == DMA) {
        dma->peripheralWrite(requestTx, txData, true, i2c.txRegAddr(), false, txLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    }
    i2c.setupTransmit(mode == DMA, true, address, tenBit, txLen);
}

template<class LL>
void I2c<LL>::asyncReceive10(PeripheralOperationMode mode, uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen,
        void (*callback)(I2c*, I2cTerminationStatus)) {
    if (mode == DMA && dma == NULL) {
        mode = INTERRUPT;
    }
    if (mode == SYNCHRONOUS) {
        callback(this, receive10(address, tenBit, rxData, rxLen));
        return;
    }
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone || i2c.isBusy()) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    if (txLen != 0 && mode == DMA && !dma->lock()) {
        callback(this, BusBusy);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    this->address = address;
    state.useDmas = mode == DMA;
    state.tenBit = tenBit;
    if (mode == DMA) {
        dma->peripheralRead(requestRx, i2c.rxRegAddr(), false, rxData, true, rxLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    }
    i2c.setupReceive(state.useDmas, true, address, tenBit, rxLen);
}

template<class LL>
void I2c<LL>::asyncTransmitReceive10(PeripheralOperationMode mode, uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData,
        uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus)) {
    if (mode == DMA && dma == NULL) {
        mode = INTERRUPT;
    }
    if (mode == SYNCHRONOUS) {
        callback(this, transmitReceive10(address, tenBit, txData, txLen, rxData, rxLen));
        return;
    }
    state.hasTx = true;
    state.hasTxRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    asyncTransmit10(mode, address, tenBit, txData, txLen, callback);
}

template<class LL>
void I2c<LL>::restartReceive() {
    if (txLen == 0 && state.useDmas && !dma->lock()) {
        callback(this, BusBusy);
        i2c.setStop();
        return;
    }
    if (state.useDmas) {
        dma->halt();
    }
    if (rxLen == 0) {
        callback(this, OtherError);
        i2c.setStop();
        if (state.useDmas) {
            dma->unlock();
        }
        return;
    }
    if (address >= 1024 || (address >= 128 && !state.tenBit)) {
        callback(this, OtherError);
        i2c.setStop();
        if (state.useDmas) {
            dma->unlock();
        }
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.txDone = true;
    if (state.useDmas) {
        dma->peripheralRead(requestRx, i2c.rxRegAddr(), false, rxData, true, rxLen + 1, false, Medium, &I2cManager<LL>::dmaInterrupt, false);
    }
    i2c.setupReceive(state.useDmas, true, address, state.tenBit, rxLen);
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
            return BusError;
        } else if (i2c.hasArbitrationLossInt()) {
            // ARLO
            bool worked = init();
            finish();
            i2c.clearArbitrationLossInt();
            if (worked) {
                return BusError;
            } else {
                return BusJammed;
            }
        } else if (i2c.hasTimeoutInt()) {
            // TIMEOUT
            finish();
            i2c.clearTimeoutInt();
            return BusJammed;
        } else if (i2c.hasNackInt()) {
            // NACK
            finish();
            i2c.clearNackInt();
            if (position == 0) {
                return AddressNack;
            } else {
                return DataNack;
            }
        } else if (i2c.hasStopInt()) {
            // STOP
            finish();
            i2c.clearStopInt();
            return Complete;
        } else if (i2c.hasTransferCompleteInt()) {
            // TC - bidirectional
            if (stop) {
                finish();
                i2c.setNackAndStop();
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
                i2c.readData();
            } else {
                rxData[position++] = i2c.readData();
            }
        } else if (i2c.hasBusErrorInt()) {
            finish();
            i2c.clearBusErrorInt();
            return BusError;
        } else if (i2c.hasArbitrationLossInt()) {
            // ARLO
            bool worked = init();
            finish();
            i2c.clearArbitrationLossInt();
            if (worked) {
                return BusError;
            } else {
                return BusJammed;
            }
        } else if (i2c.hasTimeoutInt()) {
            // TIMEOUT
            finish();
            i2c.clearTimeoutInt();
            return BusJammed;
        } else if (i2c.hasNackInt()) {
            // NACK
            finish();
            i2c.clearNackInt();
            if (position == 0) {
                return AddressNack;
            } else {
                return DataNack;
            }
        } else if (i2c.hasStopInt()) {
            // STOP
            finish();
            i2c.clearStopInt();
            return Complete;
        } else if (i2c.hasTransferCompleteInt()) {
            // TC - bidirectional
            finish();
            i2c.setNackAndStop();
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

#endif /* SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CCPP_HPP_ */

/*
 * I2c.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../I2c.hpp"
#include "../../ClocksLowLevel/ClockManager.hpp"
I2c::I2c(I2c &&o) :
        i2c(o.i2c), id(o.id), dma(o.dma), requestTx(o.requestTx), requestRx(o.requestRx), lockBool(o.lockBool), state(((I2cState&) o.state)), callback(
                o.callback) {
}
I2c::I2c() :
        i2c(), id(0), state( { 0, false, false, false, false }), callback(NULL) {
}

bool I2c::init() {
    const uint32_t enableI2c = 0x1;
    const uint32_t enableI2cRxDma = 0x8000;
    const uint32_t enableI2cTxDma = 0x4000;
    const uint32_t enableI2cNackInterrupt = 0x10;
    const uint32_t enableI2cStopInterrupt = 0x20;
    const uint32_t enableI2cTransmitCompleteInterrupt = 0x40;
    const uint32_t enableI2cErrorInterrupt = 0x80;

    const uint32_t enableI2c10BitAddress = 0x00001000;

    const uint32_t enableI2cSclLowTimeout = 0x8000;
    const uint32_t sclLowTimeout3564Clock = 0x0600;

    state.hasRx = false;
    state.hasTx = false;
    state.hasTxRx = false;
    state.txDone = false;
    lockBool = false;
    i2c.activateClock(id);
    i2c.getRegisters()->CR1 &= ~enableI2c; // turn off peripheral
    setFrequency(kHz100);
    uint32_t ccr1 = enableI2cRxDma | enableI2cTxDma | enableI2cErrorInterrupt | enableI2cTransmitCompleteInterrupt | enableI2cStopInterrupt
            | enableI2cNackInterrupt | enableI2c;

    uint32_t ccr2 = enableI2c10BitAddress;

    i2c.getRegisters()->TIMEOUTR = enableI2cSclLowTimeout | sclLowTimeout3564Clock;  // disable timeout on clock stretch,
    // Enable SCL low timeout - with 50ms of timeout.

    i2c.getRegisters()->CR2 = ccr2;
    i2c.getRegisters()->CR1 = ccr1; // turn on peripheral
    bool worked = i2c.recoverSdaJam(); // just make sure no stupid states
    i2c.activatePins();
    return worked;
}

void I2c::setFrequency(I2cFrequency freq) {
    const uint32_t enableI2c = 0x1;

    const uint32_t sclDelay3 = 0x00200000;
    const uint32_t sclDelay4 = 0x00300000;
    const uint32_t sclDelay5 = 0x00400000;

    const uint32_t sdaDelay0 = 0x00000000;
    const uint32_t sdaDelay2 = 0x00020000;
    const uint32_t sdaDelay3 = 0x00030000;

    const uint32_t sclHigh2 = 0x0100;
    const uint32_t sclHigh4 = 0x0300;
    const uint32_t sclHigh16 = 0x0F00;
    const uint32_t sclHigh184 = 0xC300;

    const uint32_t sclLow4 = 0x03;
    const uint32_t sclLow10 = 0x09;
    const uint32_t sclLow20 = 0x13;
    const uint32_t sclLow200 = 0xC7;

    i2c.getRegisters()->CR1 &= ~enableI2c; // turn off peripheral
    // Always uses PCLK_1
    if (freq == kHz10) {
        uint8_t scale = ClockManager::getClock(HSI)->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = sclDelay5 | sdaDelay2 | sclHigh184 | sclLow200 | (scale << 28);
    } else if (freq == kHz100) {
        uint8_t scale = ClockManager::getClock(HSI)->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = sclDelay5 | sdaDelay2 | sclHigh16 | sclLow20 | (scale << 28);
    } else if (freq == kHz400) {
        uint8_t scale = ClockManager::getClock(HSI)->getDivider(8000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = sclDelay4 | sdaDelay3 | sclHigh4 | sclLow10 | (scale << 28);
    } else if (freq == kHz1000) {
        uint8_t scale = ClockManager::getClock(HSI)->getDivider(16000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = sclDelay3 | sdaDelay0 | sclHigh2 | sclLow4 | (scale << 28);
    }
    i2c.getRegisters()->CR1 |= enableI2c; // turn on peripheral
}

void I2c::dmaInterrupt(DmaTerminationStatus status) {
    const uint32_t i2cStopGenerate = 0x4000;
    if (status == SetupError) {
        callback(this, OtherError);
    } else if (status == DmaError) {
        callback(this, MemoryError);
    } else if (status == DataTransferComplete) {
        if (state.hasTxRx && state.txDone) {
            i2c.getRegisters()->CR2 |= i2cStopGenerate;
        } else {
            callback(this, OtherError); // as we set the DMA to be able to transfer 1 extra byte
        }
    }
}
void I2c::interrupt() {
    const uint32_t i2cBusError = 0x100;
    const uint32_t i2cArbitrationLoss = 0x200;
    const uint32_t i2cTimeout = 0x1000;
    const uint32_t i2cNack = 0x10;
    const uint32_t i2cStop = 0x20;
    const uint32_t i2cTransferComplete = 0x40;
    const uint32_t i2cReload = 0x80;

    if (i2c.getRegisters()->ISR & i2cBusError) {
        // BERR
        callback(this, BusError);
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
        i2c.getRegisters()->ICR |= i2cBusError;
    } else if (i2c.getRegisters()->ISR & i2cArbitrationLoss) {
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
        i2c.getRegisters()->ICR |= i2cArbitrationLoss;
    } else if (i2c.getRegisters()->ISR & i2cTimeout) {
        // TIMEOUT
        callback(this, BusJammed);
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
        i2c.getRegisters()->ICR |= i2cTimeout;
    } else if (i2c.getRegisters()->ISR & i2cNack) {
        // NACK
        if ((!state.hasTxRx && state.hasRx && dma->fetchRemainingTransferLength() == rxLen)
                || (state.hasTx && (txLen == 0 || dma->fetchRemainingTransferLength() == txLen))) {
            callback(this, AddressNack);
        } else if (state.hasTxRx && state.txDone && dma->fetchRemainingTransferLength() == rxLen) {
            callback(this, Address2Nack);
        } else {
            callback(this, DataNack);
        }
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
        i2c.getRegisters()->ICR |= i2cNack;
    } else if (i2c.getRegisters()->ISR & i2cStop) {
        // STOP
        if (state.hasRx || state.hasTx || state.hasTxRx) {
            callback(this, Complete);
        }
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
        i2c.getRegisters()->ICR |= i2cStop;
    } else if (i2c.getRegisters()->ISR & i2cTransferComplete) {
        // TC - bidirectional
        const uint32_t i2cEnableNack = 0x8000;
        const uint32_t i2cEnableStop = 0x4000;
        if (!state.txDone) {
            restartReceive();
            i2c.getRegisters()->ISR &= ~i2cTransferComplete;
        } else {
            i2c.getRegisters()->CR2 |= i2cEnableNack | i2cEnableStop;
            callback(this, Complete);
        }
    } else if (i2c.getRegisters()->ISR & i2cReload) {
        // TCR - reload
        uint16_t length = 0;
        if (state.hasTx && !state.txDone) {
            length = txLen;
        } else if (state.hasRx) {
            length = rxLen;
        } // umm, what if neither - obviously an illegal state, but still
        length -= state.repeatCount * 0xFF;
        if (length > 255) {
            i2c.getRegisters()->CR2 |= 0x1FF << 16;
        } else {
            i2c.getRegisters()->CR2 &= ~(0x1FF << 16);
            i2c.getRegisters()->CR2 |= ((uint32_t) length) << 16;
        }
        state.repeatCount++;
    }
}
void I2cDmaCallback(Dma *dma, DmaTerminationStatus status) {
    for (int i = 0; i < GeneralHalSetup::i2cCount; ++i) {
        if (I2cManager::getI2cById(i)->dma == dma) {
            I2cManager::getI2cById(i)->dmaInterrupt(status);
            break;
        }
    }
}
void I2c::asyncTransmit(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, void (*callback)(I2c*, I2cTerminationStatus)) {
    const uint32_t i2cBusy = 0x8000;
    const uint32_t enableI2c10BitAddress = 0x00001000;
    const uint32_t enableI2cAutoEnd = 0x02000000;
    const uint32_t enableI2c10BitAddressing = 0x0800;
    const uint32_t i2cStart = 0x2000;

    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & i2cBusy) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    if (txLen != 0 && !dma->lock()) {
        callback(this, BusBusy);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasTx = true;
    this->txData = txData;
    this->txLen = txLen;
    this->address = address;
    this->tenBit = tenBit;
    if (txLen != 0) {
        dma->peripheralWrite(requestTx, txData, true, (uint8_t*) &i2c.getRegisters()->TXDR, false, txLen + 1, false, Medium, &I2cDmaCallback, false);
    }
    uint32_t cr2r = enableI2c10BitAddress; //we always want 10 bit addressing to be done properly
    cr2r |= enableI2cAutoEnd; // set autoend
    if (tenBit) {
        cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
    } else {
        address <<= 1;
    }
    cr2r |= address;
    uint16_t chunking;
    if (txLen > 255) {
        chunking = 0x1FF;
    } else {
        chunking = txLen;
    }
    cr2r |= chunking << 16;
    i2c.getRegisters()->CR2 = cr2r;
    i2c.getRegisters()->CR2 |= i2cStart;
}

void I2c::asyncReceive(uint16_t address, bool tenBit, uint8_t *rxData, uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus)) {
    const uint32_t i2cBusy = 0x8000;
    const uint32_t enableI2c10BitAddress = 0x00001000;
    const uint32_t enableI2cAutoEnd = 0x02000000;
    const uint32_t enableI2c10BitAddressing = 0x0800;
    const uint32_t i2cStart = 0x2000;
    const uint32_t i2cRxNTx = 0x0400;

    if (rxLen == 0) {
        callback(this, OtherError);
        return;
    }
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & i2cBusy) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    if (!dma->lock()) {
        callback(this, BusBusy);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    this->address = address;
    this->tenBit = tenBit;
    dma->peripheralRead(requestRx, (uint8_t*) &i2c.getRegisters()->RXDR, false, rxData, true, rxLen + 1, false, Medium, &I2cDmaCallback, false);
    uint32_t cr2r = enableI2c10BitAddress | i2cStart | i2cRxNTx; //we always want to start, and want 10 bit addressing to be done properly, and we want to read
    cr2r |= enableI2cAutoEnd; // set autoend
    if (tenBit) {
        cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
    } else {
        address <<= 1;
    }
    cr2r |= address;
    uint16_t chunking;
    if (rxLen > 255) {
        chunking = 0x1FF;
    } else {
        chunking = rxLen;
    }
    cr2r |= chunking << 16;
    i2c.getRegisters()->CR2 = cr2r;
}

void I2c::asyncTransmitReceive(uint16_t address, bool tenBit, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen,
        void (*callback)(I2c*, I2cTerminationStatus)) {
    const uint32_t i2cBusy = 0x8000;
    const uint32_t enableI2c10BitAddress = 0x00001000;
    const uint32_t enableI2c10BitAddressing = 0x0800;
    const uint32_t i2cStart = 0x2000;

    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & i2cBusy) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024 || (address >= 128 && !tenBit)) {
        callback(this, OtherError);
        return;
    }
    if (txLen != 0 && !dma->lock()) {
        callback(this, BusBusy);
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.hasRx = true;
    state.hasTx = true;
    state.hasTxRx = true;
    this->rxData = rxData;
    this->rxLen = rxLen;
    this->txData = txData;
    this->txLen = txLen;
    this->address = address;
    this->tenBit = tenBit;
    if (txLen != 0) {
        dma->peripheralWrite(requestTx, txData, true, (uint8_t*) &i2c.getRegisters()->TXDR, false, txLen + 1, false, Medium, &I2cDmaCallback, false);
    }
    uint32_t cr2r = enableI2c10BitAddress | i2cStart; //we always want to start, and want 10 bit addressing to be done properly
    // Do not set auto-end
    if (tenBit) {
        cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
    } else {
        address <<= 1;
    }
    cr2r |= address;
    uint16_t chunking;
    if (txLen > 255) {
        chunking = 0x1FF;
    } else {
        chunking = txLen;
    }
    cr2r |= chunking << 16;
    i2c.getRegisters()->CR2 = cr2r;
}
void I2c::restartReceive() {
    const uint32_t i2cStop = 0x4000;
    const uint32_t enableI2c10BitAddress = 0x00001000;
    const uint32_t enableI2c10BitAddressing = 0x0800;
    const uint32_t i2cStart = 0x2000;
    const uint32_t i2cRxNTx = 0x0400;

    if (txLen == 0 && !dma->lock()) {
        callback(this, BusBusy);
        i2c.getRegisters()->CR2 |= i2cStop;
        return;
    }
    dma->halt();
    if (rxLen == 0) {
        callback(this, OtherError);
        i2c.getRegisters()->CR2 |= i2cStop;
        dma->halt();
        dma->unlock();
        return;
    }
    if (address >= 1024) {
        callback(this, OtherError);
        i2c.getRegisters()->CR2 |= i2cStop;
        dma->halt();
        dma->unlock();
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.txDone = true;
    dma->peripheralRead(requestRx, (uint8_t*) &i2c.getRegisters()->RXDR, false, rxData, true, rxLen, false, Medium, &I2cDmaCallback, false);
    uint32_t cr2r = enableI2c10BitAddress | i2cStart | i2cRxNTx; //we always want to start, and want 10 bit addressing to be done properly, and we want to read
    if (tenBit) {
        cr2r |= enableI2c10BitAddressing; // set 10 bit addressing
    } else {
        address <<= 1;
    }
    cr2r |= address;
    uint16_t chunking;
    if (rxLen > 255) {
        chunking = 0x1FF;
    } else {
        chunking = rxLen;
    }
    cr2r |= chunking << 16;
    i2c.getRegisters()->CR2 = cr2r;
}

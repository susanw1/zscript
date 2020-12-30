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
    state.hasRx = false;
    state.hasTx = false;
    state.hasTxRx = false;
    state.txDone = false;
    lockBool = false;
    i2c.activateClock(id);
    i2c.getRegisters()->CR1 &= ~1; // turn off peripheral
    setFrequency(kHz100);
    uint32_t ccr1 = 0xC0F1;
    uint32_t ccr2 = 0x00001000;
    i2c.getRegisters()->TIMEOUTR = 0x8600;  // disable timeout on clock stretch,
    // Enable SCL low timeout - with 50ms of timeout.

    i2c.getRegisters()->CR2 = ccr2;
    i2c.getRegisters()->CR1 = ccr1; // turn on peripheral
    bool worked = i2c.recoverSdaJam(); // just make sure no stupid states
    i2c.activatePins();
    return worked;
}

void I2c::setFrequency(I2cFrequency freq) {
    i2c.getRegisters()->CR1 &= ~1; // turn off peripheral
    // Always uses PCLK_1
    if (freq == kHz10) {
        uint8_t scale = ClockManager::getClock(PCLK_1)->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = 0x0042C3C7 | (scale << 28);
    } else if (freq == kHz100) {
        uint8_t scale = ClockManager::getClock(PCLK_1)->getDivider(4000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = 0x00420F13 | (scale << 28);
    } else if (freq == kHz400) {
        uint8_t scale = ClockManager::getClock(PCLK_1)->getDivider(8000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = 0x00330309 | (scale << 28);
    } else if (freq == kHz1000) {
        uint8_t scale = ClockManager::getClock(PCLK_1)->getDivider(8000) - 1;
        if (scale > 15) {
            scale = 15;
        }
        i2c.getRegisters()->TIMINGR = 0x00100103 | (scale << 28);
    }
    i2c.getRegisters()->CR1 |= 1; // turn on peripheral
}

void I2c::dmaInterrupt(DmaTerminationStatus status) {
    if (status == SetupError) {
        callback(this, OtherError);
    } else if (status == DmaError) {
        callback(this, MemoryError);
    } else if (status == DataTransferComplete) {
        if (state.hasTxRx && state.txDone) {
            i2c.getRegisters()->CR2 |= 0x4000;
        } else {
            callback(this, OtherError); // as we set the DMA to be able to transfer 1 extra byte
        }
    }
}
void I2c::interrupt() {
    if (i2c.getRegisters()->ISR & 0x100) {
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
        i2c.getRegisters()->ICR |= 0x100;
    } else if (i2c.getRegisters()->ISR & 0x200) {
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
        i2c.getRegisters()->ICR |= 0x200;
    } else if (i2c.getRegisters()->ISR & 0x1000) {
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
        i2c.getRegisters()->ICR |= 0x1000;
    } else if (i2c.getRegisters()->ISR & 0x10) {
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
        i2c.getRegisters()->ICR |= 0x10;
    } else if (i2c.getRegisters()->ISR & 0x20) {
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
        i2c.getRegisters()->ICR |= 0x20;
    } else if (i2c.getRegisters()->ISR & 0x40) {
        // TC - bidirectional
        if (!state.txDone) {
            restartReceive();
            i2c.getRegisters()->ISR &= ~0x40;
        } else {
            i2c.getRegisters()->CR2 |= 0xC000;
            callback(this, Complete);
        }
    } else if (i2c.getRegisters()->ISR & 0x80) {
        // TCR - reload
        uint16_t length;
        if (state.hasTx && !state.txDone) {
            length = txLen;
        } else if (state.hasRx) {
            length = rxLen;
        }
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
void I2c::asyncTransmit(uint16_t address, const uint8_t *txData, uint16_t txLen, void (*callback)(I2c*, I2cTerminationStatus)) {

    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & 0x8000) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024) {
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
    if (txLen != 0) {
        dma->peripheralWrite(requestTx, txData, true, (uint8_t*) &i2c.getRegisters()->TXDR, false, txLen + 1, false, Medium, &I2cDmaCallback, false);
    }
    uint32_t cr2r = 0x00001000; //we always want 10 bit addressing to be done properly
    cr2r |= 0x02000000; // set autoend
    if (address > 127) {
        cr2r |= 0x0800; // set 10 bit addressing
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
    i2c.getRegisters()->CR2 |= 0x2000;
}

void I2c::asyncReceive(uint16_t address, uint8_t *rxData, uint16_t rxLen, void (*callback)(I2c*, I2cTerminationStatus)) {
    if (rxLen == 0) {
        callback(this, OtherError);
        return;
    }
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & 0x8000) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024) {
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
    dma->peripheralRead(requestRx, (uint8_t*) &i2c.getRegisters()->RXDR, false, rxData, true, rxLen + 1, false, Medium, &I2cDmaCallback, false);
    uint32_t cr2r = 0x00003400; //we always want to start, and want 10 bit addressing to be done properly, and we want to read
    cr2r |= 0x02000000; // set autoend
    if (address > 127) {
        cr2r |= 0x0800; // set 10 bit addressing
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

void I2c::asyncTransmitReceive(uint16_t address, const uint8_t *txData, uint16_t txLen, uint8_t *rxData, uint16_t rxLen,
        void (*callback)(I2c*, I2cTerminationStatus)) {
    if (state.hasTx || state.hasRx || state.hasTxRx || state.txDone) {
        callback(this, BusBusy);
        return;
    }
    if (i2c.getRegisters()->ISR & 0x8000) {
        callback(this, BusBusy);
        return;
    }
    if (address >= 1024) {
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
    if (txLen != 0) {
        dma->peripheralWrite(requestTx, txData, true, (uint8_t*) &i2c.getRegisters()->TXDR, false, txLen + 1, false, Medium, &I2cDmaCallback, false);
    }
    uint32_t cr2r = 0x00003000; //we always want to start, and want 10 bit addressing to be done properly
    // Do not set auto-end
    if (address > 127) {
        cr2r |= 0x0800; // set 10 bit addressing
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
    if (txLen == 0 && !dma->lock()) {
        callback(this, BusBusy);
        i2c.getRegisters()->CR2 |= 0x4000;
        return;
    }
    dma->halt();
    if (rxLen == 0) {
        callback(this, OtherError);
        i2c.getRegisters()->CR2 |= 0x4000;
        dma->halt();
        dma->unlock();
        return;
    }
    if (address >= 1024) {
        callback(this, OtherError);
        i2c.getRegisters()->CR2 |= 0x4000;
        dma->halt();
        dma->unlock();
        return;
    }
    this->callback = callback;
    state.repeatCount = 0;
    state.txDone = true;
    dma->peripheralRead(requestRx, (uint8_t*) &i2c.getRegisters()->RXDR, false, rxData, true, rxLen, false, Medium, &I2cDmaCallback, false);
    uint32_t cr2r = 0x00001400; //we always want to start, and want 10 bit addressing to be done properly, and we want to read
    if (address > 127) {
        cr2r |= 0x0800; // set 10 bit addressing
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
    i2c.getRegisters()->CR2 |= 0x2000;
}

/*
 * Uart.cpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#include "../Uart.hpp"
#include "../UartManager.hpp"
#include "../../ClocksLowLevel/ClockManager.hpp"

void UartDmaCallback(Dma *dma, DmaTerminationStatus status) {
    for (int i = 0; i < GeneralHalSetup::uartCount; ++i) {
        if (UartManager::getUartById(i)->txDma == dma) {
            UartManager::getUartById(i)->dmaInterrupt(status);
            break;
        }
    }
}

bool Uart::write(const uint8_t *buffer, uint16_t length) {
    if (length == 0) { // otherwise things could break a bit...
        return true;
    }
    if (!txDma->lock()) { // if DMA is not locked, we go right ahead, otherwise, we need to ensure DMA-perceived atomicity
        txDma->halt();
        if (txDma->isLocked()) { //just to be sure...
            if (!txContinues && txLength + length < GeneralHalSetup::UartBufferTxSize) {
                txBuffer[txLength] = buffer[0]; // ensure that at least 1 byte is in buffer, so we don't run out of data - just in case
                txDma->setRemainingTransferLength(txDma->fetchRemainingTransferLength() + length);
                txLength += length;
                txDma->resume(); //we can afford the DMA to continue now as we have some data in place, and it can't terminate by accident
                for (uint16_t i = 1; i < length; i++) {
                    txBuffer[txLength + i] = buffer[i];
                }
                return true;
            } else if (!txContinues && length < txLength - txDma->fetchRemainingTransferLength() - 1) {
                txBuffer[0] = buffer[0]; // ensure that at least 1 byte is in buffer, so we don't run out of data - just in case

                txContinues = true;
                txLength = length; // atomicity again...
                txDma->resume(); //we can afford the DMA to continue now as we have some data in place, and it can't terminate by accident
                for (uint16_t i = 1; i < length; i++) {
                    txBuffer[i] = buffer[i];
                }
                return true;
            } else {
                txDma->resume();
                return false;
            }
        }
        txDma->resume();
    }
    if (length >= GeneralHalSetup::UartBufferTxSize) {
        return false;
    }
    for (uint16_t i = 0; i < length; i++) {
        txBuffer[i] = buffer[i];
    }
    txLength = length;
    txDma->peripheralWrite(requestTx, txBuffer, true, (uint8_t*) uart.getTransmitRegister(), false, length, false, Medium, &UartDmaCallback,
            false);
    return true;
}

void Uart::startWrite() {
    if (txDma->lock() && txLength != 0) {
        txDma->peripheralWrite(requestTx, txBuffer, true, (uint8_t*) uart.getTransmitRegister(), false, txLength, false, Medium, &UartDmaCallback,
                false);
    }
}
void Uart::dmaInterrupt(DmaTerminationStatus status) {
    if (txContinues) {
        txContinues = false;
        txDma->halt();
        txDma->peripheralWrite(requestTx, txBuffer, true, (uint8_t*) uart.getTransmitRegister(), false, txLength, false, Medium, &UartDmaCallback,
                false);
    } else {
        txDma->halt();
        txDma->unlock();
        txLength = 0;
    }
}

void Uart::interrupt() {
    clearRxFifo();
    uart.clearFlags();
}


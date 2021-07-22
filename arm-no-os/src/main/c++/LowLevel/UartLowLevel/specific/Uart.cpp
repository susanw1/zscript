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
void UartTxOverflowCallback(UartIdentifier id) {
    UartManager::getUartById(id)->txOverflowInterrupt();
}
void Uart::transmitWriteBuffer() {
    if (txDma->lock()) {
        dmaStartDist = txBuffer.getLinearReadLength();
        if (dmaStartDist != 0) {
            txDma->peripheralWrite(requestTx, txBuffer.getCurrentLinearRead(), true, (uint8_t*) uart.getTransmitRegister(), false,
                    dmaStartDist, false, Medium, &UartDmaCallback, false);
        }
    }
}

void Uart::clearRxFifo() {
    bool fitted = true; // use boolean to avoid atomicity issues...
    while (uart.hasRxFifoData()) {
        if (!rxBuffer.canWrite(4)) { //this ensures that we can always write the current byte + 2 more bytes (for overflow marking)
            fitted = false;
            break;
        }
        int16_t datum = uart.read();
        if (datum < 0) {
            if (datum != -UartFramingError) {
                rxBuffer.write(GeneralHalSetup::uartEscapingChar);
                rxBuffer.write((uint8_t) -datum);
            }
        } else if (datum == GeneralHalSetup::uartEscapingChar) {
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            if (targetValueCallback != NULL && targetValue == GeneralHalSetup::uartEscapingChar) {
                targetValueCallback(this);
            }
            availableData++;
        } else {
            rxBuffer.write((uint8_t) datum);
            if (targetValueCallback != NULL && targetValue == datum) {
                targetValueCallback(this);
            }
            availableData++;
        }
    }
    if (!fitted) {
        if (rxBuffer.canWrite(2)) { // the first time we overrun, we can always write this, but if we repeatedly overrun, we might not
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            rxBuffer.write(UartOverflowError);
        }
        rxOverflowCallback(uart.getId());
        while (uart.hasRxFifoData()) { //flush remaining data, as we can't fit it in...
            uart.read();
        }
    }
}

UartError Uart::getError(uint16_t length) {
    clearRxFifo();
    rxBuffer.resetPeek();
    for (int16_t i = 0; i < length; ++i) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            break;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val != GeneralHalSetup::uartEscapingChar) {
                rxBuffer.resetPeek();
                return (UartError) val;
            }
        }
    }
    rxBuffer.resetPeek();
    return UartNoError;
}

uint16_t Uart::read(uint8_t *buffer, uint16_t length) {
    clearRxFifo();
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.read();
        if (val == -1) {
            break;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.read();
            if (val == GeneralHalSetup::uartEscapingChar) {
                buffer[i] = val;
            } else {
                i--;
            }
        } else {
            buffer[i] = val;
        }
    }
    availableData -= i;
    return i;
}
int16_t Uart::read() {
    clearRxFifo();
    while (true) {
        int16_t val = rxBuffer.read();
        if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.read();
            if (val == GeneralHalSetup::uartEscapingChar) {
                availableData--;
                return val;
            }
        } else {
            if (val != -1) {
                availableData--;
            }
            return val; // this includes if read returned -1
        }
    }

}

int32_t Uart::getDistance(uint8_t value) {
    clearRxFifo();
    rxBuffer.resetPeek();
    uint16_t i = 0;
    while (true) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            rxBuffer.resetPeek();
            return -1;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == GeneralHalSetup::uartEscapingChar) {
                if (value == val) {
                    break;
                }
            } else {
                i--;
            }
        } else if (val == value) {
            break;
        }
        i++;
    }
    rxBuffer.resetPeek();
    return i + 1;
}
uint16_t Uart::skip(uint16_t length) { // there is no efficient skip, as we need to exclude escaped characters
    clearRxFifo();
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.read();
        if (val == -1) {
            break;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.read();
            if (val != GeneralHalSetup::uartEscapingChar) {
                i--;
            }
        }
    }
    availableData -= i;
    return i;
}
void Uart::txOverflowInterrupt() {
    transmitWriteBuffer(); // in case somebody forgot, as that would do it...
    uint16_t completedLength = txDma->fetchRemainingTransferLength();
    txBuffer.skip(dmaStartDist - completedLength); // move along as much as the DMA has allowed in the time since we last did that
    dmaStartDist -= completedLength;
}

void Uart::dmaInterrupt(DmaTerminationStatus status) {
    txBuffer.skip(dmaStartDist);
    dmaStartDist = txBuffer.getLinearReadLength();
    txDma->halt();
    if (dmaStartDist == 0) {
        txDma->unlock();
    } else {
        txDma->peripheralWrite(requestTx, txBuffer.getCurrentLinearRead(), true, (uint8_t*) uart.getTransmitRegister(), false,
                dmaStartDist, false, Medium, &UartDmaCallback, false);
    }
}
void Uart::init(void (*volatile bufferOverflowCallback)(UartIdentifier), uint32_t baud_rate, bool singleNdoubleStop) {
// buffer overflow callback can be NULL
    this->rxOverflowCallback = bufferOverflowCallback;
    this->txBuffer.setCallback(&UartTxOverflowCallback, uart.getId());
    uart.init(baud_rate, singleNdoubleStop);
}

void Uart::interrupt() {
    clearRxFifo();
    uart.clearFlags();
}

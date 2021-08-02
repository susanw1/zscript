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
        if (((Uart*) UartManager::getUartById(i))->txDma == dma) {
            ((Uart*) UartManager::getUartById(i))->dmaInterrupt(status);
            break;
        }
    }
}
void UartTxOverflowCallback(SerialIdentifier id) {
    ((Uart*) UartManager::getUartById(id))->txOverflowInterrupt();
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
            if (datum != -SerialFramingError) {
                rxBuffer.write(GeneralHalSetup::uartEscapingChar);
                rxBuffer.write((uint8_t) -datum);
            }
        } else if (datum == GeneralHalSetup::uartEscapingChar) {
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            if (targetValueCallback != NULL && targetValue == GeneralHalSetup::uartEscapingChar) {
                targetValueCallback(uart.getId());
            }
            availableData++;
        } else {
            rxBuffer.write((uint8_t) datum);
            if (targetValueCallback != NULL && targetValue == datum) {
                targetValueCallback(uart.getId());
            }
            availableData++;
        }
    }
    if (!fitted) {
        if (rxBuffer.canWrite(2)) { // the first time we overrun, we can always write this, but if we repeatedly overrun, we might not
            rxBuffer.write(GeneralHalSetup::uartEscapingChar);
            rxBuffer.write(SerialOverflowError);
        }
        rxOverflowCallback(uart.getId());
        while (uart.hasRxFifoData()) { //flush remaining data, as we can't fit it in...
            uart.read();
        }
    }
}

SerialError Uart::getError(uint16_t length) {
    clearRxFifo();
    peekDist = 0;
    rxBuffer.resetPeek();
    for (int16_t i = 0; i < length; ++i) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            break;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val != GeneralHalSetup::uartEscapingChar) {
                rxBuffer.resetPeek();
                return (SerialError) val;
            }
        }
    }
    rxBuffer.resetPeek();
    return SerialNoError;
}

uint16_t Uart::read(uint8_t *buffer, uint16_t length) {
    clearRxFifo();
    peekDist = 0;
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
    peekDist = 0;
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
    peekDist = 0;
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
    peekDist = 0;
    return i;
}

uint16_t Uart::availablePeek() {
    clearRxFifo();
    return availableData - peekDist;
}
uint16_t Uart::peek(uint8_t *buffer, uint16_t length) {
    clearRxFifo();
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            break;
        } else if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == GeneralHalSetup::uartEscapingChar) {
                buffer[i] = val;
            } else {
                i--;
            }
        } else {
            buffer[i] = val;
        }
    }
    peekDist += i;
    return i;
}
int16_t Uart::peek() {
    clearRxFifo();
    while (true) {
        int16_t val = rxBuffer.peek();
        if (val == GeneralHalSetup::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == GeneralHalSetup::uartEscapingChar) {
                peekDist++;
                return val;
            }
        } else {
            if (val != -1) {
                peekDist++;
            }
            return val; // this includes if read returned -1
        }
    }
} //-1 if no data

void Uart::resetPeek() {
    clearRxFifo();
    peekDist = 0;
    rxBuffer.resetPeek();
}
void Uart::skipToPeek() {
    clearRxFifo();
    availableData -= peekDist;
    peekDist = 0;
    rxBuffer.skipToPeek();
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
void Uart::init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop) {
// buffer overflow callback can be NULL
    this->rxOverflowCallback = bufferOverflowCallback;
    this->txBuffer.setCallback(&UartTxOverflowCallback, uart.getId());
    uart.init(baud_rate, singleNdoubleStop);
}

void Uart::interrupt() {
    clearRxFifo();
    uart.clearFlags();
}

/*
 * Uart.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#include "specific/UartInternal.hpp"

#include "../DmaLowLevel/Dma.hpp"
#include "../DmaLowLevel/DmaManager.hpp"
#include "../GeneralLLSetup.hpp"

enum UartError {
    None, ParityError, NoiseError
};

class UartManager;
class UartInterruptManager;
//always does 8 bit bytes, no parity - just for simplicity
class Uart {
private:
    UartInternal uart;
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    void (*volatile bufferOverflowCallback)(Uart*); //the buffer overflow handler can read in the callback to get data - if it doesn't clear enough space it gets called again
    Dma *txDma = NULL;
    uint16_t rxReadPos = 0;
    uint16_t rxWritePos = 0;
    bool readEscaped = false;
    uint8_t rxBuffer[GeneralHalSetup::UartBufferRxSize];
    uint16_t txLength = 0;
    uint16_t txContinues = false;
    uint8_t txBuffer[GeneralHalSetup::UartBufferTxSize];

    friend void UartDmaCallback(Dma*, DmaTerminationStatus);
    friend UartManager;
    friend UartInterruptManager;

    void interrupt();

    void dmaInterrupt(DmaTerminationStatus status);

public:
    void setUart(UartInternal uart, Dma *txDma, DmaMuxRequest requestTx) {
        this->uart = uart;
        this->txDma = txDma;
        this->requestTx = requestTx;
    }
    void init(void (*volatile bufferOverflowCallback)(Uart*), uint32_t baud_rate, bool singleNdoubleStop) {
        this->bufferOverflowCallback = bufferOverflowCallback;
        uart.init(baud_rate, this, singleNdoubleStop);
    }
    void setTargetValue(void (*volatile targetValueCallback)(Uart*), uint8_t targetValue) {
        uart.setTargetValue(targetValueCallback, targetValue);
    }
    void clearTargetValue() {
        uart.clearTargetValue();
    }
    uint16_t available() {
        clearRxFifo();
        return rxWritePos - rxReadPos;
    }
    UartIdentifier getId() {
        return uart.getId();
    }

    void clearRxFifo() {
        uint16_t maxLen = 0;
        if (rxWritePos < rxReadPos) {
            maxLen = rxReadPos - rxWritePos;
        } else {
            maxLen = GeneralHalSetup::UartBufferRxSize - rxWritePos;
        }
        // keep reading until we have some spare space and don't fill it - allow our buffer overflow caller to absorb any excess they want
        uint16_t change = uart.performRead(rxBuffer + rxWritePos, maxLen);
        while (change == maxLen) {
            rxWritePos += maxLen;
            if (rxWritePos == rxReadPos) {
                rxWritePos--; // allow the reader to function temporarily...
                bufferOverflowCallback(this);
                rxWritePos++;
                bool hasRead = rxWritePos != rxReadPos;
                if (rxWritePos < rxReadPos) {
                    maxLen = rxReadPos - rxWritePos;
                } else {
                    maxLen = GeneralHalSetup::UartBufferRxSize - rxWritePos;
                }
                if (!hasRead) {
                    rxReadPos = rxWritePos + 1; //move read to after overrun
                }
            }
            if (rxWritePos == GeneralHalSetup::UartBufferRxSize) {
                rxWritePos = 0;
                maxLen = rxReadPos - rxWritePos;
            }
            change = uart.performRead(rxBuffer + rxWritePos, maxLen);
        }
    }
    UartError getError(uint16_t length) {
        clearRxFifo();
        uint16_t i;
        bool isChasing = false;  //indicates whether the readPos is chasing the writePos (i.e. is behind it) as we have a ring buffer...
        if (rxReadPos <= rxWritePos) {
            isChasing = true;
        }
        uint16_t readTmp = rxReadPos;
        bool isEscaped = readEscaped;
        for (i = 0; (!isChasing || readTmp < rxWritePos) && i < length; readTmp++, i++) {
            if (readTmp == GeneralHalSetup::UartBufferRxSize) {
                readTmp = 0;
                isChasing = true;
            }
            if (isEscaped) {
                isEscaped = false;
                if (rxBuffer[readTmp] != GeneralHalSetup::uartEscapingChar) {
                    return (UartError) rxBuffer[readTmp];
                }
            } else if (rxBuffer[readTmp] == GeneralHalSetup::uartEscapingChar) {
                isEscaped = true;
                i--;
            }
        }
        return None;
    }

    uint16_t read(uint8_t *buffer, uint16_t length) {
        clearRxFifo();
        uint16_t i;
        bool isChasing = false;  //indicates whether the readPos is chasing the writePos (i.e. is behind it) as we have a ring buffer...
        if (rxReadPos <= rxWritePos) {
            isChasing = true;
        }
        for (i = 0; (!isChasing || rxReadPos < rxWritePos) && i < length; rxReadPos++, i++) {
            if (rxReadPos == GeneralHalSetup::UartBufferRxSize) {
                rxReadPos = 0;
                isChasing = true;
                if (rxReadPos == rxWritePos) {
                    break;
                }
            }
            if (readEscaped) {
                readEscaped = false;
                if (rxBuffer[rxReadPos] == GeneralHalSetup::uartEscapingChar) {
                    buffer[i] = GeneralHalSetup::uartEscapingChar;
                }
            } else if (rxBuffer[rxReadPos] == GeneralHalSetup::uartEscapingChar) {
                readEscaped = true;
                i--;
            } else {
                buffer[i] = rxBuffer[rxReadPos];
            }
        }
        return i;
    }

    int32_t getDistance(uint8_t value) {  //returns the number of bytes until the specified value appears, including the value
        clearRxFifo();
        bool isChasing = false;  //indicates whether the readPos is chasing the writePos (i.e. is behind it) as we have a ring buffer...
        if (rxReadPos <= rxWritePos) {
            isChasing = true;
        }
        uint16_t readTmp = rxReadPos;
        bool isEscaped = readEscaped;
        for (uint16_t i = 1; (!isChasing || readTmp < rxWritePos); readTmp++, i++) {
            if (readTmp == GeneralHalSetup::UartBufferRxSize) {
                readTmp = 0;
                isChasing = true;
                if (readTmp == rxWritePos) {
                    break;
                }
            }
            if (isEscaped) {
                isEscaped = false;
                if (rxBuffer[readTmp] == GeneralHalSetup::uartEscapingChar) {
                    if (value == GeneralHalSetup::uartEscapingChar) {
                        return i;
                    }
                }
            } else if (rxBuffer[readTmp] == GeneralHalSetup::uartEscapingChar) {
                isEscaped = true;
                i--;
            } else {
                if (value == rxBuffer[readTmp]) {
                    return i;
                }
            }
        }
        return -1;
    }

    uint16_t
    skip(uint16_t length) {
        clearRxFifo();
        uint16_t i;
        bool isChasing = false;  //indicates whether the readPos is chasing the writePos (i.e. is behind it) as we have a ring buffer...
        if (rxReadPos <= rxWritePos) {
            isChasing = true;
        }
        bool isEscaped = false;
        for (i = 0; (!isChasing || rxReadPos < rxWritePos) && i < length; rxReadPos++, i++) {
            if (rxReadPos == GeneralHalSetup::UartBufferRxSize) {
                rxReadPos = 0;
                isChasing = true;
            }
            if (isEscaped) {
                isEscaped = false;
            } else if (rxBuffer[rxReadPos] == GeneralHalSetup::uartEscapingChar) {
                isEscaped = true;
                i--;
            }
        }
        return i;
    }

    bool write(const uint8_t *buffer, uint16_t length);

    bool canWrite(uint16_t length) {
        //can write if not transmitting, or if we can fit it in after we are done...
        return !txDma->isLocked()
                || (!txContinues && (txLength + length < GeneralHalSetup::UartBufferTxSize || length < txLength - txDma->fetchRemainingTransferLength() - 2));
    }

    uint8_t* getWriteLocation(uint16_t length, uint8_t firstByte) { //first byte needed to ensure atomicity... returns the buffer at the location where first byte should go
        //you probably want to get on with it after calling this method, the DMA might be running...
        if (length == 0) { // otherwise things could break a bit...
            return txBuffer;
        }
        if (!txDma->lock()) { // if DMA is not locked, we go right ahead, otherwise, we need to ensure DMA-perceived atomicity
            txDma->halt();
            if (txDma->isLocked()) { //just to be sure...
                if (!txContinues && txLength + length < GeneralHalSetup::UartBufferTxSize) {
                    txBuffer[txLength] = firstByte; // ensure that at least 1 byte is in buffer, so we don't run out of data - just in case
                    txDma->setRemainingTransferLength(txDma->fetchRemainingTransferLength() + length);
                    txLength += length;
                    txDma->resume(); //we can afford the DMA to continue now as we have some data in place, and it can't terminate by accident
                    return txBuffer + txLength;
                } else if (!txContinues && length < txLength - txDma->fetchRemainingTransferLength() - 1) {
                    txBuffer[0] = firstByte; // ensure that at least 1 byte is in buffer, so we don't run out of data - just in case

                    txContinues = true;
                    txLength = length; // atomicity again...
                    txDma->resume(); //we can afford the DMA to continue now as we have some data in place, and it can't terminate by accident
                    return txBuffer;
                } else {
                    txDma->resume();
                    return NULL;
                }
            }
            txDma->resume();
        }
        if (length >= GeneralHalSetup::UartBufferTxSize) {
            return NULL;
        }
        txBuffer[0] = firstByte;
        txLength = length;
        return txBuffer;
    }

    void startWrite();

}
;
#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_ */

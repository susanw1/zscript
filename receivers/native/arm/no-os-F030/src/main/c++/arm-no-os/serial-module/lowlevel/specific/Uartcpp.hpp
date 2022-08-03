/*
 * Uartcpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTCPP_HPP_

#include "../Uart.hpp"
#include "../UartManager.hpp"
#include <arm-no-os/system/clock/ClockManager.hpp>

template<class LL>
void UartDmaCallback(Dma<LL> *dma, DmaTerminationStatus status) {
    for (int i = 0; i < LL::HW::uartCount; ++i) {
        if (((Uart<LL>*) UartManager<LL>::getUartById(i))->txDma == dma) {
            ((Uart<LL>*) UartManager<LL>::getUartById(i))->dmaInterrupt(status);
            break;
        }
    }
}

template<class LL>
void UartTxOverflowCallback(SerialIdentifier id) {
    ((Uart<LL>*) UartManager<LL>::getUartById(id))->txOverflowInterrupt();
}

template<class LL>
void Uart<LL>::transmitWriteBuffer() {
    if (txDma->lock()) {
        dmaStartDist = txBuffer.getLinearReadLength();
        if (dmaStartDist != 0) {
            txDma->peripheralWrite(txBuffer.getCurrentLinearRead(), true, (uint8_t*) uart.getTransmitRegister(), false,
                    dmaStartDist, false, Medium, &UartDmaCallback, false);
        } else {
            txDma->unlock();
        }
    }
}

template<class LL>
void Uart<LL>::clearRxFifo() {
    bool fitted = true; // use boolean to avoid atomicity issues...
    while (uart.hasRxFifoData()) {
        if (!rxBuffer.canWrite(4)) { //this ensures that we can always write the current byte + 2 more bytes (for overflow marking)
            fitted = false;
            break;
        }
        int16_t datum = uart.read();
        if (datum < 0) {
            if (datum != -SerialFramingError) {
                rxBuffer.write(LL::uartEscapingChar);
                rxBuffer.write((uint8_t) -datum);
            }
        } else if (datum == LL::uartEscapingChar) {
            rxBuffer.write(LL::uartEscapingChar);
            rxBuffer.write(LL::uartEscapingChar);
            if (targetValueCallback != NULL && targetValue == LL::uartEscapingChar) {
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
            rxBuffer.write(LL::uartEscapingChar);
            rxBuffer.write(SerialOverflowError);
        }
        if (rxOverflowCallback != NULL) {
            rxOverflowCallback(uart.getId());
        }
        while (uart.hasRxFifoData()) { //flush remaining data, as we can't fit it in...
            uart.read();
        }
    }
}

template<class LL>
SerialError Uart<LL>::getError(uint16_t length) {
    clearRxFifo();
    peekDist = 0;
    rxBuffer.resetPeek();
    for (int16_t i = 0; i < length; ++i) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            break;
        } else if (val == LL::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val != LL::uartEscapingChar) {
                rxBuffer.resetPeek();
                return (SerialError) val;
            }
        }
    }
    rxBuffer.resetPeek();
    return SerialNoError;
}

template<class LL>
uint16_t Uart<LL>::read(uint8_t *buffer, uint16_t length) {
    clearRxFifo();
    peekDist = 0;
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.read();
        if (val == -1) {
            break;
        } else if (val == LL::uartEscapingChar) {
            val = rxBuffer.read();
            if (val == LL::uartEscapingChar) {
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

template<class LL>
int16_t Uart<LL>::read() {
    clearRxFifo();
    peekDist = 0;
    while (true) {
        int16_t val = rxBuffer.read();
        if (val == LL::uartEscapingChar) {
            val = rxBuffer.read();
            if (val == LL::uartEscapingChar) {
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

template<class LL>
int32_t Uart<LL>::getDistance(uint8_t value) {
    clearRxFifo();
    rxBuffer.resetPeek();
    uint16_t i = 0;
    while (true) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            rxBuffer.resetPeek();
            return -1;
        } else if (val == LL::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == LL::uartEscapingChar) {
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

template<class LL>
uint16_t Uart<LL>::skip(uint16_t length) { // there is no efficient skip, as we need to exclude escaped characters
    clearRxFifo();
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.read();
        if (val == -1) {
            break;
        } else if (val == LL::uartEscapingChar) {
            val = rxBuffer.read();
            if (val != LL::uartEscapingChar) {
                i--;
            }
        }
    }
    availableData -= i;
    peekDist = 0;
    return i;
}

template<class LL>
uint16_t Uart<LL>::availablePeek() {
    clearRxFifo();
    return availableData - peekDist;
}

template<class LL>
uint16_t Uart<LL>::peek(uint8_t *buffer, uint16_t length) {
    clearRxFifo();
    uint16_t i;
    for (i = 0; i < length; ++i) {
        int16_t val = rxBuffer.peek();
        if (val == -1) {
            break;
        } else if (val == LL::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == LL::uartEscapingChar) {
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

template<class LL>
int16_t Uart<LL>::peek() {
    clearRxFifo();
    while (true) {
        int16_t val = rxBuffer.peek();
        if (val == LL::uartEscapingChar) {
            val = rxBuffer.peek();
            if (val == LL::uartEscapingChar) {
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

template<class LL>
void Uart<LL>::resetPeek() {
    clearRxFifo();
    peekDist = 0;
    rxBuffer.resetPeek();
}

template<class LL>
void Uart<LL>::skipToPeek() {
    clearRxFifo();
    availableData -= peekDist;
    peekDist = 0;
    rxBuffer.skipToPeek();
}

template<class LL>
void Uart<LL>::txOverflowInterrupt() {
    transmitWriteBuffer(); // in case somebody forgot, as that would do it...
    uint16_t remainingLength = txDma->fetchRemainingTransferLength();
    txBuffer.skip(dmaStartDist - remainingLength); // move along as much as the DMA has allowed in the time since we last did that
    dmaStartDist -= remainingLength;
}

template<class LL>
void Uart<LL>::dmaInterrupt(DmaTerminationStatus status) {
    //FIXME: Do something with interesting failure states...
    (void) status;
    txBuffer.skip(dmaStartDist);
    dmaStartDist = txBuffer.getLinearReadLength();
    txDma->halt();
    if (dmaStartDist == 0) {
        txDma->unlock();
    } else {
        txDma->peripheralWrite(txBuffer.getCurrentLinearRead(), true, (uint8_t*) uart.getTransmitRegister(), false,
                dmaStartDist, false, Medium, &UartDmaCallback<LL>, false);
    }
}

template<class LL>
void Uart<LL>::init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop) {
// buffer overflow callback can be NULL
    this->rxOverflowCallback = bufferOverflowCallback;
    this->txBuffer.setCallback(&UartTxOverflowCallback<LL>, uart.getId());
    uart.init(baud_rate, singleNdoubleStop);
}

template<class LL>
void Uart<LL>::interrupt() {
    clearRxFifo();
    uart.clearFlags();
}

#endif /* SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTCPP_HPP_ */

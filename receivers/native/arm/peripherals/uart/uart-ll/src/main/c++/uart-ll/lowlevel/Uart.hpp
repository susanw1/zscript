/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UART_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UART_HPP_

#include <uart-ll/UartLLInterfaceInclude.hpp>

#include <llIncludes.hpp>
#include "specific/UartInternal.hpp"
#include <dma-ll/DmaManager.hpp>
#include <serial-ll/lowlevel/AbstractRingBufferSerial.hpp>
#include <serial-ll/lowlevel/SerialRingBuffer.hpp>

template<class LL>
class UartManager;

template<class LL>
void UartDmaCallback(Dma<LL> *dma, DmaTerminationStatus status);

template<class LL>
void UartTxOverflowCallback(SerialIdentifier id);

//always does 8 bit bytes, no parity - just for simplicity
template<class LL>
class Uart: public Serial {
    //TODO: Use a dma for Rx as well?
private:
    void (*volatile targetValueCallback)(SerialIdentifier);
    void (*rxOverflowCallback)(SerialIdentifier);
    SerialRingBuffer<LL::UartBufferRxSize> rxBuffer;
    SerialRingBuffer<LL::UartBufferTxSize> txBuffer;
    UartInternal<LL> uart;

#ifdef UART_TX_DMA
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    Dma<LL> *txDma = NULL;
    uint16_t dmaTxStartDist = 0;
#endif
#ifdef UART_RX_DMA
    DmaMuxRequest requestRx = DMAMUX_NO_MUX;
    Dma<LL> *rxDma = NULL;
#endif
    uint16_t availableData = 0;
    uint16_t peekDist = 0;
    uint8_t targetValue = 0;

#ifdef UART_TX_DMA
    friend void UartDmaCallback<LL>(Dma<LL>*, DmaTerminationStatus);
#endif

    friend void UartTxOverflowCallback<LL>(SerialIdentifier); //for the tx buffer...
    friend UartManager<LL> ;

    void rxDataArrivedInterrupt();

    void dmaInterrupt(DmaTerminationStatus status);

    void txOverflowInterrupt();

public:

    Uart(UartInternal<LL> uart, Dma<LL> *txDma) :
            id(uart.getId()), targetValueCallback(NULL), rxOverflowCallback(NULL), uart(uart), requestTx(DMAMUX_NO_MUX), txDma(txDma) {
    }

    Uart(UartInternal<LL> uart, Dma<LL> *txDma, DmaMuxRequest requestTx) :
            id(uart.getId()), targetValueCallback(NULL), rxOverflowCallback(NULL), uart(uart), requestTx(requestTx), txDma(txDma) {
    }

    uint32_t getMaxBaud() {
        return ClockManager<LL>::getClock(SysClock)->getFreqKhz() * 1000 / 16;
    }
    uint32_t getMinBaud() {
        return 0;
    }
    void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop);
    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write

    void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue) {
        this->targetValueCallback = targetValueCallback;
        this->targetValue = targetValue;
    }

    void clearTargetValue() {
        this->targetValueCallback = NULL;
    }

    bool write(uint8_t datum) {
        return txBuffer.write(datum);
    }

    bool write(const uint8_t *buffer, uint16_t length) {
        return txBuffer.write(buffer, length);
    }

    bool canWrite(uint16_t length) {
        return txBuffer.canWrite(length);
    }

    bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
        return txBuffer.attemptWrite(buffer, length);
    }

    bool attemptWrite(uint8_t datum) {
        return txBuffer.attemptWrite(datum);
    }

    bool canAttemptWrite(uint16_t length) {
        return txBuffer.canAttemptWrite(length);
    }

    void cancelWrite() { //undoes an attempted write
        txBuffer.cancelWrite();
    }

    void completeWrite() { //allows reading of any attempted write
        txBuffer.completeWrite();
    }

    void transmitWriteBuffer();

    void clearRxFifo();

    SerialEvent getEvent(uint16_t length);

    uint16_t skip(uint16_t length);

    uint16_t available() {
        clearRxFifo();
        return availableData;
    }

    SerialReadResult read(uint8_t *buffer, uint16_t length);

    SerialSingleResult read(); //-1 if no data

    uint16_t availablePeek();

    SerialReadResult peek(uint8_t *buffer, uint16_t length);

    SerialSingleResult peek(); //-1 if no data

    void resetPeek();

    void skipToPeek();

    int32_t getDistance(uint8_t value);  //returns the number of bytes until the specified value appears, including the value
};
template<class LL>
void Uart<LL>::rxDataArrivedInterrupt() {
    clearRxFifo();
    uart.clearFlags();
}
#include "specific/Uartcpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_UART_HPP_ */

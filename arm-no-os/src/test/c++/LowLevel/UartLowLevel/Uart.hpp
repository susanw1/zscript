/*
 * Uart.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#include "specific/UartInternal.hpp"
#include "UartRingBuffer.hpp"

#include "../DmaLowLevel/Dma.hpp"
#include "../DmaLowLevel/DmaManager.hpp"
#include "../GeneralLLSetup.hpp"

enum UartError {
    UartNoError = 0,
    UartParityError = 1,
    UartNoiseError = 2,
    UartOverflowError = 3
};

class UartManager;
class UartInterruptManager;
//always does 8 bit bytes, no parity - just for simplicity
class Uart {
private:
    void (*volatile targetValueCallback)(Uart*);
    UartRingBuffer<GeneralHalSetup::UartBufferRxSize> rxBuffer;
    UartRingBuffer<GeneralHalSetup::UartBufferTxSize> txBuffer;
    UartInternal uart;
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    Dma *txDma = NULL;
    uint16_t dmaStartDist = 0;
    uint16_t availableData = 0;
    uint8_t targetValue = 0;

    friend void UartDmaCallback(Dma*, DmaTerminationStatus);
    friend void UartTxOverflowCallback(UartIdentifier);
    friend UartManager;
    friend UartInterruptManager;

    void interrupt();

    void dmaInterrupt(DmaTerminationStatus status);

    void txOverflowInterrupt();

public:
    void setUart(UartInternal uart, Dma *txDma, DmaMuxRequest requestTx) {
        this->uart = uart;
        this->txDma = txDma;
        this->requestTx = requestTx;
    }
    void init(void (*volatile bufferOverflowCallback)(UartIdentifier), uint32_t baud_rate, bool singleNdoubleStop);
    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write

    void setTargetValue(void (*volatile targetValueCallback)(Uart*), uint8_t targetValue) {
        this->targetValueCallback = targetValueCallback;
        this->targetValue = targetValue;
    }
    void clearTargetValue() {
        this->targetValueCallback = NULL;
    }
    UartIdentifier getId() {
        return uart.getId();
    }
    UartRingBuffer<GeneralHalSetup::UartBufferTxSize>* getTxBuffer() {
        return &txBuffer;
    }
    void transmitWriteBuffer();

    void clearRxFifo();

    UartError getError(uint16_t length);

    uint16_t skip(uint16_t length);
    uint16_t available() {
        clearRxFifo();
        return availableData;
    }
    uint16_t read(uint8_t *buffer, uint16_t length);
    int16_t read(); //-1 if no data

    int32_t getDistance(uint8_t value);  //returns the number of bytes until the specified value appears, including the value

};
#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_ */

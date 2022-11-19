/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDLOWLEVEL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDLOWLEVEL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>
#include <arm-no-os/system/dma/DmaManager.hpp>
#include <arm-no-os/system/interrupt/InterruptManager.hpp>

enum UcpdTxTerminationStatus {
    UcpdTxAborted, UcpdTxMessageDiscarded, UcpdTxDmaFail, UcpdMessageSent
};

enum UcpdRxReceiveStatus {
    UcpdRxMessageDiscarded, UcpdRxDmaFail, UcpdRxMessageReceived
};

template<class LL>
class UcpdLowLevel {
    typedef typename LL::HW HW;
    static UcpdLowLevel ucpdBuffers;
    uint8_t rxBuffer[LL::ucpdRxBufferSize];
    uint8_t txBuffer[LL::ucpdTxBufferSize];
    static uint16_t rxLength;
    static Dma<LL> *rxDma;
    static void (*rxCallback)(UcpdRxReceiveStatus);
    static Dma<LL> *txDma;
    static void (*txCallback)(UcpdTxTerminationStatus);

    static void (*hardResetCallback)();

    static void cableOrientationDetect() {
        UCPD1->IMR &= ~0xC000;
        if (UCPD1->SR & 0x30000) {
            if (UCPD1->CR & 0x40) {
                UCPD1->CR &= ~0x40;
            }
        } else if (UCPD1->SR & 0xC0000) {
            UCPD1->CR |= 0x40;
        } else {
            UCPD1->IMR |= 0xC000;   // enable interrupts for a cable event.
        }
    }

    static void interrupt(uint8_t id) {
        (void) id;
        if (UCPD1->SR & 0x0420) {
            UCPD1->ICR |= 0x0420;
            hardResetCallback();
        } else if (UCPD1->SR & 0x2000) {
            resetRxDma();
            UCPD1->ICR |= 0x3000;
            rxCallback(UcpdRxMessageDiscarded);
        } else if (UCPD1->SR & 0x0008) {
            resetTxDma();
            UCPD1->ICR |= 0x000C;
            txCallback(UcpdTxAborted);
        } else if (UCPD1->SR & 0x0002) {
            resetTxDma();
            UCPD1->ICR |= 0x0006;
            txCallback(UcpdTxMessageDiscarded);
        } else if (UCPD1->SR & 0x0800) {
            resetRxDma();
            UCPD1->ICR |= 0x800;
            rxCallback(UcpdRxDmaFail);
        } else if (UCPD1->SR & 0x0040) {
            resetTxDma();
            UCPD1->ICR |= 0x0004;
            txCallback(UcpdTxDmaFail);
        } else if (UCPD1->SR & 0xC000) {
            UCPD1->ICR |= 0xC000;
            cableOrientationDetect();
        } else if (UCPD1->SR & 0x1000) {
            rxLength = UCPD1->RX_PAYSZ;
            resetRxDma();
            UCPD1->ICR |= 0x1000;
            rxCallback(UcpdRxMessageReceived);
        } else if (UCPD1->SR & 0x0004) {
            UCPD1->ICR |= 0x0004;
            resetTxDma();
            txCallback(UcpdMessageSent);
        }
    }
    static void resetRxDma() {
        rxDma->halt();
        rxDma->peripheralRead(DMAMUX_UCPD_RX, (uint8_t*) &UCPD1->RXDR, false, ucpdBuffers.rxBuffer, true, LL::ucpdRxBufferSize, false, High,
                &UcpdLowLevel::DmaRxCallback, false);
    }
    static void resetTxDma() {
        txDma->halt();
    }

    static void DmaRxCallback(Dma<LL> *d, DmaTerminationStatus status) {
        (void) d;
        if (status != DataTransferComplete) {
            rxCallback(UcpdRxDmaFail);
        }
    }
    static void DmaTxCallback(Dma<LL> *d, DmaTerminationStatus status) {
        (void) d;
        if (status != DataTransferComplete) {
            txCallback(UcpdTxDmaFail);
        }
    }

public:
    static void init() {
        RCC->APB1ENR2 |= 0x0100;
        rxDma = DmaManager<LL>::getDmaById(LL::ucpdRxDma);
        txDma = DmaManager<LL>::getDmaById(LL::ucpdTxDma);
        UCPD1->CFG1 = 0x60900000 | (0x00 << 17) | (0x09 << 11) | (0x0E << 6) | (0x1A); //Set recommended clock settings, do not enable yet
        UCPD1->CFG1 |= 0x80000000; // enable
        UCPD1->IMR = 0x1C2E; // enable RX complete, rx hard reset, tx complete/aborted/discarded
        UCPD1->CR = 0x0E00; // use 0x0E00 to do pd
        SystemMilliClock<LL>::blockDelayMillis(10);
        PWR->CR2 |= 0x4000; // disable dead-battery pull downs - hope the peripheral is running, or we lose power now!
        cableOrientationDetect();
        UCPD1->CR |= 0x20;
        InterruptManager::enableInterrupt(UcpdInt, 0, 2);
        InterruptManager::setInterrupt(&UcpdLowLevel<LL>::interrupt, UcpdInt);
        rxDma->lock();
        txDma->lock();
        rxDma->peripheralRead(DMAMUX_UCPD_RX, (uint8_t*) &UCPD1->RXDR, false, ucpdBuffers.rxBuffer, true, LL::ucpdRxBufferSize, false, High,
                &UcpdLowLevel::DmaRxCallback, false);
        PWR->CR2 |= 0x4000; // disable dead-battery pull downs - hope the peripheral is running, or we lose power now!
    }

    static uint8_t* getTxBuffer() {
        return ucpdBuffers.txBuffer;
    }

    static const uint8_t* getRxBuffer() {
        return ucpdBuffers.rxBuffer;
    }
    static uint16_t getRxLength() {
        return rxLength;
    }

    static void hardReset() {
        UCPD1->CR |= 0x8;
    }

    static void setRxCallback(void (*rxCallback)(UcpdRxReceiveStatus)) {
        UcpdLowLevel::rxCallback = rxCallback;
    }

    static void setHardResetCallback(void (*hardResetCallback)()) {
        UcpdLowLevel::hardResetCallback = hardResetCallback;
    }

    static void sendTxBuffer(int length, void (*txCallback)(UcpdTxTerminationStatus)) {
        sendBuffer(ucpdBuffers.txBuffer, length, txCallback);
    }

    static void sendBuffer(uint8_t *buf, uint8_t length, void (*txCallback)(UcpdTxTerminationStatus)) {
        UcpdLowLevel::txCallback = txCallback;
        UCPD1->TX_ORDSET = HW::ucpdTxSOP;
        UCPD1->TX_PAYSZ = length;
        txDma->peripheralWrite(DMAMUX_UCPD_TX, buf, true, (uint8_t*) &UCPD1->TXDR, false, length, false, High, &UcpdLowLevel::DmaTxCallback, false);
        UCPD1->CR |= 0x4;
    }
};
template<class LL>
UcpdLowLevel<LL> UcpdLowLevel<LL>::ucpdBuffers;
template<class LL>
uint16_t UcpdLowLevel<LL>::rxLength = 0;
template<class LL>
Dma<LL> *UcpdLowLevel<LL>::rxDma = NULL;
template<class LL>
void (*UcpdLowLevel<LL>::rxCallback)(UcpdRxReceiveStatus)= NULL;
template<class LL>
Dma<LL> *UcpdLowLevel<LL>::txDma = NULL;
template<class LL>
void (*UcpdLowLevel<LL>::txCallback)(UcpdTxTerminationStatus)= NULL;

template<class LL>
void (*UcpdLowLevel<LL>::hardResetCallback)()= NULL;

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDLOWLEVEL_HPP_ */

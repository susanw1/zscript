/*
 * DmaManager.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../../DmaLowLevel/DmaManager.hpp"
#include "stm32g484xx.h"
#include "core_cm4.h"
Dma DmaManager::dmas[] = { Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma() };

class DmaInterruptManager {
    friend DmaManager;
    static void IRQDMA1CH1() {
        DmaManager::dmas[0].interrupt();
    }
    static void IRQDMA1CH2() {
        DmaManager::dmas[1].interrupt();
    }
    static void IRQDMA1CH3() {
        DmaManager::dmas[2].interrupt();
    }
    static void IRQDMA1CH4() {
        DmaManager::dmas[3].interrupt();
    }
    static void IRQDMA1CH5() {
        DmaManager::dmas[4].interrupt();
    }
    static void IRQDMA1CH6() {
        DmaManager::dmas[5].interrupt();
    }
    static void IRQDMA1CH7() {
        DmaManager::dmas[6].interrupt();
    }
    static void IRQDMA1CH8() {
        DmaManager::dmas[7].interrupt();
    }

    static void IRQDMA2CH1() {
        DmaManager::dmas[8].interrupt();
    }
    static void IRQDMA2CH2() {
        DmaManager::dmas[9].interrupt();
    }
    static void IRQDMA2CH3() {
        DmaManager::dmas[10].interrupt();
    }
    static void IRQDMA2CH4() {
        DmaManager::dmas[11].interrupt();
    }
    static void IRQDMA2CH5() {
        DmaManager::dmas[12].interrupt();
    }
    static void IRQDMA2CH6() {
        DmaManager::dmas[13].interrupt();
    }
    static void IRQDMA2CH7() {
        DmaManager::dmas[14].interrupt();
    }
    static void IRQDMA2CH8() {
        DmaManager::dmas[15].interrupt();
    }
};
DmaChannelRegisters* getDmaChannelRegistersFromRegisters(DmaRegisters *r, uint8_t channel) {
    if (channel == 0) {
        return &r->CHR1;
    } else if (channel == 1) {
        return &r->CHR2;
    } else if (channel == 2) {
        return &r->CHR3;
    } else if (channel == 3) {
        return &r->CHR4;
    } else if (channel == 4) {
        return &r->CHR5;
    } else if (channel == 5) {
        return &r->CHR6;
    } else if (channel == 6) {
        return &r->CHR7;
    } else {
        return &r->CHR8;
    }
}
DmaChannelInternal createChannelInternalFromId(DmaIdentifier id) {
    if (id < 8) {
        DmaRegisters *registers = (DmaRegisters*) 0x50000000;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters(registers, id), id);
    } else {
        DmaRegisters *registers = (DmaRegisters*) 0x50000000;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters(registers, id - 8), id - 8);
    }
}
void DmaManager::init() {
    //__HAL_RCC_DMA1_CLK_ENABLE();
    //__HAL_RCC_DMA2_CLK_ENABLE();

    for (int i = 0; i < GeneralHalSetup::dmaCount; ++i) {
        dmas[i].setDma(createChannelInternalFromId(i));
    }

    NVIC_SetVector(DMA1_Channel1_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH1);
    NVIC_SetVector(DMA1_Channel2_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH2);
    NVIC_SetVector(DMA1_Channel3_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH3);
    NVIC_SetVector(DMA1_Channel4_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH4);
    NVIC_SetVector(DMA1_Channel5_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH5);
    NVIC_SetVector(DMA1_Channel6_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH6);
    NVIC_SetVector(DMA1_Channel7_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH7);
    NVIC_SetVector(DMA1_Channel8_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA1CH8);

    NVIC_SetVector(DMA2_Channel1_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH1);
    NVIC_SetVector(DMA2_Channel2_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH2);
    NVIC_SetVector(DMA2_Channel3_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH3);
    NVIC_SetVector(DMA2_Channel4_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH4);
    NVIC_SetVector(DMA2_Channel5_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH5);
    NVIC_SetVector(DMA2_Channel6_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH6);
    NVIC_SetVector(DMA2_Channel7_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH7);
    NVIC_SetVector(DMA2_Channel8_IRQn, (uint32_t) & DmaInterruptManager::IRQDMA2CH8);
}


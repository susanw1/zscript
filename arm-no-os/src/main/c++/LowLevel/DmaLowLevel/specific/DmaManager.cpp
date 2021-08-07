/*
 * DmaManager.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../DmaManager.hpp"
#include "stm32g484xx.h"
#include "core_cm4.h"
Dma DmaManager::dmas[] = { Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma(), Dma() };

class DmaInterruptManager {
    friend void DMA1_Channel1_IRQHandler();
    friend void DMA1_Channel2_IRQHandler();
    friend void DMA1_Channel3_IRQHandler();
    friend void DMA1_Channel4_IRQHandler();
    friend void DMA1_Channel5_IRQHandler();
    friend void DMA1_Channel6_IRQHandler();
    friend void DMA1_Channel7_IRQHandler();
    friend void DMA1_Channel8_IRQHandler();

    friend void DMA2_Channel1_IRQHandler();
    friend void DMA2_Channel2_IRQHandler();
    friend void DMA2_Channel3_IRQHandler();
    friend void DMA2_Channel4_IRQHandler();
    friend void DMA2_Channel5_IRQHandler();
    friend void DMA2_Channel6_IRQHandler();
    friend void DMA2_Channel7_IRQHandler();
    friend void DMA2_Channel8_IRQHandler();

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
void DMA1_Channel1_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH1();
}
void DMA1_Channel2_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH2();
}
void DMA1_Channel3_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH3();
}
void DMA1_Channel4_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH4();
}
void DMA1_Channel5_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH5();
}
void DMA1_Channel6_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH6();
}
void DMA1_Channel7_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH7();
}
void DMA1_Channel8_IRQHandler() {
    DmaInterruptManager::IRQDMA1CH8();
}

void DMA2_Channel1_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH1();
}
void DMA2_Channel2_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH2();
}
void DMA2_Channel3_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH3();
}
void DMA2_Channel4_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH4();
}
void DMA2_Channel5_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH5();
}
void DMA2_Channel6_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH6();
}
void DMA2_Channel7_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH7();
}
void DMA2_Channel8_IRQHandler() {
    DmaInterruptManager::IRQDMA2CH8();
}

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
        DmaRegisters *registers = (DmaRegisters*) 0x40020000;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters(registers, id),
                (uint32_t*) (0x40020800 + 0x04 * id), id);
    } else {
        DmaRegisters *registers = (DmaRegisters*) 0x40020400;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters(registers, id - 8),
                (uint32_t*) (0x40020800 + 0x04 * id), id - 8);
    }
}
void DmaManager::init() {
    const uint32_t enableDma1Registers = 0x01;
    const uint32_t enableDma2Registers = 0x02;
    const uint32_t enableDmaMuxRegisters = 0x04;

    RCC->AHB1ENR |= enableDma1Registers | enableDma2Registers | enableDmaMuxRegisters;

    for (int i = 0; i < GeneralHalSetup::dmaCount; ++i) {
        dmas[i].setDma(createChannelInternalFromId(i));
    }
    NVIC_SetPriority(DMA1_Channel1_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    NVIC_SetPriority(DMA1_Channel2_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);

    NVIC_SetPriority(DMA1_Channel3_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    NVIC_SetPriority(DMA1_Channel4_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    NVIC_SetPriority(DMA1_Channel5_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    NVIC_SetPriority(DMA1_Channel6_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    NVIC_SetPriority(DMA1_Channel7_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    NVIC_SetPriority(DMA1_Channel8_IRQn, 10);
    NVIC_EnableIRQ(DMA1_Channel8_IRQn);

    NVIC_SetPriority(DMA2_Channel1_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel1_IRQn);

    NVIC_SetPriority(DMA2_Channel2_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel2_IRQn);

    NVIC_SetPriority(DMA2_Channel3_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel3_IRQn);

    NVIC_SetPriority(DMA2_Channel4_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel4_IRQn);

    NVIC_SetPriority(DMA2_Channel5_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel5_IRQn);

    NVIC_SetPriority(DMA2_Channel6_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel6_IRQn);

    NVIC_SetPriority(DMA2_Channel7_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel7_IRQn);

    NVIC_SetPriority(DMA2_Channel8_IRQn, 10);
    NVIC_EnableIRQ(DMA2_Channel8_IRQn);
}


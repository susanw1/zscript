/*
 * DmaManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_DMALOWLEVEL_SPECIFIC_DMAMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_DMALOWLEVEL_SPECIFIC_DMAMANAGERCPP_HPP_

#include "../DmaManager.hpp"
#include <arm-no-os/system/interrupt/InterruptManager.hpp>

template<class LL>
Dma<LL> DmaManager<LL>::dmas[] = { Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(),
        Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>() };

template<class LL>
void DmaManager<LL>::interrupt(uint8_t i) {
    DmaManager::dmas[i].interrupt();
}

template<class LL>
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

template<class LL>
DmaChannelInternal createChannelInternalFromId(DmaIdentifier id) {
    if (id < 8) {
        DmaRegisters *registers = (DmaRegisters*) 0x40020000;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters<LL>(registers, id),
                (uint32_t*) (0x40020800 + 0x04 * id), id);
    } else {
        DmaRegisters *registers = (DmaRegisters*) 0x40020400;
        return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters<LL>(registers, id - 8),
                (uint32_t*) (0x40020800 + 0x04 * id), id - 8);
    }
}

template<class LL>
void DmaManager<LL>::init() {
    const uint32_t enableDma1Registers = 0x01;
    const uint32_t enableDma2Registers = 0x02;
    const uint32_t enableDmaMuxRegisters = 0x04;

    RCC->AHB1ENR |= enableDma1Registers | enableDma2Registers | enableDmaMuxRegisters;

    InterruptManager::setInterrupt(&DmaManager::interrupt, InterruptType::DmaInt);
    for (int i = 0; i < LL::dmaCount; ++i) {
        dmas[i].setDma(createChannelInternalFromId<LL>(i));
        InterruptManager::enableInterrupt(InterruptType::DmaInt, i, 10);
    }
}

#endif /* SRC_MAIN_C___LOWLEVEL_DMALOWLEVEL_SPECIFIC_DMAMANAGERCPP_HPP_ */

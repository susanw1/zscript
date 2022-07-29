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
Dma<LL> DmaManager<LL>::dmas[] = { Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>(), Dma<LL>() };

template<class LL>
void DmaManager<LL>::interrupt(uint8_t i) {
    DmaRegisters *registers = (DmaRegisters*) 0x40020000;
    if (i == 0) {
        DmaManager::dmas[0].interrupt();
    } else if (i == 1) {
        if ((registers->ISR & 0xF0) != 0) {
            DmaManager::dmas[1].interrupt();
        } else {
            DmaManager::dmas[2].interrupt();
        }
    } else {
        if ((registers->ISR & 0xF000) != 0) {
            DmaManager::dmas[3].interrupt();
        } else {
            DmaManager::dmas[4].interrupt();
        }
    }
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
    } else {
        return &r->CHR5;
    }
}

template<class LL>
DmaChannelInternal createChannelInternalFromId(DmaIdentifier id) {
    DmaRegisters *registers = (DmaRegisters*) 0x40020000;
    return DmaChannelInternal((DmaRegisters*) registers, getDmaChannelRegistersFromRegisters<LL>(registers, id), id);
}

template<class LL>
void DmaManager<LL>::init() {
    const uint32_t enableDma1Registers = 0x01;

    RCC->AHBENR |= enableDma1Registers;

    InterruptManager::setInterrupt(&DmaManager::interrupt, InterruptType::DmaInt);
    InterruptManager::enableInterrupt(InterruptType::DmaInt, 0, 10);
    InterruptManager::enableInterrupt(InterruptType::DmaInt, 1, 10);
    InterruptManager::enableInterrupt(InterruptType::DmaInt, 2, 10);
    for (int i = 0; i < LL::dmaCount; ++i) {
        dmas[i].setDma(createChannelInternalFromId<LL>(i));
    }
}

#endif /* SRC_MAIN_C___LOWLEVEL_DMALOWLEVEL_SPECIFIC_DMAMANAGERCPP_HPP_ */

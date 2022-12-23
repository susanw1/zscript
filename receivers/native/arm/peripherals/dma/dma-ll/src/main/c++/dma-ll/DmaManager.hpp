/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMAMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMAMANAGER_HPP_

#include <llIncludes.hpp>
#include "Dma.hpp"

template<class LL>
class DmaManager {
private:
    typedef typename LL::HW HW;
    static Dma<LL> dmas[LL::HW::dmaCount];

    DmaManager() {
    }
    static void interrupt(uint8_t);

public:
    static void init();

    static Dma<LL>* getDmaById(DmaIdentifier id) {
        return dmas + id;
    }
};
#include "specific/DmaManagercpp.hpp" // these at the bottom are there to clean up include order issues.

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMAMANAGER_HPP_ */

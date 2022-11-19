/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_ZCODESYSTEMMODULE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_ZCODESYSTEMMODULE_HPP_

#include "clock/SystemMilliClock.hpp"
#include "clock/ClockManager.hpp"
#include "dma/DmaManager.hpp"

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>

template<class ZP>
class ZcodeSystemModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;

public:
    typedef SystemMilliClock<LL> milliClock;
    static void initClocks() {
        ClockManager<LL>::basicSetup();
    }
    static void initClocksFast() {
        ClockManager<LL>::basicSetup();
    }
    static void initClocksSlow() {
        ClockManager<LL>::basicSetup();
    }

    static void init() {

        SystemMilliClock<LL>::init();
        for (volatile uint32_t i = 0; i < 0x100; ++i)
            ;
        SystemMilliClock<LL>::blockDelayMillis(5);
        DmaManager<LL>::init();
    }

    static Dma<LL>* getDma(DmaIdentifier id) {
        return DmaManager<LL>::getDmaById(id);
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_ZCODESYSTEMMODULE_HPP_ */

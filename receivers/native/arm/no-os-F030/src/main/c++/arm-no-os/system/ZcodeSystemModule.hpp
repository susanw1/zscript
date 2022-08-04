/*
 * ZcodeSystemModule.hpp
 *
 *  Created on: 4 Aug 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARM_NO_OS_SYSTEM_ZCODESYSTEMMODULE_HPP_
#define SRC_MAIN_C___ARM_NO_OS_SYSTEM_ZCODESYSTEMMODULE_HPP_
#include <arm-no-os/system/core/stm32f0xx.h>
#include <arm-no-os/system/core/core_cm0.h>

#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>
#include <arm-no-os/system/dma/DmaManager.hpp>

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
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
        ClockManager<LL>::fastSetup();
    }
    static void initClocksSlow() {
        ClockManager<LL>::slowSetup();
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

#endif /* SRC_MAIN_C___ARM_NO_OS_SYSTEM_ZCODESYSTEMMODULE_HPP_ */

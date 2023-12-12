/*
 * SystemMilliClockcpp.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_CLOCK_CLOCK_LL_STM32F_SRC_MAIN_C___CLOCK_LL_SPECIFIC_SYSTEMMILLICLOCKCPP_HPP_
#define PERIPHERALS_CLOCK_CLOCK_LL_STM32F_SRC_MAIN_C___CLOCK_LL_SPECIFIC_SYSTEMMILLICLOCKCPP_HPP_

#include <clock-ll/SystemMilliClock.hpp>

template<class LL>
void SystemMilliClock<LL>::init() {
    const uint32_t enableSysTickEnable = 0x1;
    const uint32_t enableSysTickEnableInt = 0x2;
    const uint32_t enableSysTickSetAsClock = 0x4;
    *((uint32_t*) 0xE000E014) = ClockManager<LL>::getClock(SysClock)->getFreqKhz(); // LOAD
    *((uint32_t*) 0xE000E010) = enableSysTickEnable | enableSysTickEnableInt | enableSysTickSetAsClock; //CTRL

    InterruptManager::setInterrupt(&SystemMilliClock::resetHappened, SysTickInt);
    InterruptManager::enableInterrupt(SysTickInt, 0, 5);
    timeBroad = 0;
}

template<class LL>
void SystemMilliClock<LL>::blockDelayMillis(uint32_t delay) {
    uint32_t end = getTimeMillis() + delay;
    while (end > getTimeMillis())
        ;
    return;
}

template<class LL>
uint32_t SystemMilliClock<LL>::getTimeMillis() {
    return timeBroad;
}

#endif /* PERIPHERALS_CLOCK_CLOCK_LL_STM32F_SRC_MAIN_C___CLOCK_LL_SPECIFIC_SYSTEMMILLICLOCKCPP_HPP_ */

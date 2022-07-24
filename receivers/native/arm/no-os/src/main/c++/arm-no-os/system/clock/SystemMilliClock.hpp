/*
 * SystemMilliClock.hpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "ClockManager.hpp"
#include <arm-no-os/system/interrupt/InterruptManager.hpp>

template<class LL>
class SystemMilliClock {
private:
    static volatile uint32_t timeBroad;
    static uint8_t bitScaling;

    friend class InterruptManager;

    static void resetTimer(uint8_t i) {
        (void) i;
        TIM6->SR = 0;
        timeBroad++;
    }

public:
    static void init() {
        const uint32_t enableTim6Registers = 0x10;
        const uint32_t setUpdateRequestToOverUnderFlow = 0x4;
        const uint32_t enableCounter = 0x1;
        const uint32_t enableUpdateInterrupt = 0x1;
        const uint32_t counterStart = 0xFFFC;

        RCC->APB1ENR1 |= enableTim6Registers;
        bitScaling = 0;
        uint32_t scaler = 0x00FFFFFF; //something which is too large for the prescaler
        while (scaler > 0x10000) {
            scaler = ClockManager<LL>::getClock(PCLK_1)->getDivider(1 << bitScaling++);
        }
        InterruptManager::setInterrupt(&SystemMilliClock::resetTimer, TIM6_);
        InterruptManager::enableInterrupt(TIM6_, 0, 5);
        bitScaling--; // we will have over-shot
        TIM6->PSC = scaler - 1;
        TIM6->CNT = counterStart;
        TIM6->DIER = enableUpdateInterrupt;
        TIM6->CR1 = setUpdateRequestToOverUnderFlow | enableCounter;
        timeBroad = 0;
    }

    static void blockDelayMillis(uint32_t delay) {
        uint32_t end = getTimeMillis() + delay;
        while (end > getTimeMillis())
            ;
        return;
    }

    static uint32_t getTimeMillis() {
        uint32_t result;
        TIM6->DIER = 0;
        result = (timeBroad << (16 - bitScaling)) | ((TIM6->CNT) >> bitScaling);
        TIM6->DIER = 1;
        return result;
    }

    static uint64_t getTimeMillisBig() {
        uint64_t result;
        TIM6->DIER = 0;
        result = ((uint64_t) (timeBroad << (16 - bitScaling))) | ((uint64_t) ((TIM6->CNT) >> bitScaling));
        TIM6->DIER = 1;
        return result;
    }
};

template<class LL>
volatile uint32_t SystemMilliClock<LL>::timeBroad = 0;

template<class LL>
uint8_t SystemMilliClock<LL>::bitScaling = 0;

#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_ */

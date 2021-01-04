/*
 * SystemMilliClock.hpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_
#define SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_

#include "ClockManager.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void TIM6_DAC_IRQHandler();
#ifdef __cplusplus
}
#endif

class SystemMilliClock {
    static volatile uint32_t timeBroad;
    static uint8_t bitScaling;

    friend void TIM6_DAC_IRQHandler();

    static void resetTimer() {
        TIM6->SR = 0;
        TIM6->CNT = 0;
        timeBroad++;
    }
public:
    static void init() {
        RCC->APB1ENR1 |= 0x10;
        bitScaling = 0;
        uint32_t scaler = 0xFFFFFFFF;
        while (scaler > 0x10000) {
            scaler = ClockManager::getClock(PCLK_1)->getDivider(1 << bitScaling++);
        }
        bitScaling--; // we will have over-shot
        NVIC_SetPriority(TIM6_DAC_IRQn, 5);
        NVIC_EnableIRQ(TIM6_DAC_IRQn);
        TIM6->CNT = 0;
        TIM6->PSC = scaler;
        TIM6->ARR = 0xFFFF;
        TIM6->DIER = 1;
        TIM6->CR1 = 0x5;
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
        result = (uint16_t)(TIM6->CNT);
        TIM6->DIER = 1;
        return (timeBroad << (16 - bitScaling)) | (result >> bitScaling);
    }
    static uint64_t getTimeMillisBig() {
        uint32_t result;
        TIM6->DIER = 0;
        result |= (uint16_t)(TIM6->CNT);
        TIM6->DIER = 1;
        return (((uint64_t) timeBroad) << (16 - bitScaling)) | (result >> bitScaling);
    }
};
#endif /* SRC_TEST_CPP_LOWLEVEL_CLOCKSLOWLEVEL_SYSTEMMILLICLOCK_HPP_ */

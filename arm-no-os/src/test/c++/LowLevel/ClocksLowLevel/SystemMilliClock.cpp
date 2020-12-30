/*
 * SystemMilliClock.cpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#include "SystemMilliClock.hpp"

uint32_t SystemMilliClock::timeBroad = 0;
uint8_t SystemMilliClock::bitScaling = 0;

void TIM6_DAC_IRQHandler() {
    SystemMilliClock::resetTimer();
}

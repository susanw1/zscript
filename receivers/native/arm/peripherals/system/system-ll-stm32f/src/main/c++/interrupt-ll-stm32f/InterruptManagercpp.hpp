/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "InterruptManager.hpp"
void (*InterruptManager::interrupt[NUMBER_OF_INTERRUPT_TYPES])(uint8_t) = {NULL, NULL, NULL, NULL};

void DMA_Channel1_IRQHandler() {
    InterruptManager::call(DmaInt, 0);
}
void DMA1_Channel2_3_IRQHandler() {
    InterruptManager::call(DmaInt, 1);
}
void DMA1_Channel4_5_IRQHandler() {
    InterruptManager::call(DmaInt, 2);
}

void I2C1_IRQHandler() {
    InterruptManager::call(I2cInt, 0);
}
void I2C2_IRQHandler() {
    InterruptManager::call(I2cInt, 1);
}

void USART1_IRQHandler() {
    InterruptManager::call(UartInt, 0);
}
void USART2_IRQHandler() {
    InterruptManager::call(UartInt, 1);
}

void SysTick_Handler() {
    InterruptManager::call(SysTickInt, 0);
}

uint8_t WARNING_INCLUDE_CPP_IN_FILE_ONLY_USE_ONCE_SET_IN_MORE_THAN_ONE_CPP_FILE = 0;

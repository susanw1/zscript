/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_INTERRUPT_INTERRUPTMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_INTERRUPT_INTERRUPTMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>

enum InterruptType {
    DmaInt, I2cInt, UartInt, SysTickInt, NUMBER_OF_INTERRUPT_TYPES
};

extern "C" {
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_3_IRQHandler();
void DMA1_Channel4_5_IRQHandler();

void I2C1_IRQHandler();
void I2C2_IRQHandler();

void USART1_IRQHandler();
void USART2_IRQHandler();

void SysTick_Handler();
}

class InterruptManager {
private:
    friend void DMA_Channel1_IRQHandler();
    friend void DMA1_Channel2_3_IRQHandler();
    friend void DMA1_Channel4_5_IRQHandler();
    friend void DMA_Channel4_IRQHandler();
    friend void DMA_Channel5_IRQHandler();

    friend void I2C1_IRQHandler();
    friend void I2C2_IRQHandler();

    friend void USART1_IRQHandler();
    friend void USART2_IRQHandler();

    friend void SysTick_Handler();

    static void (*interrupt[NUMBER_OF_INTERRUPT_TYPES])(uint8_t);

    static uint8_t WARNING_INCLUDE_CPP_IN_FILE_ONLY_USE_ONCE_SET_IN_MORE_THAN_ONE_CPP_FILE;

    static void call(InterruptType type, uint8_t number) {
        if (interrupt[type] != NULL) {
            interrupt[type](number);
        }
    }
    static void enableDmaInterrupt(uint8_t number, uint8_t priority) {
        switch (number) {
        case 0:
            NVIC_SetPriority(DMA1_Channel1_IRQn, priority);
            NVIC_EnableIRQ (DMA1_Channel1_IRQn);
            break;
        case 1:
            NVIC_SetPriority(DMA1_Channel2_3_IRQn, priority);
            NVIC_EnableIRQ (DMA1_Channel2_3_IRQn);
            break;
        case 2:
            NVIC_SetPriority(DMA1_Channel4_5_IRQn, priority);
            NVIC_EnableIRQ (DMA1_Channel4_5_IRQn);
            break;
        default:
            break;
        }
    }
public:
    static void enableInterrupt(InterruptType type, uint8_t number, uint8_t priority) {
        switch (type) {
        case DmaInt:
            enableDmaInterrupt(number, priority);
            break;
        case I2cInt:
            if (number == 0) {
                NVIC_SetPriority(I2C1_IRQn, priority);
                NVIC_EnableIRQ (I2C1_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(I2C2_IRQn, priority);
                NVIC_EnableIRQ (I2C2_IRQn);

            }
            break;
        case UartInt:
            if (number == 0) {
                NVIC_SetPriority(USART1_IRQn, priority);
                NVIC_EnableIRQ (USART1_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(USART2_IRQn, priority);
                NVIC_EnableIRQ (USART2_IRQn);
            }
            break;
        case SysTickInt:
            NVIC_SetPriority(SysTick_IRQn, priority);
            NVIC_EnableIRQ (SysTick_IRQn);
            break;
        default:
            break;
        }
    }
    static void setInterrupt(void (*interrupt)(uint8_t), InterruptType type) {
        InterruptManager::interrupt[type] = interrupt;
    }
};
#ifdef INCLUDE_CPP_IN_FILE_ONLY_USE_ONCE
#include "InterruptManagercpp.hpp"
#endif

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_INTERRUPT_INTERRUPTMANAGER_HPP_ */

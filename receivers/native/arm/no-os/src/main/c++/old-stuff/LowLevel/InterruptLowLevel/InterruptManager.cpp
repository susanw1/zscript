/*
 * InterruptManager.cpp
 *
 *  Created on: 7 Jul 2022
 *      Author: robert
 */

#include "InterruptManager.hpp"

void (*InterruptManager::interrupt[NUMBER_OF_INTERRUPT_TYPES])(uint8_t) = {NULL, NULL, NULL, NULL, NULL, NULL};

void DMA1_Channel1_IRQHandler() {
    InterruptManager::call(DmaInt, 0);
}
void DMA1_Channel2_IRQHandler() {
    InterruptManager::call(DmaInt, 1);
}
void DMA1_Channel3_IRQHandler() {
    InterruptManager::call(DmaInt, 2);
}
void DMA1_Channel4_IRQHandler() {
    InterruptManager::call(DmaInt, 3);
}
void DMA1_Channel5_IRQHandler() {
    InterruptManager::call(DmaInt, 4);
}
void DMA1_Channel6_IRQHandler() {
    InterruptManager::call(DmaInt, 5);
}
void DMA1_Channel7_IRQHandler() {
    InterruptManager::call(DmaInt, 6);
}
void DMA1_Channel8_IRQHandler() {
    InterruptManager::call(DmaInt, 7);
}

void DMA2_Channel1_IRQHandler() {
    InterruptManager::call(DmaInt, 8);
}
void DMA2_Channel2_IRQHandler() {
    InterruptManager::call(DmaInt, 9);
}
void DMA2_Channel3_IRQHandler() {
    InterruptManager::call(DmaInt, 10);
}
void DMA2_Channel4_IRQHandler() {
    InterruptManager::call(DmaInt, 11);
}
void DMA2_Channel5_IRQHandler() {
    InterruptManager::call(DmaInt, 12);
}
void DMA2_Channel6_IRQHandler() {
    InterruptManager::call(DmaInt, 13);
}
void DMA2_Channel7_IRQHandler() {
    InterruptManager::call(DmaInt, 14);
}
void DMA2_Channel8_IRQHandler() {
    InterruptManager::call(DmaInt, 15);
}

void I2C1_EV_IRQHandler() {
    InterruptManager::call(I2cEv, 0);
}
void I2C2_EV_IRQHandler() {
    InterruptManager::call(I2cEv, 1);
}
void I2C3_EV_IRQHandler() {
    InterruptManager::call(I2cEv, 2);
}
void I2C4_EV_IRQHandler() {
    InterruptManager::call(I2cEv, 3);
}

void I2C1_ER_IRQHandler() {
    InterruptManager::call(I2cEr, 0);
}
void I2C2_ER_IRQHandler() {
    InterruptManager::call(I2cEr, 1);
}
void I2C3_ER_IRQHandler() {
    InterruptManager::call(I2cEr, 2);
}
void I2C4_ER_IRQHandler() {
    InterruptManager::call(I2cEr, 3);
}

void USART1_IRQHandler() {
    InterruptManager::call(UartInt, 0);
}
void USART2_IRQHandler() {
    InterruptManager::call(UartInt, 1);
}
void USART3_IRQHandler() {
    InterruptManager::call(UartInt, 2);
}

void UART4_IRQHandler() {
    InterruptManager::call(UartInt, 3);
}
void UART5_IRQHandler() {
    InterruptManager::call(UartInt, 4);
}

void LPUART1_IRQHandler() {
    InterruptManager::call(UartInt, 5);
}

void USB_LP_IRQHandler() {
    InterruptManager::call(UsbInt, 0);
}
void USB_HP_IRQHandler() {
    InterruptManager::call(UsbInt, 1);
}

void TIM6_DAC_IRQHandler() {
    InterruptManager::call(TIM6_, 0);
}
void UCPD1_IRQHandler() {
    InterruptManager::call(UcpdInt, 0);
}

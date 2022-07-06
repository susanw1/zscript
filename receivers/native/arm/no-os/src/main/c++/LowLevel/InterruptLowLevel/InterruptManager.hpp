/*
 * InterruptManager.hpp
 *
 *  Created on: 5 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_INTERRUPTLOWLEVEL_INTERRUPTMANAGER_HPP_
#define SRC_MAIN_C___LOWLEVEL_INTERRUPTLOWLEVEL_INTERRUPTMANAGER_HPP_

#include <llIncludes.hpp>

enum InterruptType {
    DMA, I2cEv, I2cEr, Uart, Usb, TIM6_, NUMBER_OF_INTERRUPT_TYPES
};

class InterruptManager {
private:
    friend void DMA1_Channel1_IRQHandler();
    friend void DMA1_Channel2_IRQHandler();
    friend void DMA1_Channel3_IRQHandler();
    friend void DMA1_Channel4_IRQHandler();
    friend void DMA1_Channel5_IRQHandler();
    friend void DMA1_Channel6_IRQHandler();
    friend void DMA1_Channel7_IRQHandler();
    friend void DMA1_Channel8_IRQHandler();

    friend void DMA2_Channel1_IRQHandler();
    friend void DMA2_Channel2_IRQHandler();
    friend void DMA2_Channel3_IRQHandler();
    friend void DMA2_Channel4_IRQHandler();
    friend void DMA2_Channel5_IRQHandler();
    friend void DMA2_Channel6_IRQHandler();
    friend void DMA2_Channel7_IRQHandler();
    friend void DMA2_Channel8_IRQHandler();

    friend void I2C1_EV_IRQHandler();
    friend void I2C2_EV_IRQHandler();
    friend void I2C3_EV_IRQHandler();
    friend void I2C4_EV_IRQHandler();

    friend void I2C1_ER_IRQHandler();
    friend void I2C2_ER_IRQHandler();
    friend void I2C3_ER_IRQHandler();
    friend void I2C4_ER_IRQHandler();

    friend void USART1_IRQHandler();
    friend void USART2_IRQHandler();
    friend void USART3_IRQHandler();

    friend void UART4_IRQHandler();
    friend void UART5_IRQHandler();

    friend void LPUART1_IRQHandler();

    friend void USB_LP_IRQHandler();
    friend void USB_HP_IRQHandler();

    friend void TIM6_DAC_IRQHandler();

    static void (*interrupt[NUMBER_OF_INTERRUPT_TYPES])(uint8_t);

    static void call(InterruptType type, uint8_t number) {
        if (interrupt[type] != NULL) {
            interrupt[type](number);
        }
    }
    static void enableDmaInterrupt(uint8_t number, uint8_t priority) {
        switch (number) {
        case 0:
            NVIC_SetPriority(DMA1_Channel1_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel1_IRQn);
            break;
        case 1:
            NVIC_SetPriority(DMA1_Channel2_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel2_IRQn);
            break;
        case 2:
            NVIC_SetPriority(DMA1_Channel3_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel3_IRQn);
            break;
        case 3:
            NVIC_SetPriority(DMA1_Channel4_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel4_IRQn);
            break;
        case 4:
            NVIC_SetPriority(DMA1_Channel5_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel5_IRQn);
            break;
        case 5:
            NVIC_SetPriority(DMA1_Channel6_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel6_IRQn);
            break;
        case 6:
            NVIC_SetPriority(DMA1_Channel7_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel7_IRQn);
            break;
        case 7:
            NVIC_SetPriority(DMA1_Channel8_IRQn, priority);
            NVIC_EnableIRQ(DMA1_Channel8_IRQn);
            break;
        case 8:
            NVIC_SetPriority(DMA2_Channel1_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel1_IRQn);
            break;
        case 9:
            NVIC_SetPriority(DMA2_Channel2_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel2_IRQn);
            break;
        case 10:
            NVIC_SetPriority(DMA2_Channel3_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel3_IRQn);
            break;
        case 11:
            NVIC_SetPriority(DMA2_Channel4_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel4_IRQn);
            break;
        case 12:
            NVIC_SetPriority(DMA2_Channel5_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel5_IRQn);
            break;
        case 13:
            NVIC_SetPriority(DMA2_Channel6_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel6_IRQn);
            break;
        case 14:
            NVIC_SetPriority(DMA2_Channel7_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel7_IRQn);
            break;
        case 15:
            NVIC_SetPriority(DMA2_Channel8_IRQn, priority);
            NVIC_EnableIRQ(DMA2_Channel8_IRQn);
            break;
        default:
            break;
        }
    }
public:
    static void enableInterrupt(InterruptType type, uint8_t number, uint8_t priority) {
        switch (type) {
        case DMA:
            enableDmaInterrupt(number, priority);
            break;
        case I2cEv:
            if (number == 0) {
                NVIC_SetPriority(I2C1_EV_IRQn, priority);
                NVIC_EnableIRQ(I2C1_EV_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(I2C2_EV_IRQn, priority);
                NVIC_EnableIRQ(I2C2_EV_IRQn);

            } else if (number == 2) {
                NVIC_SetPriority(I2C3_EV_IRQn, priority);
                NVIC_EnableIRQ(I2C3_EV_IRQn);

            } else if (number == 3) {
                NVIC_SetPriority(I2C4_EV_IRQn, priority);
                NVIC_EnableIRQ(I2C4_EV_IRQn);

            }
            break;
        case I2cEr:
            if (number == 0) {
                NVIC_SetPriority(I2C1_ER_IRQn, priority);
                NVIC_EnableIRQ(I2C1_ER_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(I2C2_ER_IRQn, priority);
                NVIC_EnableIRQ(I2C2_ER_IRQn);

            } else if (number == 2) {
                NVIC_SetPriority(I2C3_ER_IRQn, priority);
                NVIC_EnableIRQ(I2C3_ER_IRQn);

            } else if (number == 3) {
                NVIC_SetPriority(I2C4_ER_IRQn, priority);
                NVIC_EnableIRQ(I2C4_ER_IRQn);

            }
            break;
        case Uart:
            if (number == 0) {
                NVIC_SetPriority(USART1_IRQn, priority);
                NVIC_EnableIRQ(USART1_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(USART2_IRQn, priority);
                NVIC_EnableIRQ(USART2_IRQn);
            } else if (number == 2) {
                NVIC_SetPriority(USART3_IRQn, priority);
                NVIC_EnableIRQ(USART3_IRQn);
            } else if (number == 3) {
                NVIC_SetPriority(UART4_IRQn, priority);
                NVIC_EnableIRQ(UART4_IRQn);
            } else if (number == 4) {
                NVIC_SetPriority(UART5_IRQn, priority);
                NVIC_EnableIRQ(UART5_IRQn);
            } else if (number == 5) {
                NVIC_SetPriority(LPUART1_IRQn, priority);
                NVIC_EnableIRQ(LPUART1_IRQn);
            }
            break;
        case Uart:
            if (number == 0) {
                NVIC_SetPriority(USART1_IRQn, priority);
                NVIC_EnableIRQ(USART1_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(USART2_IRQn, priority);
                NVIC_EnableIRQ(USART2_IRQn);
            } else if (number == 2) {
                NVIC_SetPriority(USART3_IRQn, priority);
                NVIC_EnableIRQ(USART3_IRQn);
            } else if (number == 3) {
                NVIC_SetPriority(UART4_IRQn, priority);
                NVIC_EnableIRQ(UART4_IRQn);
            } else if (number == 4) {
                NVIC_SetPriority(UART5_IRQn, priority);
                NVIC_EnableIRQ(UART5_IRQn);
            } else if (number == 5) {
                NVIC_SetPriority(LPUART1_IRQn, priority);
                NVIC_EnableIRQ(LPUART1_IRQn);
            }
            break;
        case Usb:
            if (number == 0) {
                NVIC_SetPriority(USB_LP_IRQn, priority);
                NVIC_DisableIRQ(USB_LP_IRQn);
            } else if (number == 1) {
                NVIC_SetPriority(USB_HP_IRQn, priority);
                NVIC_DisableIRQ(USB_HP_IRQn);
            }
            break;
        case TIM6_:
            NVIC_SetPriority(TIM6_DAC_IRQn, priority);
            NVIC_EnableIRQ(TIM6_DAC_IRQn);
            break;
        default:
            break;
        }
    }
    static void setInterrupt(void (*interrupt)(uint8_t), InterruptType type) {
        interrupt[type] = interrupt;
    }
};

extern "C" {
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
void DMA1_Channel4_IRQHandler();
void DMA1_Channel5_IRQHandler();
void DMA1_Channel6_IRQHandler();
void DMA1_Channel7_IRQHandler();
void DMA1_Channel8_IRQHandler();

void DMA2_Channel1_IRQHandler();
void DMA2_Channel2_IRQHandler();
void DMA2_Channel3_IRQHandler();
void DMA2_Channel4_IRQHandler();
void DMA2_Channel5_IRQHandler();
void DMA2_Channel6_IRQHandler();
void DMA2_Channel7_IRQHandler();
void DMA2_Channel8_IRQHandler();

void I2C1_EV_IRQHandler();
void I2C2_EV_IRQHandler();
void I2C3_EV_IRQHandler();
void I2C4_EV_IRQHandler();

void I2C1_ER_IRQHandler();
void I2C2_ER_IRQHandler();
void I2C3_ER_IRQHandler();
void I2C4_ER_IRQHandler();

void USART1_IRQHandler();
void USART2_IRQHandler();
void USART3_IRQHandler();

void UART4_IRQHandler();
void UART5_IRQHandler();

void LPUART1_IRQHandler();

void USB_LP_IRQHandler();
void USB_HP_IRQHandler();

void TIM6_DAC_IRQHandler();
}

void DMA1_Channel1_IRQHandler() {
    InterruptManager::call(DMA, 0);
}
void DMA1_Channel2_IRQHandler() {
    InterruptManager::call(DMA, 1);
}
void DMA1_Channel3_IRQHandler() {
    InterruptManager::call(DMA, 2);
}
void DMA1_Channel4_IRQHandler() {
    InterruptManager::call(DMA, 3);
}
void DMA1_Channel5_IRQHandler() {
    InterruptManager::call(DMA, 4);
}
void DMA1_Channel6_IRQHandler() {
    InterruptManager::call(DMA, 5);
}
void DMA1_Channel7_IRQHandler() {
    InterruptManager::call(DMA, 6);
}
void DMA1_Channel8_IRQHandler() {
    InterruptManager::call(DMA, 7);
}

void DMA2_Channel1_IRQHandler() {
    InterruptManager::call(DMA, 8);
}
void DMA2_Channel2_IRQHandler() {
    InterruptManager::call(DMA, 9);
}
void DMA2_Channel3_IRQHandler() {
    InterruptManager::call(DMA, 10);
}
void DMA2_Channel4_IRQHandler() {
    InterruptManager::call(DMA, 11);
}
void DMA2_Channel5_IRQHandler() {
    InterruptManager::call(DMA, 12);
}
void DMA2_Channel6_IRQHandler() {
    InterruptManager::call(DMA, 13);
}
void DMA2_Channel7_IRQHandler() {
    InterruptManager::call(DMA, 14);
}
void DMA2_Channel8_IRQHandler() {
    InterruptManager::call(DMA, 15);
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
    InterruptManager::call(Uart, 0);
}
void USART2_IRQHandler() {
    InterruptManager::call(Uart, 1);
}
void USART3_IRQHandler() {
    InterruptManager::call(Uart, 2);
}

void UART4_IRQHandler() {
    InterruptManager::call(Uart, 3);
}
void UART5_IRQHandler() {
    InterruptManager::call(Uart, 4);
}

void LPUART1_IRQHandler() {
    InterruptManager::call(Uart, 5);
}

void USB_LP_IRQHandler() {
    InterruptManager::call(Usb, 0);
}
void USB_HP_IRQHandler() {
    InterruptManager::call(Usb, 1);
}

void TIM6_DAC_IRQHandler() {
    InterruptManager::call(TIM6_, 0);
}

#endif /* SRC_MAIN_C___LOWLEVEL_INTERRUPTLOWLEVEL_INTERRUPTMANAGER_HPP_ */

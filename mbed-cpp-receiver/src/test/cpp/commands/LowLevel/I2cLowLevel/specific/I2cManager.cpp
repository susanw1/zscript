/*
 * I2cManager.cpp
 *
 *  Created on: 19 Dec 2020
 *      Author: robert
 */

#include "../../I2cLowLevel/I2cManager.hpp"

#define I2C_SDA_Inner(I2C, PIN) I2C##_SDA_##PIN
#define I2C_SDA(I2C, PIN) I2C_SDA_Inner(I2C, PIN)

#define I2C_SCL_Inner(I2C, PIN) I2C##_SCL_##PIN
#define I2C_SCL(I2C, PIN) I2C_SCL_Inner(I2C, PIN)

#define I2C_1_SCL_PA_13 GPIO_AF4_I2C1
#define I2C_1_SDA_PA_14 GPIO_AF4_I2C1
#define I2C_1_SCL_PA_15 GPIO_AF4_I2C1
#define I2C_1_SDA_PB_7 GPIO_AF4_I2C1
#define I2C_1_SCL_PB_8 GPIO_AF4_I2C1
#define I2C_1_SDA_PB_9 GPIO_AF4_I2C1

#define I2C_2_SDA_PA_8 GPIO_AF4_I2C2
#define I2C_2_SCL_PA_9 GPIO_AF4_I2C2
#define I2C_2_SCL_PC_4 GPIO_AF4_I2C2
#define I2C_2_SDA_PF_0 GPIO_AF4_I2C2
#define I2C_2_SCL_PF_6 GPIO_AF4_I2C2

#define I2C_3_SCL_PA_8 GPIO_AF2_I2C3
#define I2C_3_SDA_PB_5 GPIO_AF8_I2C3
#define I2C_3_SCL_PC_8 GPIO_AF8_I2C3
#define I2C_3_SDA_PC_9 GPIO_AF8_I2C3
#define I2C_3_SDA_PC_11 GPIO_AF8_I2C3
#define I2C_3_SCL_PF_3 GPIO_AF4_I2C3
#define I2C_3_SDA_PF_4 GPIO_AF4_I2C3
#define I2C_3_SCL_PG_7 GPIO_AF4_I2C3
#define I2C_3_SDA_PG_8 GPIO_AF4_I2C3

#define I2C_4_SCL_PA_13 GPIO_AF3_I2C4
#define I2C_4_SDA_PB_7 GPIO_AF3_I2C4
#define I2C_4_SCL_PC_6 GPIO_AF8_I2C4
#define I2C_4_SDA_PC_7 GPIO_AF8_I2C4
#define I2C_4_SCL_PF_14 GPIO_AF4_I2C4
#define I2C_4_SDA_PF_15 GPIO_AF4_I2C4
#define I2C_4_SCL_PG_3 GPIO_AF4_I2C4
#define I2C_4_SDA_PG_4 GPIO_AF4_I2C4

#define _I2C_1_SCL_PA_13 1
#define _I2C_1_SDA_PA_14 1
#define _I2C_1_SCL_PA_15 1
#define _I2C_1_SDA_PB_7 1
#define _I2C_1_SCL_PB_8 1
#define _I2C_1_SDA_PB_9 1

#define _I2C_2_SDA_PA_8 1
#define _I2C_2_SCL_PA_9 1
#define _I2C_2_SCL_PC_4 1
#define _I2C_2_SDA_PF_0 1
#define _I2C_2_SCL_PF_6 1

#define _I2C_3_SCL_PA_8 1
#define _I2C_3_SDA_PB_5 1
#define _I2C_3_SCL_PC_8 1
#define _I2C_3_SDA_PC_9 1
#define _I2C_3_SDA_PC_11 1
#define _I2C_3_SCL_PF_3 1
#define _I2C_3_SDA_PF_4 1
#define _I2C_3_SCL_PG_7 1
#define _I2C_3_SDA_PG_8 1

#define _I2C_4_SCL_PA_13 1
#define _I2C_4_SDA_PB_7 1
#define _I2C_4_SCL_PC_6 1
#define _I2C_4_SDA_PC_7 1
#define _I2C_4_SCL_PF_14 1
#define _I2C_4_SDA_PF_15 1
#define _I2C_4_SCL_PG_3 1
#define _I2C_4_SDA_PG_4 1

#ifdef USE_I2C_1
#if !I2C_SDA(_I2C_1, I2C_1_SDA)
#error  Not defined as a valid I2C 1 SDA pin: I2C_1_SDA
#endif
#if !I2C_SCL(_I2C_1, I2C_1_SCL)
#error  Not defined as a valid I2C 1 SCL pin: I2C_1_SCL
#endif
#endif

#ifdef USE_I2C_2
#if !I2C_SDA(_I2C_2, I2C_2_SDA)
#error  Not defined as a valid I2C 2 SDA pin: I2C_2_SDA
#endif
#if !I2C_SCL(_I2C_2, I2C_2_SCL)
#error  Not defined as a valid I2C 2 SCL pin: I2C_2_SCL
#endif
#endif

#ifdef USE_I2C_3
#if !I2C_SDA(_I2C_3, I2C_3_SDA)
#error  Not defined as a valid I2C 3 SDA pin: I2C_3_SDA
#endif
#if !I2C_SCL(_I2C_3, I2C_3_SCL)
#error  Not defined as a valid I2C 3 SCL pin: I2C_3_SCL
#endif
#endif

#ifdef USE_I2C_4
#if !I2C_SDA(_I2C_4, I2C_4_SDA)
#error  Not defined as a valid I2C 4 SDA pin: I2C_4_SDA
#endif
#if !I2C_SCL(_I2C_4, I2C_4_SCL)
#error  Not defined as a valid I2C 4 SCL pin: I2C_4_SCL
#endif
#endif

I2c I2cManager::i2cs[] = {I2c(), I2c(), I2c(), I2c()};

class I2cInterruptManager {
    friend I2cManager;

    static void IRQI2C1() {
        I2cManager::i2cs[0].interrupt();
    }
    static void IRQI2C2() {
        I2cManager::i2cs[1].interrupt();
    }
    static void IRQI2C3() {
        I2cManager::i2cs[2].interrupt();
    }
    static void IRQI2C4() {
        I2cManager::i2cs[3].interrupt();
    }
};

I2cInternal getI2cInternal(I2cIdentifier id) {
    if (id == 0) {
#ifdef USE_I2C_1
        return I2cInternal(I2C_1_SDA, I2C_SDA(I2C_1, I2C_1_SDA), I2C_1_SCL, I2C_SCL(I2C_1, I2C_1_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal();
#endif
    } else if (id == 1) {
#ifdef USE_I2C_2
        return I2cInternal(I2C_2_SDA, I2C_SDA(I2C_2, I2C_2_SDA), I2C_2_SCL, I2C_SCL(I2C_2, I2C_2_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal();
#endif
    } else if (id == 2) {
#ifdef USE_I2C_3
        return I2cInternal(I2C_3_SDA, I2C_SDA(I2C_3, I2C_3_SDA), I2C_3_SCL, I2C_SCL(I2C_3, I2C_3_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal();
#endif
    } else {
#ifdef USE_I2C_4
        return I2cInternal(I2C_4_SDA, I2C_SDA(I2C_4, I2C_4_SDA), I2C_4_SCL, I2C_SCL(I2C_4, I2C_4_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal();
#endif
    }
}
void I2cManager::init() {
    for (int i = 0; i < GeneralHalSetup::dmaCount; ++i) {
        i2cs[i].setI2c(getI2cInternal(i), i);
    }

    NVIC_SetVector(I2C1_EV_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C1);
    NVIC_SetVector(I2C1_ER_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C1);

    NVIC_SetVector(I2C2_EV_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C2);
    NVIC_SetVector(I2C2_ER_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C2);

    NVIC_SetVector(I2C3_EV_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C3);
    NVIC_SetVector(I2C3_ER_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C3);

    NVIC_SetVector(I2C4_EV_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C4);
    NVIC_SetVector(I2C4_ER_IRQn, (uint32_t) &I2cInterruptManager::IRQI2C4);
}

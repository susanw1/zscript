/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_

#define I2C_SDA_Inner(I2C,PIN) I2C##_SDA_##PIN
#define I2C_SDA(I2C,PIN) I2C_SDA_Inner(I2C, PIN)

#define I2C_SCL_Inner(I2C,PIN) I2C##_SCL_##PIN
#define I2C_SCL(I2C,PIN) I2C_SCL_Inner(I2C, PIN)

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

template<class LL>
I2c<LL> I2cManager<LL>::i2cs[HW::i2cCount];

template<class LL>
I2cInternal<LL> getI2cInternal_internal(I2cIdentifier id) {
    if (id == 0) {
#ifdef USE_I2C_1
        return I2cInternal<LL>(I2C_SDA(I2C_1, I2C_1_SDA), I2C_SCL(I2C_1, I2C_1_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal<LL>();
#endif
    } else {
#ifdef USE_I2C_2
        return I2cInternal<LL>(I2C_SDA(I2C_2, I2C_2_SDA), I2C_SCL(I2C_2, I2C_2_SCL), (I2cRegisters*) 0x40005800);
#else
        return I2cInternal<LL>();
#endif
    }
}

template<class LL>
void I2cManager<LL>::interrupt(uint8_t id) {
    i2cs[id].interrupt();
}

template<class LL>
void I2cManager<LL>::init() {

    InterruptManager::setInterrupt(&I2cManager::interrupt, InterruptType::I2cInt);

    for (int i = 0; i < HW::i2cCount; ++i) {
        i2cs[i].setI2c(getI2cInternal_internal < LL > (i), i);
        i2cs[i].init();
        InterruptManager::enableInterrupt(InterruptType::I2cInt, i, 10);
    }
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_ */

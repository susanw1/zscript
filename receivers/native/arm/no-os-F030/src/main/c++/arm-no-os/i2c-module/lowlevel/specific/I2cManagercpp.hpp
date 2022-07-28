/*
 * I2cManagercpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_

#include "../I2cManager.hpp"

#define I2C_SDA_Inner(I2C,PIN) I2C##_SDA_##PIN
#define I2C_SDA(I2C,PIN) I2C_SDA_Inner(I2C, PIN)

#define I2C_SCL_Inner(I2C,PIN) I2C##_SCL_##PIN
#define I2C_SCL(I2C,PIN) I2C_SCL_Inner(I2C, PIN)

#define I2C_1_SCL_PB_6_ PB_6,GPIO_AF1
#define I2C_1_SDA_PB_7_ PB_7,GPIO_AF1
#define I2C_1_SCL_PB_8_ PB_8,GPIO_AF1
#define I2C_1_SDA_PB_9_ PB_9,GPIO_AF1

#define I2C_2_SCL_PB_10_ PB_10,GPIO_AF1
#define I2C_2_SDA_PB_11_ PB_11,GPIO_AF1
#define I2C_2_SCL_PB_13_ PB_13,GPIO_AF5
#define I2C_2_SDA_PB_14_ PB_14,GPIO_AF5
#define I2C_2_SCL_PF_6_ PF_6,GPIO_AF0
#define I2C_2_SDA_PF_7_ PF_7,GPIO_AF0

#define _I2C_1_SCL_PB_6_ 1
#define _I2C_1_SDA_PB_7_ 1
#define _I2C_1_SCL_PB_8_ 1
#define _I2C_1_SDA_PB_9_ 1

#define _I2C_2_SCL_PB_10_ 1
#define _I2C_2_SDA_PB_11_ 1
#define _I2C_2_SCL_PB_13_ 1
#define _I2C_2_SDA_PB_14_ 1
#define _I2C_2_SCL_PF_6_ 1
#define _I2C_2_SDA_PF_7_ 1

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
I2c<LL> I2cManager<LL>::i2cs[LL::i2cCount];

template<class LL>
I2cInternal<LL> getI2cInternal_internal(I2cIdentifier id) {
    if (id == 0) {
#ifdef USE_I2C_1
        return I2cInternal<LL>(I2C_SDA(I2C_1, I2C_1_SDA), I2C_SCL(I2C_1, I2C_1_SCL), (I2cRegisters*) 0x40005400);
#else
        return I2cInternal<LL>();
#endif
    } else if (id == 1) {
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

    for (int i = 0; i < LL::i2cCount; ++i) {
        i2cs[i].setI2c(getI2cInternal_internal<LL>(i), i);
        i2cs[i].init();
        InterruptManager::enableInterrupt(InterruptType::I2cInt, i, 10);
    }
}

#endif /* SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_ */

/*
 * I2cManagercpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_

#define I2C_SDA_Inner(I2C,PIN) I2C##_SDA_##PIN
#define I2C_SDA(I2C,PIN) I2C_SDA_Inner(I2C, PIN)

#define I2C_SCL_Inner(I2C,PIN) I2C##_SCL_##PIN
#define I2C_SCL(I2C,PIN) I2C_SCL_Inner(I2C, PIN)

#if defined(STM32F030x6)
#include <arm-no-os/stm32f030x6/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F030x8)
#include <arm-no-os/stm32f030x8/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F031x6)
#include <arm-no-os/stm32f031x6/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F038xx)
#include <arm-no-os/stm32f038/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F042x6)
#include <arm-no-os/stm32f042x6/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F048xx)
#include <arm-no-os/stm32f048/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F051x8)
#include <arm-no-os/stm32f051x8/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F058xx)
#include <arm-no-os/stm32f058/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F070x6)
#include <arm-no-os/stm32f070x6/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F070xB)
#include <arm-no-os/stm32f070xb/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F071xB)
#include <arm-no-os/stm32f071xb/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F072xB)
#include <arm-no-os/stm32f072xb/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F078xx)
#include <arm-no-os/stm32f078/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F091xC)
#include <arm-no-os/stm32f091xc/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F098xx)
#include <arm-no-os/stm32f098/i2c-module/lowlevel/specific/I2cPins.hpp>
#elif defined(STM32F030xC)
#include <arm-no-os/stm32f030xc/i2c-module/lowlevel/specific/I2cPins.hpp>
#else
#error "Please select the target STM32F0xx device used in your application"
#endif

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

#endif /* SRC_MAIN_C___LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CMANAGERCPP_HPP_ */

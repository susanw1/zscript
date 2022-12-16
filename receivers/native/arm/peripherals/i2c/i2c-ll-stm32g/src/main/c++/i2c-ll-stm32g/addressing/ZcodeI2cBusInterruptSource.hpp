/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <i2c-ll/addressing/ZcodeI2cBusInterruptSource.hpp>

template<class ZP>
const GpioPin<ZP::LL> ZcodeI2cBusInterruptSource<ZP>::interruptPins[ZP::LL::HW::i2cCount] = { I2C_1_ALERT, I2C_2_ALERT, I2C_3_ALERT, I2C_4_ALERT };

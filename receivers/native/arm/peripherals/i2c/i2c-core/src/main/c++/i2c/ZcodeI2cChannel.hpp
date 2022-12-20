/*
 * ZcodeI2cChannel.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_CPP_I2C_ZCODEI2CCHANNEL_HPP_
#define SRC_MAIN_CPP_I2C_ZCODEI2CCHANNEL_HPP_

// Every I2C Channel should have a constructor taking only an I2C identifier

#if defined(I2C_LL)
#include <i2c-ll/ZcodeI2cChannel.hpp>

#else
#error No I2C system specified for usage
#endif

#endif /* SRC_MAIN_CPP_I2C_ZCODEI2CCHANNEL_HPP_ */

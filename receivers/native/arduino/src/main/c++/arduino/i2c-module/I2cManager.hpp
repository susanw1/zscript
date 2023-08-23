/*
 * I2cManager.hpp
 *
 *  Created on: 20 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CMANAGER_HPP_
#define SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CMANAGER_HPP_

#ifndef I2C_ADDRESS_AND_GENERAL_CALL
#if defined(__AVR__)
#define I2C_ADDRESS_AND_GENERAL_CALL(addr) TWAR = (addr<<1) | 1
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_ADDRESS_AND_GENERAL_CALL(addr) NRF_TWIS0->ADDRESS[0] = addr; NRF_TWIS0->ADDRESS[1] = 0; NRF_TWIS0->CONFIG = 0x3;
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_BEGIN_SMBUS_ALERT
#if defined(__AVR__)
#define I2C_BEGIN_SMBUS_ALERT() TWAR = (0xC<<1) | 1
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_BEGIN_SMBUS_ALERT(addr) NRF_TWIS0->ADDRESS[1] = 0xC;
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_END_SMBUS_ALERT
#if defined(__AVR__)
#define I2C_END_SMBUS_ALERT(addr) TWAR = (addr<<1) | 1
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_END_SMBUS_ALERT(addr) NRF_TWIS0->ADDRESS[1] = 0x0;
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_WAS_SMBUS_ALERT_READ
#if defined(__AVR__)
#define I2C_WAS_SMBUS_ALERT_READ() true
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_WAS_SMBUS_ALERT_READ() (NRF_TWIS0->MATCH == 1)
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_SMBUS_ALERT_IS_EXCLUSIVE_WITH_ADDR
#if defined(__AVR__)
#define I2C_SMBUS_ALERT_IS_EXCLUSIVE_WITH_ADDR() true
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_SMBUS_ALERT_IS_EXCLUSIVE_WITH_ADDR() false
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CMANAGER_HPP_ */

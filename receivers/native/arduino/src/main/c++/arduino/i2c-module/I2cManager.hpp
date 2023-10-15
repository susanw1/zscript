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
#define I2C_ADDRESS_AND_GENERAL_CALL(addr) do { TWAR = (addr<<1) | 1; } while(0)
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_ADDRESS_AND_GENERAL_CALL(addr) do { NRF_TWIS0->ADDRESS[0] = addr; NRF_TWIS0->ADDRESS[1] = 0; NRF_TWIS0->CONFIG = 0x3; } while(0)
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_BEGIN_SMBUS_ALERT
#if defined(__AVR__)
#define I2C_BEGIN_SMBUS_ALERT() do { TWAR = (0xC<<1) | 1; } while(0)
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_BEGIN_SMBUS_ALERT(addr) do { NRF_TWIS0->ADDRESS[1] = 0xC; } while(0)
#else
#error "Unknown board, cannot implement I2C address set outside of wire library, so cannot use I2C Channel"
#endif
#endif

#ifndef I2C_END_SMBUS_ALERT
#if defined(__AVR__)
#define I2C_END_SMBUS_ALERT(addr) do { TWAR = (addr<<1) | 1; } while(0)
#elif defined(ARDUINO_ARCH_NRF52840)
#define I2C_END_SMBUS_ALERT(addr) do { NRF_TWIS0->ADDRESS[1] = 0x0; } while(0)
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


#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CMANAGER_HPP_ */

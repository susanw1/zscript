/*
 * SPI.h
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#define SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_

#include <LowLevel/GeneralLLSetup.hpp>
#include <LowLevel/GpioLowLevel/GpioManager.hpp>
#include <LowLevel/GpioLowLevel/Gpio.hpp>

class SPIClass {
    public:
        // Initialize the SPI library
        static void begin();

        static void beginTransaction();

        static uint8_t transfer(uint8_t data);

        static void transfer(uint8_t *buf, uint16_t count);

        static void endTransaction(void);

        static void end();
};

extern SPIClass SPI;

#endif /* SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_ */

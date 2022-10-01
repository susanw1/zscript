/*
 * SPI.h
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#define SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#include <arm-no-os/llIncludes.hpp>

#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>

#define SPI_SCK_Inner(SPI,PIN) SPI##_SCK_##PIN
#define SPI_SCK(SPI,PIN) SPI_SCK_Inner(SPI, PIN)

#define SPI_MISO_Inner(SPI,PIN) SPI##_MISO_##PIN
#define SPI_MISO(SPI,PIN) SPI_MISO_Inner(SPI, PIN)

#define SPI_MOSI_Inner(SPI,PIN) SPI##_MOSI_##PIN
#define SPI_MOSI(SPI,PIN) SPI_MOSI_Inner(SPI, PIN)

#if defined(STM32F030x6)
#include <arm-no-os/stm32f030x6/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F030x8)
#include <arm-no-os/stm32f030x8/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F031x6)
#include <arm-no-os/stm32f031x6/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F038xx)
#include <arm-no-os/stm32f038/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F042x6)
#include <arm-no-os/stm32f042x6/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F048xx)
#include <arm-no-os/stm32f048/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F051x8)
#include <arm-no-os/stm32f051x8/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F058xx)
#include <arm-no-os/stm32f058/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F070x6)
#include <arm-no-os/stm32f070x6/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F070xB)
#include <arm-no-os/stm32f070xb/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F071xB)
#include <arm-no-os/stm32f071xb/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F072xB)
#include <arm-no-os/stm32f072xb/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F078xx)
#include <arm-no-os/stm32f078/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F091xC)
#include <arm-no-os/stm32f091xc/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F098xx)
#include <arm-no-os/stm32f098/udp-module/lowlevel/SpiPins.hpp>
#elif defined(STM32F030xC)
#include <arm-no-os/stm32f030xc/udp-module/lowlevel/SpiPins.hpp>
#else
#error "Please select the target STM32F0xx device used in your application"
#endif

#ifndef USE_SPI_1
#error SPI_1 is needed for udp comunications...
#endif
#ifdef USE_SPI_1
#if !SPI_SCK(_SPI_1, SPI_1_SCK)
#error  Not defined as a valid SPI 1 SCK pin: SPI_1_SCK
#endif
#if !SPI_MISO(_SPI_1, SPI_1_MISO)
#error  Not defined as a valid SPI 1 MISO pin: SPI_1_MISO
#endif
#if !SPI_MOSI(_SPI_1, SPI_1_MOSI)
#error  Not defined as a valid SPI 1 MOSI pin: SPI_1_MOSI
#endif
#endif

#ifdef USE_SPI_2
#if !SPI_SCK(_SPI_2, SPI_2_SCK)
#error  Not defined as a valid SPI 2 SCK pin: SPI_2_SCK
#endif
#if !SPI_MISO(_SPI_2, SPI_2_MISO)
#error  Not defined as a valid SPI 2 MISO pin: SPI_2_MISO
#endif
#if !SPI_MOSI(_SPI_2, SPI_2_MOSI)
#error  Not defined as a valid SPI 2 MOSI pin: SPI_2_MOSI
#endif
#endif

template<class LL>
class SPIClass {
    static void setupPin(GpioPinName name, uint8_t af) {
        GpioPin<LL> pin = GpioManager<LL>::getPin(name);
        pin.init();
        pin.setAlternateFunction(af);
        pin.setOutputSpeed(VeryHighSpeed);
        pin.setMode(AlternateFunction);
        pin.setOutputMode(PushPull);
        pin.setPullMode(NoPull);
    }

public:
    // Initialize the SPI library
    static void begin();

    static void beginTransaction();

    static uint8_t transfer(uint8_t data);

    static void transfer(uint8_t *buf, uint16_t count);

    static void endTransaction(void);

    static void end();

};

template<class LL>
extern SPIClass<LL> SPI;

template<class LL>
void SPIClass<LL>::begin() {
    const uint16_t enableSpiRegisterClock = 0x1000;
    const uint16_t setFifo1_4Full = 0x1000;
    const uint16_t dataSize8bit = 0x0700;
    const uint16_t setSoftwareSlaveManagement = 0x0200;
    const uint16_t setInternalSlaveSelect = 0x0100;
    const uint16_t setAsMaster = 0x0004;
    const uint16_t enableSpi = 0x0040;

    RCC->APB2ENR |= enableSpiRegisterClock;
    SPI1->CR2 = setFifo1_4Full | dataSize8bit;
    SPI1->CR1 = setAsMaster | setSoftwareSlaveManagement | setInternalSlaveSelect;

    setupPin (SPI_SCK(SPI_1, SPI_1_SCK));
    setupPin (SPI_MISO(SPI_1, SPI_1_MISO));
    setupPin (SPI_MOSI(SPI_1, SPI_1_MOSI));

    SPI1->CR1 |= enableSpi;
}

template<class LL>
void SPIClass<LL>::beginTransaction() {
}

#pragma GCC push_options
#pragma GCC optimize ("-O0")
template<class LL>
uint8_t SPIClass<LL>::transfer(uint8_t data) {
    const uint16_t fifoLevelMask = 0x0001;

    // Forces 8 bit read/write, to allow 1 byte at a time
    *((volatile uint8_t*) &(SPI1->DR)) = data;
    volatile bool b = true;
    volatile uint16_t *sr = (volatile uint16_t*) &SPI1->SR;
    while (b) {
        b = ((*sr) & fifoLevelMask) == 0;
    }
    return *((volatile uint8_t*) &(SPI1->DR));
}
#pragma GCC pop_options

template<class LL>
void SPIClass<LL>::transfer(uint8_t *buf, uint16_t count) {
    for (int i = 0; i < count; ++i) {
        buf[i] = transfer(buf[i]);
    }
}

template<class LL>
void SPIClass<LL>::endTransaction(void) {
}

template<class LL>
void SPIClass<LL>::end() {
}

#endif /* SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_ */

/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_PINMANAGER_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_PINMANAGER_HPP_
#include <zscript/modules/ZscriptCommand.hpp>

#ifndef digitalPinHasPWM
#ifdef ARDUINO_ARCH_NRF52840    // NANO 33 BLE apparently...
#define digitalPinHasPWM(p) (p==2 || p==3 || p==5 || p==6 || p==9 || p==10 || p==11 || p==12)
#else
#define digitalPinHasPWM(p) (true)
#ifndef IGNORE_UNKNOWN_DEVICE_FEATURES
#error "Device not recognised, so pins with support for PWM not known. Please specify in parameters with #define digitalPinHasPWM(p) (p==...)"
#endif
#endif
#endif

#ifdef A20
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18 || p==A19 || p==A20)
#elif defined(A19)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18 || p==A19)
#elif defined(A18)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18)
#elif defined(A17)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17)
#elif defined(A16)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16)
#elif defined(A15)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15)
#elif defined(A14)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14)
#elif defined(A13)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13)
#elif defined(A12)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12)
#elif defined(A11)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11)
#elif defined(A10)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10)
#elif defined(A9)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9)
#elif defined(A8)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8)
#elif defined(A7)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7)
#elif defined(A6)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6)
#elif defined(A5)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5)
#elif defined(A4)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4)
#elif defined(A3)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3)
#elif defined(A2)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2)
#elif defined(A1)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1)
#elif defined(A0)
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0)
#else
#define PIN_SUPPORTS_ANALOG_READ(p) (false)
#endif

#ifdef DAC5
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3 || p==DAC4 || p==DAC5)
#elif defined(DAC4)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3 || p==DAC4)
#elif defined(DAC3)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3)
#elif defined(DAC2)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2)
#elif defined(DAC1)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1)
#elif defined(DAC0)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0)
#else
#define PIN_SUPPORTS_ANALOG_WRITE(p) false
#endif
#ifdef DAC0
#define DEVICE_SUPPORTS_ANALOG_WRITE
#endif

namespace Zscript {
template<class ZP>
class PinManager {
    static uint8_t digitalPinsMode[(ZP::pinCount + 1) / 2];

public:

    static void setState(uint8_t pin, bool isModeActivated, bool isPullup, bool isCollectorNDrain) {
        digitalPinsMode[pin / 2] &= ~(0x7 << ((pin % 2) * 4));
        digitalPinsMode[pin / 2] |= ((isModeActivated ? 0x1 : 0) | (isPullup ? 0x2 : 0) | (isCollectorNDrain ? 0x4 : 0)) << ((pin % 2) * 4);
    }

    static bool isModeActivated(uint8_t pin) {
        return (digitalPinsMode[pin / 2] & (0x1 << ((pin % 2) * 4))) != 0;
    }

    static bool isPullup(uint8_t pin) {
        return (digitalPinsMode[pin / 2] & (0x2 << ((pin % 2) * 4))) != 0;
    }

    static bool isCollectorNDrain(uint8_t pin) {
        return (digitalPinsMode[pin / 2] & (0x4 << ((pin % 2) * 4))) != 0;
    }
};

template<class ZP>
uint8_t PinManager<ZP>::digitalPinsMode[(ZP::pinCount + 1) / 2];
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_PINMANAGER_HPP_ */

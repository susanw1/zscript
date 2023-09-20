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
#error "Device not recognised, so pins with support for PWM not known. Please specify in parameters with '#define digitalPinHasPWM(p) (p==...)', ignore with '#define IGNORE_UNKNOWN_DEVICE_FEATURES'"
#endif
#endif
#endif

#ifndef PIN_SUPPORTS_ANALOG_READ
#ifdef PIN_A21
#ifndef IGNORE_OVERRUN_ANALOG
#error More analog pins converters defined than the library can cope with, please specify '#define PIN_SUPPORTS_ANALOG_READ(p) (p==...)', '#define ANALOG_PIN_COUNT_FOUND x' and '#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), ...' (ignore with '#define IGNORE_OVERRUN_ANALOG')
#endif
#elif defined(PIN_A20)
#define ANALOG_PIN_COUNT_FOUND 21
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18 || p==A19 || p==A20)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15), name(A16), name(A17), name(A18), name(A19), \
        name(A20)
#elif defined(PIN_A19)
#define ANALOG_PIN_COUNT_FOUND 20
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18 || p==A19)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15), name(A16), name(A17), name(A18), name(A19)
#elif defined(PIN_A18)
#define ANALOG_PIN_COUNT_FOUND 19
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17 || p==A18)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15), name(A16), name(A17), name(A18)
#elif defined(PIN_A17)
#define ANALOG_PIN_COUNT_FOUND 18
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16 || p==A17)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15), name(A16), name(A17)
#elif defined(PIN_A16)
#define ANALOG_PIN_COUNT_FOUND 17
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15 || p==A16)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15), name(A16)
#elif defined(PIN_A15)
#define ANALOG_PIN_COUNT_FOUND 16
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14\
        p==A15)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14), name(A15)
#elif defined(PIN_A14)
#define ANALOG_PIN_COUNT_FOUND 15
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13 || p==A14)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13), name(A14)
#elif defined(PIN_A13)
#define ANALOG_PIN_COUNT_FOUND 14
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12 || p==A13)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12), name(A13)
#elif defined(PIN_A12)
#define ANALOG_PIN_COUNT_FOUND 13
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11 || p==A12)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11), \
        name(A12)
#elif defined(PIN_A11)
#define ANALOG_PIN_COUNT_FOUND 12
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10 || p==A11)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10), name(A11)
#elif defined(PIN_A10)
#define ANALOG_PIN_COUNT_FOUND 11
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9 || p==A10)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9), name(A10)
#elif defined(PIN_A9)
#define ANALOG_PIN_COUNT_FOUND 10
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8 || p==A9)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8), name(A9)
#elif defined(PIN_A8)
#define ANALOG_PIN_COUNT_FOUND 9
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7 || p==A8)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7), name(A8)
#elif defined(PIN_A7)
#define ANALOG_PIN_COUNT_FOUND 8
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6 || p==A7)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6), name(A7)
#elif defined(PIN_A6)
#define ANALOG_PIN_COUNT_FOUND 7
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5 || \
        p==A6)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5), name(A6)
#elif defined(PIN_A5)
#define ANALOG_PIN_COUNT_FOUND 6
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4 || p==A5)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4), name(A5)
#elif defined(PIN_A4)
#define ANALOG_PIN_COUNT_FOUND 5
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3 || p==A4)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3), \
        name(A4)
#elif defined(PIN_A3)
#define ANALOG_PIN_COUNT_FOUND 4
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2 || p==A3)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2), name(A3)
#elif defined(PIN_A2)
#define ANALOG_PIN_COUNT_FOUND 3
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1 || p==A2)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1), name(A2)
#elif defined(PIN_A1)
#define ANALOG_PIN_COUNT_FOUND 2
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0 || p==A1)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0), name(A1)
#elif defined(PIN_A0)
#define ANALOG_PIN_COUNT_FOUND 1
#define PIN_SUPPORTS_ANALOG_READ(p) (p==A0)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name) name(A0)
#else
#define ANALOG_PIN_COUNT_FOUND 0
#define PIN_SUPPORTS_ANALOG_READ(p) (false)
#define INITIALISE_ANALOG_NOTIFICATION_SYSTEM(name)
#endif
#endif

#ifndef PIN_SUPPORTS_ANALOG_WRITE
#ifdef PIN_DAC6
#ifndef IGNORE_OVERRUN_DAC
#error More DtoA converters defined than the library can cope with, please specify '#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==...)'  (ignore with '#define IGNORE_OVERRUN_DAC')
#endif
#elif defined(PIN_DAC5)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3 || p==DAC4 || p==DAC5)
#elif defined(PIN_DAC4)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3 || p==DAC4)
#elif defined(PIN_DAC3)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2 || p==DAC3)
#elif defined(PIN_DAC2)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1 || p==DAC2)
#elif defined(PIN_DAC1)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0 || p==DAC1)
#elif defined(PIN_DAC0)
#define PIN_SUPPORTS_ANALOG_WRITE(p) (p==DAC0)
#else
#define PIN_SUPPORTS_ANALOG_WRITE(p) false
#endif
#ifdef PIN_DAC0
#define DEVICE_SUPPORTS_ANALOG_WRITE
#endif
#endif


namespace Zscript {

enum class PinNotificationType : uint8_t {
    NONE,
    ON_HIGH,
    ON_LOW,
    ON_RISING,
    ON_FALLING,
    ON_CHANGE
};

template<class ZP>
class PinManager {

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

    static void pinInterrupt_00() {
        pinInterrupt(0);
    }

    static void pinInterrupt_01() {
        pinInterrupt(1);
    }

    static void pinInterrupt_02() {
        pinInterrupt(2);
    }

    static void pinInterrupt_03() {
        pinInterrupt(3);
    }

    static void pinInterrupt_04() {
        pinInterrupt(4);
    }

    static void pinInterrupt_05() {
        pinInterrupt(5);
    }

    static void pinInterrupt_06() {
        pinInterrupt(6);
    }

    static void pinInterrupt_07() {
        pinInterrupt(7);
    }

    static void pinInterrupt_08() {
        pinInterrupt(8);
    }

    static void pinInterrupt_09() {
        pinInterrupt(9);
    }

    static void pinInterrupt_10() {
        pinInterrupt(10);
    }

    static void pinInterrupt_11() {
        pinInterrupt(11);
    }

    static void pinInterrupt_12() {
        pinInterrupt(12);
    }

    static void pinInterrupt_13() {
        pinInterrupt(13);
    }

    static void pinInterrupt_14() {
        pinInterrupt(14);
    }

    static void pinInterrupt_15() {
        pinInterrupt(15);
    }

    static void pinInterrupt_16() {
        pinInterrupt(16);
    }

    static void pinInterrupt_17() {
        pinInterrupt(17);
    }

    static void pinInterrupt_18() {
        pinInterrupt(18);
    }

    static void pinInterrupt_19() {
        pinInterrupt(19);
    }

    static void pinInterrupt_20() {
        pinInterrupt(20);
    }

    static void pinInterrupt_21() {
        pinInterrupt(21);
    }

    static void pinInterrupt_22() {
        pinInterrupt(22);
    }

    static void pinInterrupt_23() {
        pinInterrupt(23);
    }

    static void pinInterrupt_24() {
        pinInterrupt(24);
    }

    static void pinInterrupt_25() {
        pinInterrupt(25);
    }

    static void pinInterrupt_26() {
        pinInterrupt(26);
    }

    static void pinInterrupt_27() {
        pinInterrupt(27);
    }

    static void pinInterrupt_28() {
        pinInterrupt(28);
    }

    static void pinInterrupt_29() {
        pinInterrupt(29);
    }

    static void pinInterrupt_30() {
        pinInterrupt(30);
    }

    static void pinInterrupt_31() {
        pinInterrupt(31);
    }

    static void pinInterrupt_32() {
        pinInterrupt(32);
    }

    static void pinInterrupt_33() {
        pinInterrupt(33);
    }

    static void pinInterrupt_34() {
        pinInterrupt(34);
    }

    static void pinInterrupt_35() {
        pinInterrupt(35);
    }

    static void pinInterrupt_36() {
        pinInterrupt(36);
    }

    static void pinInterrupt_37() {
        pinInterrupt(37);
    }

    static void pinInterrupt_38() {
        pinInterrupt(38);
    }

    static void pinInterrupt_39() {
        pinInterrupt(39);
    }

    static void pinInterrupt_40() {
        pinInterrupt(40);
    }

    static void pinInterrupt_41() {
        pinInterrupt(41);
    }

    static void pinInterrupt_42() {
        pinInterrupt(42);
    }

    static void pinInterrupt_43() {
        pinInterrupt(43);
    }

    static void pinInterrupt_44() {
        pinInterrupt(44);
    }

    static void pinInterrupt_45() {
        pinInterrupt(45);
    }

    static void pinInterrupt_46() {
        pinInterrupt(46);
    }

    static void pinInterrupt_47() {
        pinInterrupt(47);
    }

    static void pinInterrupt_48() {
        pinInterrupt(48);
    }

    static void pinInterrupt_49() {
        pinInterrupt(49);
    }

    static void pinInterrupt_50() {
        pinInterrupt(50);
    }

    static void pinInterrupt_51() {
        pinInterrupt(51);
    }

    static void pinInterrupt_52() {
        pinInterrupt(52);
    }

    static void pinInterrupt_53() {
        pinInterrupt(53);
    }

    static void pinInterrupt_54() {
        pinInterrupt(54);
    }

    static void pinInterrupt_55() {
        pinInterrupt(55);
    }

    static void pinInterrupt_56() {
        pinInterrupt(56);
    }

    static void pinInterrupt_57() {
        pinInterrupt(57);
    }

    static void pinInterrupt_58() {
        pinInterrupt(58);
    }

    static void pinInterrupt_59() {
        pinInterrupt(59);
    }

    static void pinInterrupt_60() {
        pinInterrupt(60);
    }

    static void pinInterrupt_61() {
        pinInterrupt(61);
    }

    static void pinInterrupt_62() {
        pinInterrupt(62);
    }

    static void pinInterrupt_63() {
        pinInterrupt(63);
    }

    static void pinInterrupt_64() {
        pinInterrupt(64);
    }

    static void pinInterrupt_65() {
        pinInterrupt(65);
    }

    static void pinInterrupt_66() {
        pinInterrupt(66);
    }

    static void pinInterrupt_67() {
        pinInterrupt(67);
    }

    static void pinInterrupt_68() {
        pinInterrupt(68);
    }

    static void pinInterrupt_69() {
        pinInterrupt(69);
    }

    static void pinInterrupt_70() {
        pinInterrupt(70);
    }

    static void pinInterrupt_71() {
        pinInterrupt(71);
    }

    static void pinInterrupt_72() {
        pinInterrupt(72);
    }

    static void pinInterrupt_73() {
        pinInterrupt(73);
    }

    static void pinInterrupt_74() {
        pinInterrupt(74);
    }

    static void pinInterrupt_75() {
        pinInterrupt(75);
    }

    static void pinInterrupt_76() {
        pinInterrupt(76);
    }

    static void pinInterrupt_77() {
        pinInterrupt(77);
    }

    static void pinInterrupt_78() {
        pinInterrupt(78);
    }

    static void pinInterrupt_79() {
        pinInterrupt(79);
    }

    static void pinInterrupt_80() {
        pinInterrupt(80);
    }

    static void pinInterrupt_81() {
        pinInterrupt(81);
    }

    static void pinInterrupt_82() {
        pinInterrupt(82);
    }

    static void pinInterrupt_83() {
        pinInterrupt(83);
    }

    static void pinInterrupt_84() {
        pinInterrupt(84);
    }

    static void pinInterrupt_85() {
        pinInterrupt(85);
    }

    static void pinInterrupt_86() {
        pinInterrupt(86);
    }

    static void pinInterrupt_87() {
        pinInterrupt(87);
    }

    static void pinInterrupt_88() {
        pinInterrupt(88);
    }

    static void pinInterrupt_89() {
        pinInterrupt(89);
    }

    static void pinInterrupt_90() {
        pinInterrupt(90);
    }

    static void pinInterrupt_91() {
        pinInterrupt(91);
    }

    static void pinInterrupt_92() {
        pinInterrupt(92);
    }

    static void pinInterrupt_93() {
        pinInterrupt(93);
    }

    static void pinInterrupt_94() {
        pinInterrupt(94);
    }

    static void pinInterrupt_95() {
        pinInterrupt(95);
    }

    static void pinInterrupt_96() {
        pinInterrupt(96);
    }

    static void pinInterrupt_97() {
        pinInterrupt(97);
    }

    static void pinInterrupt_98() {
        pinInterrupt(98);
    }

    static void pinInterrupt_99() {
        pinInterrupt(99);
    }

    static void setInterrupt(uint8_t pin, int mode) {
        switch (pin) {
            case 0:
                if (digitalPinToInterrupt(0) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(0), &PinManager<ZP>::pinInterrupt_00, mode);
                }
                break;
            case 1:
                if (digitalPinToInterrupt(1) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(1), &PinManager<ZP>::pinInterrupt_01, mode);
                }
                break;
            case 2:
                if (digitalPinToInterrupt(2) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(2), &PinManager<ZP>::pinInterrupt_02, mode);
                }
                break;
            case 3:
                if (digitalPinToInterrupt(3) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(3), &PinManager<ZP>::pinInterrupt_03, mode);
                }
                break;
            case 4:
                if (digitalPinToInterrupt(4) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(4), &PinManager<ZP>::pinInterrupt_04, mode);
                }
                break;
            case 5:
                if (digitalPinToInterrupt(5) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(5), &PinManager<ZP>::pinInterrupt_05, mode);
                }
                break;
            case 6:
                if (digitalPinToInterrupt(6) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(6), &PinManager<ZP>::pinInterrupt_06, mode);
                }
                break;
            case 07:
                if (digitalPinToInterrupt(07) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(7), &PinManager<ZP>::pinInterrupt_07, mode);
                }
                break;
            case 8:
                if (digitalPinToInterrupt(8) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(8), &PinManager<ZP>::pinInterrupt_08, mode);
                }
                break;
            case 9:
                if (digitalPinToInterrupt(9) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(9), &PinManager<ZP>::pinInterrupt_09, mode);
                }
                break;


            case 10:
                if (digitalPinToInterrupt(10) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(10), &PinManager<ZP>::pinInterrupt_10, mode);
                }
                break;
            case 11:
                if (digitalPinToInterrupt(11) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(11), &PinManager<ZP>::pinInterrupt_11, mode);
                }
                break;
            case 12:
                if (digitalPinToInterrupt(12) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(12), &PinManager<ZP>::pinInterrupt_12, mode);
                }
                break;
            case 13:
                if (digitalPinToInterrupt(13) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(13), &PinManager<ZP>::pinInterrupt_13, mode);
                }
                break;
            case 14:
                if (digitalPinToInterrupt(14) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(14), &PinManager<ZP>::pinInterrupt_14, mode);
                }
                break;
            case 15:
                if (digitalPinToInterrupt(15) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(15), &PinManager<ZP>::pinInterrupt_15, mode);
                }
                break;
            case 16:
                if (digitalPinToInterrupt(16) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(16), &PinManager<ZP>::pinInterrupt_16, mode);
                }
                break;
            case 17:
                if (digitalPinToInterrupt(17) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(17), &PinManager<ZP>::pinInterrupt_17, mode);
                }
                break;
            case 18:
                if (digitalPinToInterrupt(18) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(18), &PinManager<ZP>::pinInterrupt_18, mode);
                }
                break;
            case 19:
                if (digitalPinToInterrupt(19) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(19), &PinManager<ZP>::pinInterrupt_19, mode);
                }
                break;


            case 20:
                if (digitalPinToInterrupt(20) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(20), &PinManager<ZP>::pinInterrupt_20, mode);
                }
                break;
            case 21:
                if (digitalPinToInterrupt(21) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(21), &PinManager<ZP>::pinInterrupt_21, mode);
                }
                break;
            case 22:
                if (digitalPinToInterrupt(22) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(22), &PinManager<ZP>::pinInterrupt_22, mode);
                }
                break;
            case 23:
                if (digitalPinToInterrupt(23) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(23), &PinManager<ZP>::pinInterrupt_23, mode);
                }
                break;
            case 24:
                if (digitalPinToInterrupt(24) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(24), &PinManager<ZP>::pinInterrupt_24, mode);
                }
                break;
            case 25:
                if (digitalPinToInterrupt(25) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(25), &PinManager<ZP>::pinInterrupt_25, mode);
                }
                break;
            case 26:
                if (digitalPinToInterrupt(26) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(26), &PinManager<ZP>::pinInterrupt_26, mode);
                }
                break;
            case 27:
                if (digitalPinToInterrupt(27) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(27), &PinManager<ZP>::pinInterrupt_27, mode);
                }
                break;
            case 28:
                if (digitalPinToInterrupt(28) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(28), &PinManager<ZP>::pinInterrupt_28, mode);
                }
                break;
            case 29:
                if (digitalPinToInterrupt(29) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(29), &PinManager<ZP>::pinInterrupt_29, mode);
                }
                break;


            case 30:
                if (digitalPinToInterrupt(30) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(30), &PinManager<ZP>::pinInterrupt_30, mode);
                }
                break;
            case 31:
                if (digitalPinToInterrupt(31) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(31), &PinManager<ZP>::pinInterrupt_31, mode);
                }
                break;
            case 32:
                if (digitalPinToInterrupt(32) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(32), &PinManager<ZP>::pinInterrupt_32, mode);
                }
                break;
            case 33:
                if (digitalPinToInterrupt(33) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(33), &PinManager<ZP>::pinInterrupt_33, mode);
                }
                break;
            case 34:
                if (digitalPinToInterrupt(34) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(34), &PinManager<ZP>::pinInterrupt_34, mode);
                }
                break;
            case 35:
                if (digitalPinToInterrupt(35) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(35), &PinManager<ZP>::pinInterrupt_35, mode);
                }
                break;
            case 36:
                if (digitalPinToInterrupt(36) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(36), &PinManager<ZP>::pinInterrupt_36, mode);
                }
                break;
            case 37:
                if (digitalPinToInterrupt(37) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(37), &PinManager<ZP>::pinInterrupt_37, mode);
                }
                break;
            case 38:
                if (digitalPinToInterrupt(38) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(38), &PinManager<ZP>::pinInterrupt_38, mode);
                }
                break;
            case 39:
                if (digitalPinToInterrupt(39) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(39), &PinManager<ZP>::pinInterrupt_39, mode);
                }
                break;


            case 40:
                if (digitalPinToInterrupt(40) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(40), &PinManager<ZP>::pinInterrupt_40, mode);
                }
                break;
            case 41:
                if (digitalPinToInterrupt(41) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(41), &PinManager<ZP>::pinInterrupt_41, mode);
                }
                break;
            case 42:
                if (digitalPinToInterrupt(42) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(42), &PinManager<ZP>::pinInterrupt_42, mode);
                }
                break;
            case 43:
                if (digitalPinToInterrupt(43) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(43), &PinManager<ZP>::pinInterrupt_43, mode);
                }
                break;
            case 44:
                if (digitalPinToInterrupt(44) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(44), &PinManager<ZP>::pinInterrupt_44, mode);
                }
                break;
            case 45:
                if (digitalPinToInterrupt(45) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(45), &PinManager<ZP>::pinInterrupt_45, mode);
                }
                break;
            case 46:
                if (digitalPinToInterrupt(46) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(46), &PinManager<ZP>::pinInterrupt_46, mode);
                }
                break;
            case 47:
                if (digitalPinToInterrupt(47) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(47), &PinManager<ZP>::pinInterrupt_47, mode);
                }
                break;
            case 48:
                if (digitalPinToInterrupt(48) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(48), &PinManager<ZP>::pinInterrupt_48, mode);
                }
                break;
            case 49:
                if (digitalPinToInterrupt(49) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(49), &PinManager<ZP>::pinInterrupt_49, mode);
                }
                break;


            case 50:
                if (digitalPinToInterrupt(50) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(50), &PinManager<ZP>::pinInterrupt_50, mode);
                }
                break;
            case 51:
                if (digitalPinToInterrupt(51) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(51), &PinManager<ZP>::pinInterrupt_51, mode);
                }
                break;
            case 52:
                if (digitalPinToInterrupt(52) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(52), &PinManager<ZP>::pinInterrupt_52, mode);
                }
                break;
            case 53:
                if (digitalPinToInterrupt(53) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(53), &PinManager<ZP>::pinInterrupt_53, mode);
                }
                break;
            case 54:
                if (digitalPinToInterrupt(54) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(54), &PinManager<ZP>::pinInterrupt_54, mode);
                }
                break;
            case 55:
                if (digitalPinToInterrupt(55) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(55), &PinManager<ZP>::pinInterrupt_55, mode);
                }
                break;
            case 56:
                if (digitalPinToInterrupt(56) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(56), &PinManager<ZP>::pinInterrupt_56, mode);
                }
                break;
            case 57:
                if (digitalPinToInterrupt(57) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(57), &PinManager<ZP>::pinInterrupt_57, mode);
                }
                break;
            case 58:
                if (digitalPinToInterrupt(58) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(58), &PinManager<ZP>::pinInterrupt_58, mode);
                }
                break;
            case 59:
                if (digitalPinToInterrupt(59) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(59), &PinManager<ZP>::pinInterrupt_59, mode);
                }
                break;


            case 60:
                if (digitalPinToInterrupt(60) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(60), &PinManager<ZP>::pinInterrupt_60, mode);
                }
                break;
            case 61:
                if (digitalPinToInterrupt(61) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(61), &PinManager<ZP>::pinInterrupt_61, mode);
                }
                break;
            case 62:
                if (digitalPinToInterrupt(62) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(62), &PinManager<ZP>::pinInterrupt_62, mode);
                }
                break;
            case 63:
                if (digitalPinToInterrupt(63) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(63), &PinManager<ZP>::pinInterrupt_63, mode);
                }
                break;
            case 64:
                if (digitalPinToInterrupt(64) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(64), &PinManager<ZP>::pinInterrupt_64, mode);
                }
                break;
            case 65:
                if (digitalPinToInterrupt(65) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(65), &PinManager<ZP>::pinInterrupt_65, mode);
                }
                break;
            case 66:
                if (digitalPinToInterrupt(66) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(66), &PinManager<ZP>::pinInterrupt_66, mode);
                }
                break;
            case 67:
                if (digitalPinToInterrupt(67) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(67), &PinManager<ZP>::pinInterrupt_67, mode);
                }
                break;
            case 68:
                if (digitalPinToInterrupt(68) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(68), &PinManager<ZP>::pinInterrupt_68, mode);
                }
                break;
            case 69:
                if (digitalPinToInterrupt(69) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(69), &PinManager<ZP>::pinInterrupt_69, mode);
                }
                break;


            case 70:
                if (digitalPinToInterrupt(70) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(70), &PinManager<ZP>::pinInterrupt_70, mode);
                }
                break;
            case 71:
                if (digitalPinToInterrupt(71) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(71), &PinManager<ZP>::pinInterrupt_71, mode);
                }
                break;
            case 72:
                if (digitalPinToInterrupt(72) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(72), &PinManager<ZP>::pinInterrupt_72, mode);
                }
                break;
            case 73:
                if (digitalPinToInterrupt(73) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(73), &PinManager<ZP>::pinInterrupt_73, mode);
                }
                break;
            case 74:
                if (digitalPinToInterrupt(74) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(74), &PinManager<ZP>::pinInterrupt_74, mode);
                }
                break;
            case 75:
                if (digitalPinToInterrupt(75) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(75), &PinManager<ZP>::pinInterrupt_75, mode);
                }
                break;
            case 76:
                if (digitalPinToInterrupt(76) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(76), &PinManager<ZP>::pinInterrupt_76, mode);
                }
                break;
            case 77:
                if (digitalPinToInterrupt(77) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(77), &PinManager<ZP>::pinInterrupt_77, mode);
                }
                break;
            case 78:
                if (digitalPinToInterrupt(78) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(78), &PinManager<ZP>::pinInterrupt_78, mode);
                }
                break;
            case 79:
                if (digitalPinToInterrupt(79) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(79), &PinManager<ZP>::pinInterrupt_79, mode);
                }
                break;


            case 80:
                if (digitalPinToInterrupt(80) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(80), &PinManager<ZP>::pinInterrupt_80, mode);
                }
                break;
            case 81:
                if (digitalPinToInterrupt(81) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(81), &PinManager<ZP>::pinInterrupt_81, mode);
                }
                break;
            case 82:
                if (digitalPinToInterrupt(82) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(82), &PinManager<ZP>::pinInterrupt_82, mode);
                }
                break;
            case 83:
                if (digitalPinToInterrupt(83) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(83), &PinManager<ZP>::pinInterrupt_83, mode);
                }
                break;
            case 84:
                if (digitalPinToInterrupt(84) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(84), &PinManager<ZP>::pinInterrupt_84, mode);
                }
                break;
            case 85:
                if (digitalPinToInterrupt(85) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(85), &PinManager<ZP>::pinInterrupt_85, mode);
                }
                break;
            case 86:
                if (digitalPinToInterrupt(86) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(86), &PinManager<ZP>::pinInterrupt_86, mode);
                }
                break;
            case 87:
                if (digitalPinToInterrupt(87) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(87), &PinManager<ZP>::pinInterrupt_87, mode);
                }
                break;
            case 88:
                if (digitalPinToInterrupt(88) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(88), &PinManager<ZP>::pinInterrupt_88, mode);
                }
                break;
            case 89:
                if (digitalPinToInterrupt(89) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(89), &PinManager<ZP>::pinInterrupt_89, mode);
                }
                break;


            case 90:
                if (digitalPinToInterrupt(90) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(90), &PinManager<ZP>::pinInterrupt_90, mode);
                }
                break;
            case 91:
                if (digitalPinToInterrupt(91) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(91), &PinManager<ZP>::pinInterrupt_91, mode);
                }
                break;
            case 92:
                if (digitalPinToInterrupt(92) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(92), &PinManager<ZP>::pinInterrupt_92, mode);
                }
                break;
            case 93:
                if (digitalPinToInterrupt(93) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(93), &PinManager<ZP>::pinInterrupt_93, mode);
                }
                break;
            case 94:
                if (digitalPinToInterrupt(94) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(94), &PinManager<ZP>::pinInterrupt_94, mode);
                }
                break;
            case 95:
                if (digitalPinToInterrupt(95) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(95), &PinManager<ZP>::pinInterrupt_95, mode);
                }
                break;
            case 96:
                if (digitalPinToInterrupt(96) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(96), &PinManager<ZP>::pinInterrupt_96, mode);
                }
                break;
            case 97:
                if (digitalPinToInterrupt(97) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(97), &PinManager<ZP>::pinInterrupt_97, mode);
                }
                break;
            case 98:
                if (digitalPinToInterrupt(98) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(98), &PinManager<ZP>::pinInterrupt_98, mode);
                }
                break;
            case 99:
                if (digitalPinToInterrupt(99) != NOT_AN_INTERRUPT) {
                    attachInterrupt(digitalPinToInterrupt(99), &PinManager<ZP>::pinInterrupt_99, mode);
                }
                break;

            default:
                return;
        }
    }

    static void pinInterrupt(uint8_t pinNumber) {
        notificationHappened(pinNumber);
        onNotificationDigital();
    }

#endif

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

    struct AnalogPinNotificationSetup {
        bool notifState: 1;
        bool notif: 1;
        bool enabled: 1;
        uint8_t pinNum: 8;
        uint16_t maxVal: 12;
        uint16_t minVal: 12;
        uint16_t last: 12;
    public:
        AnalogPinNotificationSetup(uint8_t pin) : notifState(false), notif(false), enabled(false), pinNum(pin), maxVal(0xfff),
                                       minVal(0x000), last(0x800) {

        }
    };

    static AnalogPinNotificationSetup analogPinNotifs[ANALOG_PIN_COUNT_FOUND];
#endif
    static uint8_t digitalPinsMode[ZP::pinCount];

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

    static void (*onNotificationDigital)(void);

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

    static void (*onNotificationAnalog)(void);

#endif

    // the 'notification state' is to ensure that repeated HIGH or LOW notifications aren't sent
    // instead waiting for the condition to end and begin again to send the notification.

    static bool isInNotificationState(uint8_t pin) {
        return (digitalPinsMode[pin] & (0x1 << 6)) != 0;
    }

    static void clearInNotificationState(uint8_t pin) {
        digitalPinsMode[pin] &= ~0x40;
    }

    static void notificationHappened(uint8_t pin) {
        digitalPinsMode[pin] |= 0xC0;
    }

    static bool checkPinNotificationState(uint8_t pin, PinNotificationType state) {
        return (digitalRead(pin) == HIGH) == (state == PinNotificationType::ON_HIGH);
    }

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

    static bool readCheckAnalog(uint8_t i) {
        analogPinNotifs[i].last = analogRead(analogPinNotifs[i].pinNum) << 2;
        if (analogPinNotifs[i].last < analogPinNotifs[i].minVal ||
            analogPinNotifs[i].last > analogPinNotifs[i].maxVal) {
            return true;
        }
        return false;
    }

#endif

#endif

public:

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS


    static void setOnNotificationDigital(void (*onNotificationDigital)(void)) {
        PinManager<ZP>::onNotificationDigital = onNotificationDigital;
    }

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

    static void setOnNotificationAnalog(void (*onNotificationAnalog)(void)) {
        PinManager<ZP>::onNotificationAnalog = onNotificationAnalog;
    }

#endif

    static void pollPinStates() {
        for (uint8_t pin = 0; pin < ZP::pinCount; pin++) {
            PinNotificationType value = getNotificationType(pin);
            if (value == PinNotificationType::ON_HIGH || value == PinNotificationType::ON_LOW) {
                if (isInNotificationState(pin)) {
                    if (!hasNotification(pin)) {
                        if (!checkPinNotificationState(pin, value)) {
                            // if we have been in the notification state, but the notification has been sent and the condition no longer applies, exit the state
                            clearInNotificationState(pin);
                        }
                    }
                } else if (checkPinNotificationState(pin, value)) {
                    // if we have not been in the notification state, but the condition applies, enter the notification state
                    notificationHappened(pin);
                    onNotificationDigital();
                }
            }
        }
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        for (uint8_t i = 0; i < ANALOG_PIN_COUNT_FOUND; i++) {
            if (analogPinNotifs[i].enabled) {
                if (analogPinNotifs[i].notifState) {
                    if (!analogPinNotifs[i].notif) {
                        if (!readCheckAnalog(i)) {
                            analogPinNotifs[i].notifState = false;
                        }
                    }
                } else {
                    if (readCheckAnalog(i)) {
                        analogPinNotifs[i].notifState = true;
                        analogPinNotifs[i].notif = true;
                        onNotificationAnalog();
                    }
                }
            }
        }
#endif
    }

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

    static uint8_t getAnalogIndex(uint8_t pin) {
        for (uint8_t i = 0; i < ANALOG_PIN_COUNT_FOUND; i++) {
            if (pin == analogPinNotifs[i].pinNum) {
                return i;
            }
        }
        return 0xFF;
    }

    static void enableAnalogNotification(uint8_t index) {
        if (index != 0xFF) {
            analogPinNotifs[index].enabled = true;
        }
    }

    static void setAnalogNotificationLower(uint8_t index, uint16_t min) {
        if (index != 0xFF) {
            analogPinNotifs[index].minVal = min >> 4;
        }
    }

    static void setAnalogNotificationUpper(uint8_t index, uint16_t max) {
        if (index != 0xFF) {
            analogPinNotifs[index].maxVal = max >> 4;
        }
    }

    static void disableAnalogNotification(uint8_t index) {
        if (index != 0xFF) {
            analogPinNotifs[index].enabled = false;
        }
    }

    static void clearAnalogNotification(uint8_t index) {
        if (index != 0xFF) {
            analogPinNotifs[index].notif = false;
        }
    }

    static uint8_t getAnalogPin(uint8_t index) {
        if (index != 0xFF) {
            return analogPinNotifs[index].pinNum;
        }
        return analogPinNotifs[0].pinNum;
    }

    static bool wasAnalogTooHigh(uint8_t index) {
        if (index != 0xFF) {
            return analogPinNotifs[index].last > analogPinNotifs[index].maxVal;
        }
        return false;
    }

    static uint16_t getLastAnalog(uint8_t index) {
        if (index != 0xFF) {
            return analogPinNotifs[index].last << 4;
        }
        return 0;
    }

    static bool hasAnalogNotification(uint8_t index) {
        if (index != 0xFF) {
            return analogPinNotifs[index].notif;
        }
        return false;
    }

#endif

    static void clearNotification(uint8_t pin) {
        digitalPinsMode[pin] &= ~0x80;
    }

    static bool hasNotification(uint8_t pin) {
        return (digitalPinsMode[pin] & 0x80) != 0;
    }

    static PinNotificationType getNotificationType(uint8_t pin) {
        return (PinNotificationType) ((digitalPinsMode[pin] & (0x7 << 3)) >> 3);
    }

    static void setNotificationType(uint8_t pin, PinNotificationType notifType) {
        digitalPinsMode[pin] &= ~(0x7 << 3);
        digitalPinsMode[pin] |= (uint8_t)(notifType) << 3;
        switch (notifType) {
            case PinNotificationType::ON_RISING:
                setInterrupt(pin, RISING);
                break;
            case PinNotificationType::ON_FALLING:
                setInterrupt(pin, FALLING);
                break;
            case PinNotificationType::ON_CHANGE:
                setInterrupt(pin, CHANGE);
                break;
            default:
                if (digitalPinToInterrupt(pin) != NOT_AN_INTERRUPT) {
                    detachInterrupt(digitalPinToInterrupt(pin));
                }
                break;
        }
    }

    static bool supportsInterruptNotifications(uint8_t pin) {
        return digitalPinToInterrupt(pin) != NOT_AN_INTERRUPT;
    }

#endif

    static void setState(uint8_t pin, bool isModeActivated, bool isPullup, bool isCollectorNDrain) {
        digitalPinsMode[pin] &= ~0x7;
        digitalPinsMode[pin] |= (isModeActivated ? 0x1 : 0) | (isPullup ? 0x2 : 0) | (isCollectorNDrain ? 0x4 : 0);
    }


    static bool isModeActivated(uint8_t pin) {
        return (digitalPinsMode[pin] & 0x1) != 0;
    }

    static bool isPullup(uint8_t pin) {
        return (digitalPinsMode[pin] & 0x2) != 0;
    }

    static bool isCollectorNDrain(uint8_t pin) {
        return (digitalPinsMode[pin] & 0x4) != 0;
    }
};

template<class ZP>
uint8_t PinManager<ZP>::digitalPinsMode[ZP::pinCount];

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

template<class ZP>
void (*PinManager<ZP>::onNotificationDigital)(void);

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

template<class ZP>
typename PinManager<ZP>::AnalogPinNotificationSetup PinManager<ZP>::analogPinNotifs[ANALOG_PIN_COUNT_FOUND] = {INITIALISE_ANALOG_NOTIFICATION_SYSTEM(AnalogPinNotificationSetup)};

template<class ZP>
void (*PinManager<ZP>::onNotificationAnalog)(void);

#endif
#endif
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_PINMANAGER_HPP_ */

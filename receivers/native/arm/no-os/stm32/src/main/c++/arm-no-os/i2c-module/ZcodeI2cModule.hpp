/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ZCODEI2CMODULE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ZCODEI2CMODULE_HPP_

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>
#ifdef I2C_ADDRESSING
#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#endif

#include "commands/ZcodeI2cSetupCommand.hpp"
#include "commands/ZcodeI2cSendCommand.hpp"
#include "commands/ZcodeI2cReceiveCommand.hpp"
#include "commands/ZcodeI2cSendReceiveCommand.hpp"
#include "commands/ZcodeI2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(ZcodeI2cModule<ZP>::execute)

template<class ZP>
class ZcodeI2cModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

#ifdef I2C_ADDRESSING
    static uint8_t addressed[(256 * HW::i2cCount) / 8];
#endif

public:

#ifdef I2C_ADDRESSING
    typedef ZcodeI2cBusInterruptSource<ZP> busInterruptSource;

    static void addressI2c(I2cIdentifier id, uint16_t address) {
        if (address < 0x100) {
            addressed[id * 256 / 8 + address / 8] |= (1 << (address & 0x3));
        }
    }
    static void unaddressI2c(I2cIdentifier id, uint16_t address) {
        if (address < 0x100) {
            addressed[id * 256 / 8 + address / 8] &= ~(1 << (address & 0x3));
        }
    }
    static bool isAddressed(I2cIdentifier id, uint16_t address) {
        if (address < 0x100) {
            return (addressed[id * 256 / 8 + address / 8] & (1 << (address & 0x3))) != 0;
        } else {
            return false;
        }
    }
#else
    static bool isAddressed(I2cIdentifier id, uint16_t address) {
        return false;
    }
#endif

    static void init() {
        I2cManager<LL>::init();
    }

    static I2c<LL>* getI2c(I2cIdentifier id) {
        return I2cManager<LL>::getI2cById(id);
    }

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZcodeI2cCapabilitiesCommand<ZP>::CODE:
            ZcodeI2cCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSetupCommand<ZP>::CODE:
            ZcodeI2cSetupCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendCommand<ZP>::CODE:
            ZcodeI2cSendCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cReceiveCommand<ZP>::CODE:
            ZcodeI2cReceiveCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendReceiveCommand<ZP>::CODE:
            ZcodeI2cSendReceiveCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};
template<class ZP>
uint8_t ZcodeI2cModule<ZP>::addressed[(256 * ZcodeI2cModule<ZP>::HW::i2cCount) / 8];

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_ZCODEI2CMODULE_HPP_ */

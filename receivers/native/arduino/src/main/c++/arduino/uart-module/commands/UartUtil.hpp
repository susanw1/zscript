/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTUTIL_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTUTIL_HPP_


namespace Zscript {

template<class ZP>
class UartUtil {
public:
    static void writeFrequencySelection(CommandOutStream<ZP> out, uint8_t freqIndex) {
        const uint32_t maxBaud = ZP::uartSupportedFreqs[freqIndex];
        uint8_t maxBaudBytes[4]{(maxBaud >> 24) & 0xff, (maxBaud >> 16) & 0xff, (maxBaud >> 8) & 0xff, (maxBaud) & 0xff};
        out.writeBigHex(maxBaudBytes, 4);
    }

};

}

#endif /* SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTUTIL_HPP_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_
#define SRC_MAIN_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_

#include "../channels/ZcodeChannelInStream.hpp"
#include "../ZcodeIncludes.hpp"
#include "ZcodeBigField.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeParser {

public:

    static void parseNext() {
        uint16_t count = 0;
        for (uint8_t i = 0; i < Zcode<ZP>::zcode.getChannelCount(); i++) {
            ZcodeChannelInStream<ZP> *in = Zcode<ZP>::zcode.getChannel(i)->in;
            in->getSlot()->clearHasData();
            while (in->pushData() && count++ < ZP::parseIterationMax)
                ;
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_ */

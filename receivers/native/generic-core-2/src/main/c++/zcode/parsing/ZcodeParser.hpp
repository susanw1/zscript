/*
 * ZcodeParser.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_
#include "../channels/ZcodeChannelInStream.hpp"
#include "../ZcodeIncludes.hpp"
#include "ZcodeBigField.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeParser {

public:

    static void parseNext(Zcode<ZP> *zcode) {
        uint16_t count = 0;
        for (uint8_t i = 0; i < zcode->getChannelCount(); i++) {
            ZcodeChannelInStream<ZP> *in = zcode->getChannel(i)->in;
            in->getSlot()->clearHasData();
            while (in->pushData() && count++ < ZP::parseIterationMax)
                ;
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_ */

/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_C___ZSCRIPT_SCRIPTSPACE_STRINGCHANNEL_HPP_
#define SRC_TEST_C___ZSCRIPT_SCRIPTSPACE_STRINGCHANNEL_HPP_

#include "zscript/ZscriptIncludes.hpp"
#include "zscript/ZscriptChannel.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class StringChannel: public ZscriptChannel<ZP> {
    uint8_t data[256];
    TokenRingBuffer<ZP> buffer;
    ZscriptTokenizer<ZP> tokenizer;
    const char *text;
    uint16_t textInd = 0;

public:
    StringChannel(AbstractOutStream<ZP> *out, const char *text) :
            ZscriptChannel<ZP>(out, &buffer, 0, true), buffer(data, 256), tokenizer(&buffer, 2), text(text) {
    }
    void moveAlong() {
        while (text[textInd] != 0 && tokenizer.offer(text[textInd])) {
            textInd++;
        }
    }

};
}
}

#endif /* SRC_TEST_C___ZSCRIPT_SCRIPTSPACE_STRINGCHANNEL_HPP_ */

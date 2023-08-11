/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_BUFFEROUTSTREAM_HPP_
#define SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_BUFFEROUTSTREAM_HPP_
#include "../../../../main/c++/zscript/AbstractOutStream.hpp"
#include "../../../../main/c++/zscript/ZscriptIncludes.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class BufferOutStream: public AbstractOutStream<ZP> {
    uint8_t data[1024];
    uint16_t index = 0;

public:
    bool openB = false;

    void open() {
        openB = true;
    }

    void close() {
        openB = false;
    }

    bool isOpen() {
        return openB;
    }

    DataArrayWLeng16 getData() {
        return {data, index};
    }

    void reset() {
        index = 0;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                data[index++] = AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4);
                data[index++] = AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                data[index++] = bytes[i];
            }
        }
    }

};
}
}

#endif /* SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_BUFFEROUTSTREAM_HPP_ */

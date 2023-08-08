/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEOUTSTREAM_HPP_
#define SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEOUTSTREAM_HPP_
#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"

namespace Zscript {
template<class ZP>
class Zscript;
namespace GenericCore {
template<class ZP>
class SemanticParser;

template<class ZP>
class ScriptSpaceOutStream: public AbstractOutStream<ZP> {
    Zscript<ZP> *z;
    SemanticParser<ZP> *parser;
    uint8_t *buffer;
    uint16_t bufferLength;
    uint16_t bufferPos = 0;
    bool openB = false;
    bool isFailed = false;
    bool isError = false;

public:

    ScriptSpaceOutStream(Zscript<ZP> *z, SemanticParser<ZP> *parser, uint8_t *buffer, uint16_t bufferLength) :
            z(z), parser(parser), buffer(buffer), bufferLength(bufferLength) {
    }

    void open() {
        openB = true;
        bufferPos = 0;
        isFailed = false;
    }

    void close() {
        if (isFailed) {
            z->getNotificationOutStream()->open();
            z->getNotificationOutStream()->writeBytes(buffer, bufferPos);
            z->getNotificationOutStream()->close();
        }
        if (isError) {
            parser->stop();
        }
        isError = false;
        isFailed = false;
        openB = false;
    }

    bool isOpen() {
        return openB;
    }

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (int i = 0; i < count; i++) {
                buffer[bufferPos++] = AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4);
                buffer[bufferPos++] = AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (int i = 0; i < count; i++) {
                buffer[bufferPos++] = bytes[i];
            }
        }
        if (parser->isInErrorState()) {
            isFailed = true;
            isError = true;
        } else if (parser->isFailed()) {
            isFailed = true;
        } else {
            isFailed = false;
        }
    }

};
}
}
#endif /* SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEOUTSTREAM_HPP_ */

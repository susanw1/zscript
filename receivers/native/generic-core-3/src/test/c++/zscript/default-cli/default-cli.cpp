/*
 * default-cli.cpp
 *
 *  Created on: 10 Aug 2023
 *      Author: alicia
 */

#include <iostream>
#include "../../../../main/c++/zscript/modules/scriptSpace/ScriptSpaceModule.hpp"
#include "../../../../main/c++/zscript/modules/outerCore/OuterCoreModule.hpp"
#include "../../../../main/c++/zscript/modules/core/CoreModule.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"

namespace Zscript {
template<class ZP>
class LocalOutStream: public AbstractOutStream<ZP> {
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

    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        if (hexMode) {
            for (uint16_t i = 0; i < count; i++) {
                std::cout << (char) AbstractOutStream<ZP>::toHexChar(bytes[i] >> 4);
                std::cout << (char) AbstractOutStream<ZP>::toHexChar(bytes[i] & 0xf);
            }
        } else {
            for (uint16_t i = 0; i < count; ++i) {
                std::cout << (char) bytes[i];
            }
        }
    }

};
template<class ZP>
class LocalChannel: public ZscriptChannel<ZP> {
    uint8_t data[128];
    LocalOutStream<ZP> outStr;                              // 8 bytes
    GenericCore::TokenRingBuffer<ZP> buffer;                // 20 bytes
    ZscriptTokenizer<ZP> tokenizer;                         //16 bytes
    uint16_t waitForTheBlockingInput = 0;
    char tmp = 0;
    bool hasTmp = false;

public:
    LocalChannel() :
            ZscriptChannel<ZP>(&outStr, &buffer, true), buffer(data, 128), tokenizer(buffer.getWriter(), 2) {
    }
    void moveAlong() {
        if (waitForTheBlockingInput++ < 10000) {
            return;
        }
        waitForTheBlockingInput = 0;
        if (hasTmp) {
            if (tokenizer.offer(tmp)) {
                hasTmp = false;
            } else {
                return;
            }
        }
        tmp = 0;
        while (tmp != '\n') {
            std::cin.get(tmp);
            if (!tokenizer.offer(tmp)) {
                hasTmp = true;
                return;
            }
        }
    }

};
}
class zp {
    static uint16_t currentRnd;

public:
    typedef uint8_t tokenBufferSize_t;

    static const uint8_t lockByteCount = 1;

    static uint16_t generateRandom16() {
        currentRnd++;
        currentRnd ^= currentRnd * 23;
        return currentRnd;
    }
};
uint16_t zp::currentRnd = 1249;

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Zscript::LocalChannel<zp> localChannel;
    Zscript::ZscriptChannel<zp> *localChannelP = &localChannel;
    Zscript::Zscript<zp>::zscript.setChannels(&localChannelP, 1);
    while (true) {
        Zscript::Zscript<zp>::zscript.progress();
    }
}


/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include "../../../../main/c++/zscript/modules/core/CoreModule.hpp"
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "../../../../main/c++/zscript/notifications/ZscriptNotificationSource.hpp"
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"

#include "../../../../main/c++/zscript/scriptSpace/ScriptSpace.hpp"
#include "../semanticParser/BufferOutStream.hpp"
namespace Zscript {
namespace GenericCore {
class zp {
public:
    static const uint8_t lockByteCount = 3;
};

class ScriptSpaceTest {
    uint8_t scriptSpace[256];
    uint8_t outBuffer[256];
    uint8_t data[256];
    TokenRingBuffer<zp> buffer;

    SemanticParser<zp> parser;

    ZscriptTokenizer<zp> tokenizer;

    Zscript<zp> zscript;
    BufferOutStream<zp> outStream;

    void checkAgainstOut(const char *text) {
        DataArrayWLeng16 data = outStream.getData();
        uint16_t textLen = 0;
        for (uint16_t i = 0; text[i] != 0; ++i) {
            textLen++;
        }
        bool worked = true;
        if (textLen != data.length) {
            std::cerr << "Out length doesn't match expected (Exp Len: " << textLen << ", actual: " << data.length << ")\n";
            worked = false;
        } else {
            for (uint16_t i = 0; text[i] != 0; ++i) {
                if (data.data[i] != text[i]) {
                    std::cerr << "Out value doesn't match expected\n ";
                    worked = false;
                }
            }
        }
        if (!worked) {
            std::cerr << "Expected: " << text;
            std::cerr << "\nActual: ";
            for (uint16_t i = 0; i < data.length; ++i) {
                std::cerr << data.data[i];
            }
            std::cerr << "\n";
            throw 0;
        }
    }

public:
    ScriptSpaceTest() :
            buffer(outBuffer, 256), parser(&buffer), tokenizer(&buffer, 2) {

    }

    void shouldRunScriptSpaceWithFailuresAndErrors() {
        const char *text = "Z1&Z0\nZ1ABC&Z1S1\nZ1S1|Z0\nZ1S10\n";
        zscript.setNotificationOutStream(&outStream);

        ScriptSpace<zp> space(&zscript, scriptSpace, 256, outBuffer, 256);
        TokenRingBuffer<zp> writer = space.overwrite();
        ZscriptTokenizer<zp> tok(&writer, 2);
        for (uint16_t i = 0; text[i] != 0; i++) {
            tok.accept(text[i]);
        }
        space.commitChanges(&writer);
        space.run();
        ZscriptChannel<zp> *spaceP = &space;
        zscript.setChannels(&spaceP, 1);

        for (uint16_t i = 0; i < 20; ++i) {
            zscript.progress();
        }
        while (zscript.progress()) {
        }
        checkAgainstOut("!ABCS&S1\n!S10\n");
    }

};
}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    Zscript::GenericCore::ScriptSpaceTest test1;
    test1.shouldRunScriptSpaceWithFailuresAndErrors();

//    Zscript::someValue = 0;
}

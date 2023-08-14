/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#define ZSCRIPT_SUPPORT_NOTIFICATIONS
#define ZSCRIPT_SUPPORT_SCRIPT_SPACE

#include "../../../../main/c++/zscript/modules/scriptSpace/ScriptSpaceModule.hpp"
#include "../../../../main/c++/zscript/modules/core/CoreModule.hpp"
#include "StringChannel.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"

#include "../../../../main/c++/zscript/scriptSpace/ScriptSpace.hpp"
#include "../semanticParser/BufferOutStream.hpp"
namespace Zscript {
namespace GenericCore {
class zp {
    static uint16_t currentRnd;

public:
    typedef uint16_t tokenBufferSize_t;

    static const uint8_t lockByteCount = 3;

    static uint16_t generateRandom16() {
        currentRnd++;
        currentRnd ^= currentRnd * 23;
        return currentRnd;
    }
};
uint16_t zp::currentRnd = 1249;

class ScriptSpaceTest {
    uint8_t scriptSpace[256];
    uint8_t outBuffer[256];

    BufferOutStream<zp> outStream;
    class EmptyChannel: public ZscriptChannel<zp> {
        TokenRingBuffer<zp> emptyBuffer;

    public:
        EmptyChannel(BufferOutStream<zp> *outStream) :
                ZscriptChannel<zp>(outStream, &emptyBuffer, true), emptyBuffer(NULL, 0) {
        }

    };
    EmptyChannel empty;
    ZscriptChannel<zp> *emptyP = &empty;

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

    ScriptSpace<zp> spaceFromString(const char *text) {
        ScriptSpace<zp> space(scriptSpace, 256, outBuffer, 256);
        TokenRingBuffer<zp> writer = space.overwrite();
        ZscriptTokenizer<zp> tok(writer.getWriter(), 2);
        for (uint16_t i = 0; text[i] != 0; i++) {
            tok.accept(text[i]);
        }
        space.commitChanges(&writer);
        return space;
    }

public:
    ScriptSpaceTest() :
            empty(&outStream) {

    }

    void shouldRunScriptSpaceWithFailuresAndErrors() {

        ScriptSpace<zp> space = spaceFromString("Z1&Z0\nZ1ABC&Z1S1\nZ1S1|Z0\nZ1S10\n");
        space.run();
        ZscriptChannel<zp> *channels[2] = { &space, emptyP };
        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);

        for (uint16_t i = 0; i < 20; ++i) {
            Zscript<zp>::zscript.progress();
        }
        while (Zscript<zp>::zscript.progress()) {
        }
        checkAgainstOut("!ABCS&S1\n!S10\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

    void scriptSpaceShouldJamZscript() {
        StringChannel<zp> channel(&outStream, "Z1AB\n");

        ScriptSpace<zp> space = spaceFromString("Z1&Z0\n");
        space.run();
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);

        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

    void shouldReadScriptSpaceCapabilities() {
        StringChannel<zp> channel(&outStream, "Z2&Z20\n");

        ScriptSpace<zp> space = spaceFromString("Z1&Z0\n");
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("!AS&C7P1S\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

    void shouldReadScriptSpaceSetup() {
        StringChannel<zp> channel(&outStream, "Z2&Z21P0\n");

        ScriptSpace<zp> space = spaceFromString("Z1&Z0\n");
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("!AS&P7WL100S\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

    void shouldRunScriptSpaceFromSetup() {
        StringChannel<zp> channel(&outStream, "Z2&Z21P0R1\nZ\n");

        ScriptSpace<zp> space = spaceFromString("Z1&Z0\n");
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("!AS&P7WL100S\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

    void shouldRunAndStopScriptSpaceFromSetup() {
        StringChannel<zp> channel(&outStream, "Z2&Z21P0R1&Z21P&Z21PR&Z21P\nZ1\n");

        ScriptSpace<zp> space = spaceFromString("Z2&Z1S1\n");
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("!AS&PbWL100S&PbRWL100S&PbRWL100S&PbWL100S\n!S\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }
    void shouldWriteToSpace() {
        StringChannel<zp> channel(&outStream, "Z2&Z21P&Z22P\"Z1S10=0a\"&Z21PR1\n");

        ScriptSpace<zp> space = spaceFromString("");
        ScriptSpace<zp> *spaceP = &space;
        ZscriptChannel<zp> *channels[2] = { &space, &channel };

        Zscript<zp>::zscript.setChannels(channels, 2);
        Zscript<zp>::zscript.setNotificationChannelIndex(1);
        Zscript<zp>::zscript.setScriptSpaces(&spaceP, 1);
        int i = 0;
        while (Zscript<zp>::zscript.progress() && i++ < 1000000) {
        }
        checkAgainstOut("!AS&PWL100S&Lf9S&P7WL100S\n!S10\n");
        LockSet<zp> locks = LockSet<zp>::allLocked();
        Zscript<zp>::zscript.unlock(&locks);
    }

};
}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    Zscript::GenericCore::ScriptSpaceTest test1;
    test1.shouldRunScriptSpaceWithFailuresAndErrors();

    Zscript::GenericCore::ScriptSpaceTest test2;
    test2.scriptSpaceShouldJamZscript();

    Zscript::GenericCore::ScriptSpaceTest test3;
    test3.shouldReadScriptSpaceCapabilities();

    Zscript::GenericCore::ScriptSpaceTest test4;
    test4.shouldReadScriptSpaceSetup();

    Zscript::GenericCore::ScriptSpaceTest test5;
    test5.shouldRunScriptSpaceFromSetup();

    Zscript::GenericCore::ScriptSpaceTest test6;
    test6.shouldRunAndStopScriptSpaceFromSetup();

    Zscript::GenericCore::ScriptSpaceTest test7;
    test7.shouldWriteToSpace();

//    Zscript::someValue = 0;
}

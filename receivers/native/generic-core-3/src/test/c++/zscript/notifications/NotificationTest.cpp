/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "../../../../main/c++/zscript/ZscriptIncludes.hpp"
#include "../../../../main/c++/zscript/execution/ZscriptCommandContext.hpp"
#include "../../../../main/c++/zscript/execution/ZscriptNotificationContext.hpp"
#include "../../../../main/c++/zscript/modules/ZscriptModule.hpp"

/**
 * Test command with async behaviour: supply 'E' ("end") value - if 0, just writes B=0; otherwise requires attention every time its value is incremented. When value reaches
 * the 'E' value, if even, it writes that value as 'C'; if odd, returns a fail status.
 */
namespace Zscript {
class zp {
    static uint16_t currentRnd;

public:
    static const uint8_t lockByteCount = 3;

    static uint16_t generateRandom16() {
        currentRnd++;
        currentRnd ^= currentRnd * 23;
        return currentRnd;
    }
};
uint16_t zp::currentRnd = 1249;

int someValue = 0;
int lastID = 0;
bool lastIsAddressing = false;
bool lastIsMoveAlong = false;
const char *toWrite = "";

AsyncActionNotifier<zp> notifier;

#define MODULE_NOTIFICATION_EXISTS_00F EXISTENCE_MARKER_UTIL
#define MODULE_NOTIFICATION_SWITCH_00F NOTIFICATION_SWITCH_UTIL(AsyncTestModule::notification)

class AsyncTestModule {
public:

    /**
     * For testing, use 2nd part of address to match someValue if it's <10. Otherwise, don't go async at all.
     */

    static void notification(GenericCore::ZscriptNotificationContext<zp> ctx, bool moveAlong) {
        lastID = ctx.getID();
        lastIsAddressing = ctx.isAddressing();
        lastIsMoveAlong = moveAlong;
        if (someValue != 0) {
            ctx.notificationNotComplete();
            someValue--;
        }
        notifier = ctx.getAsyncActionNotifier();
        ctx.getOutStream().writeQuotedString(toWrite);
    }

    static void increment() {
        someValue++;
        notifier.notifyNeedsAction();
    }

};

}

#include "../../../../main/c++/zscript/modules/core/CoreModule.hpp"
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "../../../../main/c++/zscript/notifications/ZscriptNotificationSource.hpp"
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"
#include "../semanticParser/BufferOutStream.hpp"
namespace Zscript {
GenericCore::ZscriptNotificationSource<zp> source;
GenericCore::ZscriptNotificationSource<zp> *sourceP = &source;
namespace GenericCore {

class NotificationTest {
    static const SemanticActionType INVALID = SemanticActionType::INVALID;
    static const SemanticActionType RUN_COMMAND = SemanticActionType::RUN_COMMAND;
    static const SemanticActionType RUN_FIRST_COMMAND = SemanticActionType::RUN_FIRST_COMMAND;
    static const SemanticActionType WAIT_FOR_TOKENS = SemanticActionType::WAIT_FOR_TOKENS;
    static const SemanticActionType END_SEQUENCE = SemanticActionType::END_SEQUENCE;
    static const SemanticActionType CLOSE_PAREN = SemanticActionType::CLOSE_PAREN;
    static const SemanticActionType ERROR = SemanticActionType::ERROR;
    uint8_t data[256];
    TokenRingBuffer<zp> buffer;

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

    void setup() {
        Zscript<zp>::zscript.setChannels(&emptyP, 1);
        Zscript<zp>::zscript.setNotificationSources(&sourceP, 1);
        Zscript<zp>::zscript.setNotificationChannelIndex(0);
    }

public:
    NotificationTest() :
            buffer(data, 256), empty(&outStream) {
    }
    void shouldProduceBasicNotification() {
        setup();
        LockSet<zp> locks = LockSet<zp>::allLocked();
        toWrite = "data";

        source.set(&locks, 0xf0, false);

        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("!f\"data\"\n");
        outStream.reset();
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldProduceBasicAsyncNotification() {
        setup();
        LockSet<zp> locks = LockSet<zp>::allLocked();

        toWrite = "data";
        someValue = 1;

        source.set(&locks, 0xf0, false);

        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("!f\"data\"");
        outStream.reset();
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        } else if (lastID != 0xf0) {
            std::cerr << "lastID wrong\n";
            throw 0;
        } else if (lastIsAddressing) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        } else if (lastIsMoveAlong) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        }

        toWrite = "MoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("\"MoreData\"\n");
        outStream.reset();
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        } else if (lastID != 0xf0) {
            std::cerr << "lastID wrong\n";
            throw 0;
        } else if (lastIsAddressing) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        } else if (!lastIsMoveAlong) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        }
    }
    void shouldProduceBasicAsyncAddressingNotification() {
        setup();
        LockSet<zp> locks = LockSet<zp>::allLocked();
        toWrite = "data";
        someValue = 2;

        source.set(&locks, 0xfd, true);

        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("!\"data\"");
        outStream.reset();
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        } else if (lastID != 0xfd) {
            std::cerr << "lastID wrong\n";
            throw 0;
        } else if (!lastIsAddressing) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        } else if (lastIsMoveAlong) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        }

        toWrite = "MoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("\"MoreData\"");
        outStream.reset();
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        } else if (lastID != 0xfd) {
            std::cerr << "lastID wrong\n";
            throw 0;
        } else if (!lastIsAddressing) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        } else if (!lastIsMoveAlong) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        }

        toWrite = "YetMoreData";
        notifier.notifyNeedsAction();
        for (int i = 0; i < 20; i++) {
            Zscript<zp>::zscript.progress();
        }
        checkAgainstOut("\"YetMoreData\"\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        } else if (lastID != 0xfd) {
            std::cerr << "lastID wrong\n";
            throw 0;
        } else if (!lastIsAddressing) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        } else if (!lastIsMoveAlong) {
            std::cerr << "lastIsAddressing wrong\n";
            throw 0;
        }

    }
};

}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Zscript::GenericCore::NotificationTest s1;
    s1.shouldProduceBasicNotification();

    Zscript::GenericCore::NotificationTest s2;
    s2.shouldProduceBasicAsyncNotification();

    Zscript::GenericCore::NotificationTest s3;
    s3.shouldProduceBasicAsyncAddressingNotification();
//    Zscript::someValue = 0;
}


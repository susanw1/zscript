/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "../../../../main/c++/zscript/ZscriptIncludes.hpp"
#include "../../../../main/c++/zscript/execution/ZscriptCommandContext.hpp"
#include "../../../../main/c++/zscript/execution/ZscriptAddressingContext.hpp"
#include "../../../../main/c++/zscript/modules/ZscriptModule.hpp"

/**
 * Test command with async behaviour: supply 'E' ("end") value - if 0, just writes B=0; otherwise requires attention every time its value is incremented. When value reaches
 * the 'E' value, if even, it writes that value as 'C'; if odd, returns a fail status.
 */
namespace Zscript {
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

int someValue = 0;
AsyncActionNotifier<zp> notifier;
#define COMMAND_EXISTS_00F2 EXISTENCE_MARKER_UTIL
class AsyncCommand {
public:

    static void execute(ZscriptCommandContext<zp> ctx) {
        CommandOutStream<zp> out = ctx.getOutStream();
        out.writeField('A', 0xff);

        uint16_t endOnValue = ctx.getField('E', 1);
        if (endOnValue == 0) {
            out.writeField('B', 0);
        } else {
            notifier = ctx.getAsyncActionNotifier();
            ctx.commandNotComplete();
        }
    }

    static void moveAlong(ZscriptCommandContext<zp> ctx) {
        uint16_t endOnValue = ctx.getField('E', 1);

        if (someValue < endOnValue) {
            ctx.commandNotComplete();
        } else if (someValue == endOnValue) {
            if (endOnValue % 2 == 0) {
                ctx.getOutStream().writeField('C', endOnValue);
            } else {
                ctx.status(1);
            }
        }
    }

};
#define MODULE_EXISTS_00F EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_00F MODULE_SWITCH_UTIL_WITH_ASYNC(AsyncTestModule::execute)
#define MODULE_ADDRESS_EXISTS_00F EXISTENCE_MARKER_UTIL
#define MODULE_ADDRESS_SWITCH_00F ADDRESS_SWITCH_UTIL_WITH_ASYNC(AsyncTestModule::address)

class AsyncTestModule {
public:

    static void execute(ZscriptCommandContext<zp> ctx, uint8_t bottomBits, bool moveAlong) {
        switch (bottomBits) {
        case 2:
            // picked 2 at random. this is command 0xf2
            if (moveAlong) {
                AsyncCommand::moveAlong(ctx);
            } else {
                AsyncCommand::execute(ctx);
            }
        }
    }

    /**
     * For testing, use 2nd part of address to match someValue if it's <10. Otherwise, don't go async at all.
     */

    static void address(ZscriptAddressingContext<zp> ctx, bool moveAlong) {
        AddressOptIterator<zp> s = ctx.getAddressSegments();
        s.next(); // throw away module-lookup key
        if (moveAlong) {
            if (s.next().value != someValue) {
                ctx.commandNotComplete();
            }
        } else {
            if (s.next().value < 10) {
                ctx.commandNotComplete();
                notifier = ctx.getAsyncActionNotifier();
            }
        }
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
#include "BufferOutStream.hpp"
#include "ParserActionTester.hpp"
namespace Zscript {
namespace GenericCore {

class SemanticParserTest {
    static const SemanticActionType INVALID = SemanticActionType::INVALID;
    static const SemanticActionType RUN_COMMAND = SemanticActionType::RUN_COMMAND;
    static const SemanticActionType RUN_FIRST_COMMAND = SemanticActionType::RUN_FIRST_COMMAND;
    static const SemanticActionType WAIT_FOR_TOKENS = SemanticActionType::WAIT_FOR_TOKENS;
    static const SemanticActionType END_SEQUENCE = SemanticActionType::END_SEQUENCE;
    static const SemanticActionType CLOSE_PAREN = SemanticActionType::CLOSE_PAREN;
    static const SemanticActionType ERROR = SemanticActionType::ERROR;
    uint8_t data[256];
    TokenRingBuffer<zp> buffer;

    SemanticParser<zp> parser;

    ZscriptTokenizer<zp> tokenizer;

    BufferOutStream<zp> outStream;

    void feedToTokenizer(const char *text) {
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer.accept(text[i]);
        }
    }
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

    void checkParserState(SemanticParserState s) {
        if (parser.getState() != s) {
            std::cerr << "Bad parser state\n";
            throw 0;
        }
    }

    void checkActionType(ZscriptAction<zp> a, SemanticActionType t) {
        if ((SemanticActionType) a.getType() != t) {
            std::cerr << "Bad action type: \nExpected: " << (uint16_t) t << "\nActual: " << (uint16_t) a.getType() << "\n";
            throw 0;
        }
    }

public:
    SemanticParserTest() :
            buffer(data, 256), parser(&buffer), tokenizer(&buffer, 2) {
    }
    void shouldRunAsyncCommand() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
        parserActionTester.parseSnippet("Z2 & Zf2 ", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");

        // simulate async interaction with the command's state
        AsyncTestModule::increment();

        parserActionTester.parseSnippet("", SemanticActionType::COMMAND_MOVEALONG, SemanticParserState::COMMAND_FAILED, "!AS&AffS1");
        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS&AffS1\n");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS&AffS1\n");
    }

    void shouldRunMultiplePasses() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
        parserActionTester.parseSnippet("Z2 & Zf2 E2 ", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");

        // simulate async interaction with the command's state
        AsyncTestModule::increment();
        checkParserState(SemanticParserState::COMMAND_NEEDS_ACTION);

        parserActionTester.parseSnippet("", SemanticActionType::COMMAND_MOVEALONG, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");

        AsyncTestModule::increment();

        parserActionTester.parseSnippet("", SemanticActionType::COMMAND_MOVEALONG, SemanticParserState::COMMAND_COMPLETE, "!AS&AffC2S");
        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS&AffC2S\n");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS&AffC2S\n");
    }

    void shouldRunMultiplePassesWithErrorMidflight() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z2 & Zf2 E2 &", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }
        parserActionTester.parseSnippet("", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");

        tokenizer.dataLost();

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("\n", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }
        // simulate async interaction with the command's state
        AsyncTestModule::increment();
        checkParserState(SemanticParserState::COMMAND_NEEDS_ACTION);

        parserActionTester.parseSnippet("", SemanticActionType::COMMAND_MOVEALONG, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::COMMAND_INCOMPLETE, "!AS&Aff");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }
        AsyncTestModule::increment();

        parserActionTester.parseSnippet("", SemanticActionType::COMMAND_MOVEALONG, SemanticParserState::COMMAND_COMPLETE, "!AS&AffC2S");
        parserActionTester.parseSnippet("", SemanticActionType::ERROR, SemanticParserState::PRESEQUENCE, "!AS&AffC2S!S10\n");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS&AffC2S!S10\n");
    }

    void shouldRunMultiplePassesInAddressing() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parser.activate();
        parserActionTester.parseSnippet("@f.2 Z1A\n", SemanticActionType::INVOKE_ADDRESSING, SemanticParserState::ADDRESSING_INCOMPLETE, "");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::ADDRESSING_INCOMPLETE, "");

        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        // simulate async interaction with the command's state
        AsyncTestModule::increment();
        checkParserState(SemanticParserState::ADDRESSING_NEEDS_ACTION);

        parserActionTester.parseSnippet("", SemanticActionType::ADDRESSING_MOVEALONG, SemanticParserState::ADDRESSING_INCOMPLETE, "");
        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_ASYNC, SemanticParserState::ADDRESSING_INCOMPLETE, "");

        AsyncTestModule::increment();
        checkParserState(SemanticParserState::ADDRESSING_NEEDS_ACTION);
        parserActionTester.parseSnippet("", SemanticActionType::ADDRESSING_MOVEALONG, SemanticParserState::ADDRESSING_COMPLETE, "");
        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "");

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
    }
};

}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

//    Zscript::GenericCore::SemanticParserTest s1;
//    s1.shouldRunAsyncCommand();
//    Zscript::someValue = 0;
//
//    Zscript::GenericCore::SemanticParserTest s2;
//    s2.shouldRunMultiplePasses();
//    Zscript::someValue = 0;
//
//    Zscript::GenericCore::SemanticParserTest s3;
//    s3.shouldRunMultiplePassesWithErrorMidflight();
//    Zscript::someValue = 0;

    Zscript::GenericCore::SemanticParserTest s4;
    s4.shouldRunMultiplePassesInAddressing();
    Zscript::someValue = 0;

}


/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"
#include "BufferOutStream.hpp"
namespace Zscript {
namespace GenericCore {
class zp {
public:
    static const uint8_t lockByteCount = 3;
};

class SemanticParserTest {
    uint8_t data[256];
    TokenRingBuffer<zp> buffer;

    SemanticParser<zp> parser;

    ZscriptTokenizer<zp> tokenizer;

    Zscript<zp> zscript;
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
            std::cerr << "Out length doesn't match expected\n";
            worked = false;
        } else {
            for (uint16_t i = 0; text[i] != 0; ++i) {
                if (data.data[i] != text[i]) {
                    std::cerr << "Out value doesn't match expected\n";
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

    void checkParserState(State s) {
        if (parser.getState() != s) {
            std::cerr << "Bad parser state\n";
            throw 0;
        }
    }

    void checkActionType(SemanticParserAction<zp> a, ActionType t) {
        if (a.getType() != t) {
            std::cerr << "Bad action type: \nExpected: " << t << "\nActual: " << a.getType() << "\n";
            throw 0;
        }
    }

public:
    SemanticParserTest() :
            buffer(data, 256), parser(&buffer), tokenizer(&buffer, 2) {
    }
    void shouldStartNoActionWithEmptyTokens() {
        checkParserState(PRESEQUENCE);
        SemanticParserAction<zp> a = parser.getAction();
        checkActionType(a, WAIT_FOR_TOKENS);
        if (outStream.getData().length != 0) {
            std::cerr << "Bad output content\n";
            throw 0;
        }
        checkParserState(PRESEQUENCE);
    }

    void shouldProduceActionForCommand() {
        feedToTokenizer("Z0\n");

        checkParserState(PRESEQUENCE);
        SemanticParserAction<zp> a1 = parser.getAction();
        checkActionType(a1, RUN_FIRST_COMMAND);

        a1.performAction(&zscript, &outStream);
        checkAgainstOut("!V1C3007M1S");
        outStream.reset();
        checkParserState(COMMAND_COMPLETE);

        SemanticParserAction<zp> a2 = parser.getAction();
        checkActionType(a2, END_SEQUENCE);

        a2.performAction(&zscript, &outStream);
        checkAgainstOut("\n");
        checkParserState(PRESEQUENCE);

        SemanticParserAction<zp> a3 = parser.getAction();
        checkActionType(a3, WAIT_FOR_TOKENS);
        checkAgainstOut("");
        checkParserState(PRESEQUENCE);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }
};

}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Zscript::GenericCore::SemanticParserTest s1;
    s1.shouldProduceActionForCommand();
}


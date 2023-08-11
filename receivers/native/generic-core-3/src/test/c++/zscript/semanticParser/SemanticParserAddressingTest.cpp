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
#include "BufferOutStream.hpp"
#include "ParserActionTester.hpp"
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

class SemanticParserTest {
    static const SemanticActionType INVALID = SemanticActionType::INVALID;
    static const SemanticActionType RUN_COMMAND = SemanticActionType::RUN_COMMAND;
    static const SemanticActionType RUN_FIRST_COMMAND = SemanticActionType::RUN_FIRST_COMMAND;
    static const SemanticActionType WAIT_FOR_TOKENS = SemanticActionType::WAIT_FOR_TOKENS;
    static const SemanticActionType END_SEQUENCE = SemanticActionType::END_SEQUENCE;
    static const SemanticActionType CLOSE_PAREN = SemanticActionType::CLOSE_PAREN;
    static const SemanticActionType ERROR = SemanticActionType::ERROR;
    static const SemanticActionType INVOKE_ADDRESSING = SemanticActionType::INVOKE_ADDRESSING;

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
            std::cerr << "Bad action type: \nExpected: " << (uint8_t) t << "\nActual: " << (uint8_t) a.getType() << "\n";
            throw 0;
        }
    }

public:
    SemanticParserTest() :
            buffer(data, 256), parser(&buffer), tokenizer(&buffer, 2) {
    }
    void shouldProduceActionForBasicAddressing() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS };
        parserActionTester.parseSnippet("@2Z1\n", types, 3);
        checkAgainstOut("");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldAcceptAddressingWithEchoAndLocks() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS };
        parserActionTester.parseSnippet("_2%233@2Z1\n", types, 3);
        checkAgainstOut("");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldFailInvalidAddressing() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { ERROR, WAIT_FOR_TOKENS };
        parserActionTester.parseSnippet("@99999Z1\n", types, 2);
        checkAgainstOut("!10S20\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldAcceptMultPartAddressing() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS };
        parserActionTester.parseSnippet("@1.2.3.4.5.6.7.8.9.0.1.2.3.4.5.6.7.8.9.0Z1\n", types, 3);
        checkAgainstOut("");
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
    s1.shouldProduceActionForBasicAddressing();

    Zscript::GenericCore::SemanticParserTest s2;
    s2.shouldAcceptAddressingWithEchoAndLocks();

    Zscript::GenericCore::SemanticParserTest s3;
    s3.shouldFailInvalidAddressing();

    Zscript::GenericCore::SemanticParserTest s4;
    s4.shouldAcceptMultPartAddressing();

}


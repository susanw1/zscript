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

    void checkParserState(State s) {
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

    void shouldHandleActionTypesAndIO(const char *input, const char *output) {
        ParserActionTester<zp> parserActionTester(&Zscript<zp>::zscript, &buffer, &tokenizer, &parser, &outStream);

        parserActionTester.parseSnippet(input, output);
        checkAgainstOut(output);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }
public:
    SemanticParserTest() :
            buffer(data, 256), parser(&buffer), tokenizer(&buffer, 2) {
    }
    void shouldHandleTwoEmptyCommands() {
        ParserActionTester<zp> parserActionTester(&Zscript<zp>::zscript, &buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS };

        parserActionTester.parseSnippet("\n \n", types, 5);
        checkAgainstOut("!\n!\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldHandleSeveralEmptyCommands() {
        ParserActionTester<zp> parserActionTester(&Zscript<zp>::zscript, &buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE,
                WAIT_FOR_TOKENS };

        parserActionTester.parseSnippet("\n\n \n\n", types, 9);
        checkAgainstOut("!\n!\n!\n!\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldHandleSeveralBasicCommands() {
        ParserActionTester<zp> parserActionTester(&Zscript<zp>::zscript, &buffer, &tokenizer, &parser, &outStream);
        SemanticActionType types[] = { RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE,
                WAIT_FOR_TOKENS };

        parserActionTester.parseSnippet("Z1A\nZ1B\n Z1C\nZ1D\n", types, 9);
        checkAgainstOut("!AS\n!BS\n!CS\n!DS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    static void shouldProduceMultipleResponses() {
        SemanticParserTest s1;
        s1.shouldHandleActionTypesAndIO("Z1 A2 +1234\n Z1 A3B3 +5678\n", "!A2+1234S\n!A3B3+5678S\n");
        SemanticParserTest s2;
        s2.shouldHandleActionTypesAndIO("Z1A S1 \n Z1B\n", "!AS1\n!BS\n");
        SemanticParserTest s3;
        s3.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S1 \n Z1C\n", "!AS1|BS1\n!CS\n");
        SemanticParserTest s4;
        s4.shouldHandleActionTypesAndIO("Z1A S13 \n Z1B\n", "!AS13\n!BS\n");
        SemanticParserTest s5;
        s5.shouldHandleActionTypesAndIO("Z1A S13 | Z1B S1 \n Z1C\n", "!AS13\n!CS\n");
    }

    static void shouldProduceMultipleResponsesWithComments() {
        SemanticParserTest s1;
        s1.shouldHandleActionTypesAndIO("Z1 A \n#hi\n", "!AS\n");
        SemanticParserTest s2;
        s2.shouldHandleActionTypesAndIO("Z1 A \n#hi\n#abc\nZ1B\n", "!AS\n!BS\n");
    }

    static void shouldProduceMultipleResponsesWithAddressing() {
        // Addressing actions are passed on downstream, not acknowledged - they should not appear in the response.
        SemanticParserTest s1;
        s1.shouldHandleActionTypesAndIO("Z1 A \n@1 Z1B\n", "!AS\n");
        SemanticParserTest s2;
        s2.shouldHandleActionTypesAndIO("Z1 A \n@1.2 Z1B\n@2.2 Z1C\n Z1D\n", "!AS\n!DS\n");
    }

};

}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Zscript::GenericCore::SemanticParserTest s1;
    s1.shouldHandleTwoEmptyCommands();

    Zscript::GenericCore::SemanticParserTest s2;
    s2.shouldHandleSeveralEmptyCommands();

    Zscript::GenericCore::SemanticParserTest s3;
    s3.shouldHandleSeveralBasicCommands();

    Zscript::GenericCore::SemanticParserTest::shouldProduceMultipleResponses();

    Zscript::GenericCore::SemanticParserTest::shouldProduceMultipleResponsesWithComments();

    Zscript::GenericCore::SemanticParserTest::shouldProduceMultipleResponsesWithAddressing();
}


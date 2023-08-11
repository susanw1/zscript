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
    void shouldWaitWithAndThen() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        parserActionTester.parseSnippet("Z1A", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("&", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1B", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS&BS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS&BS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }

    void shouldWaitWithOrElse() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        parserActionTester.parseSnippet("Z1A", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("|", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1B", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_SKIP, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

    }

    void shouldWaitWithParens() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        // Z1A & (Z1B S1 | Z1C) | Z1D
        parserActionTester.parseSnippet("Z1A", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("&", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("(", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS&");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1B S1", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS, "!AS&");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("|", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_FAILED, "!AS&(BS1");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1C", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_COMPLETE_NEEDS_TOKENS, "!AS&(BS1");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet(")", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS&(BS1|CS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("|", SemanticActionType::RUN_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS&(BS1|CS)");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1D", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::COMMAND_SKIP, "!AS&(BS1|CS)");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS&(BS1|CS)\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS&(BS1|CS)\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

    }

    void shouldWaitWithMultipleCommands() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        // Z1A\nZ1B\n - with lots of token-draining, too.
        parserActionTester.parseSnippet("Z1A", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1B", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!AS\n!BS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!AS\n!BS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!AS\n!BS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

    }

    void shouldWaitWithErrors() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        // Z1A+a & Z1B\n Z1C\n
        parserActionTester.parseSnippet("Z1A+a", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("&", SemanticActionType::ERROR, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1B", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1C", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!10S21\n!CS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!10S21\n!CS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S21\n!CS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

    }

    void shouldWaitWithOverrunError() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        // Z1A & Z1B & Z1C *\n Z1D\n (overun shown as '*')
        parserActionTester.parseSnippet("Z1A ", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
        feedToTokenizer("& Z1B & Z1C");
        tokenizer.dataLost();

        parserActionTester.parseSnippet("", SemanticActionType::ERROR, SemanticParserState::PRESEQUENCE, "!10S10\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("\n", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S10\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S10\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("Z1D\n", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!10S10\n!DS");
        if (!outStream.isOpen()) {
            std::cerr << "Out stream closed unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!10S10\n!DS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

        parserActionTester.parseSnippet("", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S10\n!DS\n");
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }

    }

    void shouldGiveErrorForFailedComment() {
        ParserActionTester<zp> parserActionTester(&buffer, &tokenizer, &parser, &outStream);
        parserActionTester.parseSnippet("#pq", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "");
        tokenizer.dataLost();
        parserActionTester.parseSnippet("xyz", SemanticActionType::ERROR, SemanticParserState::PRESEQUENCE, "!10S10\n");
        parserActionTester.parseSnippet("\n", SemanticActionType::WAIT_FOR_TOKENS, SemanticParserState::PRESEQUENCE, "!10S10\n");
        parserActionTester.parseSnippet("Z1\n", SemanticActionType::RUN_FIRST_COMMAND, SemanticParserState::COMMAND_COMPLETE, "!10S10\n!S");
        parserActionTester.parseSnippet("", SemanticActionType::END_SEQUENCE, SemanticParserState::PRESEQUENCE, "!10S10\n!S\n");

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
    s1.shouldWaitWithAndThen();

    Zscript::GenericCore::SemanticParserTest s2;
    s2.shouldWaitWithOrElse();

    Zscript::GenericCore::SemanticParserTest s3;
    s3.shouldWaitWithParens();

    Zscript::GenericCore::SemanticParserTest s4;
    s4.shouldWaitWithMultipleCommands();

    Zscript::GenericCore::SemanticParserTest s5;
    s5.shouldWaitWithErrors();

    Zscript::GenericCore::SemanticParserTest s6;
    s6.shouldWaitWithOverrunError();

    Zscript::GenericCore::SemanticParserTest s7;
    s7.shouldGiveErrorForFailedComment();

}


/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#define COMMAND_EXISTS_000C EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_000D EXISTENCE_MARKER_UTIL

#include <iostream>
#include "../../../../main/c++/zscript/modules/core/CoreModule.hpp"
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"
#include "BufferOutStream.hpp"
#include "ParserActionTester.hpp"
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

    void checkActionType(SemanticParserAction<zp> a, ActionType t) {
        if (a.getType() != t) {
            std::cerr << "Bad action type: \nExpected: " << t << "\nActual: " << a.getType() << "\n";
            throw 0;
        }
    }

    void shouldHandleActionTypesAndIO(const char *input, ActionType *actions, const char *output) {
        ParserActionTester<zp> parserActionTester(&zscript, &buffer, &tokenizer, &parser, &outStream);

        uint16_t actionCount;
        for (actionCount = 0; actions[actionCount] != INVALID; ++actionCount) {
        }
        parserActionTester.parseSnippet(input, actions, actionCount);
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
        outStream.reset();
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
    void shouldProduceActionForTwoCommands() {
        feedToTokenizer("Z1A & Z1B\n");

        checkParserState(PRESEQUENCE);
        SemanticParserAction<zp> a1 = parser.getAction();
        checkActionType(a1, RUN_FIRST_COMMAND);
        a1.performAction(&zscript, &outStream);
        checkAgainstOut("!AS");
        outStream.reset();
        checkParserState(COMMAND_COMPLETE);

        SemanticParserAction<zp> a2 = parser.getAction();
        checkActionType(a2, RUN_COMMAND);
        a2.performAction(&zscript, &outStream);
        checkAgainstOut("&BS");
        outStream.reset();
        checkParserState(COMMAND_COMPLETE);

        SemanticParserAction<zp> a3 = parser.getAction();
        checkActionType(a3, END_SEQUENCE);
        a3.performAction(&zscript, &outStream);
        checkAgainstOut("\n");
        outStream.reset();
        checkParserState(PRESEQUENCE);

        SemanticParserAction<zp> a4 = parser.getAction();
        checkActionType(a4, WAIT_FOR_TOKENS);
        checkAgainstOut("");
        checkParserState(PRESEQUENCE);

        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }
    void shouldProduceActionForComment() {
        feedToTokenizer("#a\n");

        checkParserState(PRESEQUENCE);
        SemanticParserAction<zp> a1 = parser.getAction();
        checkActionType(a1, WAIT_FOR_TOKENS);
        a1.performAction(&zscript, &outStream);
        checkAgainstOut("");
        outStream.reset();
        checkParserState(PRESEQUENCE);
        if (outStream.isOpen()) {
            std::cerr << "Out stream open unexpectedly\n";
            throw 0;
        }
    }
    static void shouldProduceActionsForSingleCommands() {
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("Z0\n", types1, "!V1C3007M1S\n");
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("Z1A\n", types2, "!AS\n");
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("Z1A S1\n", types3, "!AS1\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("Z1A S10\n", types4, "!AS10\n");
    }

    static void shouldProduceActionsForLogicalCommandSeries() {
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("Z1A & Z1B\n", types1, "!AS&BS\n");
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("Z1A | Z1B\n", types2, "!AS\n");
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("Z1A & Z1B & Z1C\n", types3, "!AS&BS&CS\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("Z1A | Z1B | Z1C\n", types4, "!AS\n");
        SemanticParserTest s5;
        ActionType types5[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s5.shouldHandleActionTypesAndIO("Z1A & Z1B | Z1C\n", types5, "!AS&BS\n");
        SemanticParserTest s6;
        ActionType types6[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s6.shouldHandleActionTypesAndIO("Z1A | Z1B & Z1C\n", types6, "!AS\n");
    }

    static void shouldProduceActionsForLogicalCommandSeriesWithFailures() {
        // 2 cmds with ANDTHEN
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("Z1A S1 & Z1B\n", types1, "!AS1\n");
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S2\n", types2, "!AS1\n");

        // 2 cmds with ORELSE
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("Z1A S1 | Z1B\n", types3, "!AS1|BS\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S2\n", types4, "!AS1|BS2\n");

        // 3 cmds with ANDTHEN
        SemanticParserTest s5;
        ActionType types5[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s5.shouldHandleActionTypesAndIO("Z1A S1 & Z1B & Z1C\n", types5, "!AS1\n");
        SemanticParserTest s6;
        ActionType types6[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s6.shouldHandleActionTypesAndIO("Z1A & Z1B S1 & Z1C\n", types6, "!AS&BS1\n");
        SemanticParserTest s7;
        ActionType types7[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s7.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S1 & Z1C\n", types7, "!AS1\n");
        SemanticParserTest s8;
        ActionType types8[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s8.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S1 & Z1C S1\n", types8, "!AS1\n");

        // 3 cmds with ORELSE
        SemanticParserTest s9;
        ActionType types9[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s9.shouldHandleActionTypesAndIO("Z1A S1 | Z1B | Z1C\n", types9, "!AS1|BS\n");
        SemanticParserTest s10;
        ActionType types10[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s10.shouldHandleActionTypesAndIO("Z1A | Z1B S1 | Z1C\n", types10, "!AS\n");
        SemanticParserTest s11;
        ActionType types11[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s11.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S1 | Z1C\n", types11, "!AS1|BS1|CS\n");
        SemanticParserTest s12;
        ActionType types12[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s12.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S1 | Z1C S1\n", types12, "!AS1|BS1|CS1\n");

        // 3 cmds with ANDTHEN/ORELSE combo
        SemanticParserTest s13;
        ActionType types13[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s13.shouldHandleActionTypesAndIO("Z1A S1 & Z1B | Z1C\n", types13, "!AS1|CS\n");
        SemanticParserTest s14;
        ActionType types14[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s14.shouldHandleActionTypesAndIO("Z1A & Z1B S1 | Z1C\n", types14, "!AS&BS1|CS\n");
        SemanticParserTest s15;
        ActionType types15[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s15.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S1 & Z1C\n", types15, "!AS1|BS1\n");
        SemanticParserTest s16;
        ActionType types16[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s16.shouldHandleActionTypesAndIO("Z1A S1 | Z1B & Z1C S2\n", types16, "!AS1|BS&CS2\n");
    }

    static void shouldProduceActionsForLogicalCommandSeriesWithErrors() {
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("Z1A S10\n", types1, "!AS10\n");

        // 2 cmds with ANDTHEN
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("Z1A S10 & Z1B\n", types2, "!AS10\n");
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("Z1A & Z1B S20\n", types3, "!AS&BS20\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("Z1A S10 & Z1B S20\n", types4, "!AS10\n");

        // 2 cmds with ORELSE
        SemanticParserTest s5;
        ActionType types5[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s5.shouldHandleActionTypesAndIO("Z1A S10 | Z1B\n", types5, "!AS10\n");
        SemanticParserTest s6;
        ActionType types6[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s6.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S20\n", types6, "!AS1|BS20\n");
        SemanticParserTest s7;
        ActionType types7[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s7.shouldHandleActionTypesAndIO("Z1A S10 | Z1B S20\n", types7, "!AS10\n");

        // 3 cmds with ANDTHEN
        SemanticParserTest s8;
        ActionType types8[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s8.shouldHandleActionTypesAndIO("Z1A S10 & Z1B & Z1C\n", types8, "!AS10\n");
        SemanticParserTest s9;
        ActionType types9[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s9.shouldHandleActionTypesAndIO("Z1A & Z1B S20 & Z1C\n", types9, "!AS&BS20\n");
        SemanticParserTest s10;
        ActionType types10[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s10.shouldHandleActionTypesAndIO("Z1A & Z1B & Z1C S30\n", types10, "!AS&BS&CS30\n");
        SemanticParserTest s11;
        ActionType types11[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s11.shouldHandleActionTypesAndIO("Z1A S10 & Z1B S20 & Z1C\n", types11, "!AS10\n");
        SemanticParserTest s12;
        ActionType types12[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s12.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S20 & Z1C S30\n", types12, "!AS1\n");

        // 3 cmds with ORELSE
        SemanticParserTest s13;
        ActionType types13[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s13.shouldHandleActionTypesAndIO("Z1A S10 | Z1B | Z1C\n", types13, "!AS10\n");
        SemanticParserTest s14;
        ActionType types14[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s14.shouldHandleActionTypesAndIO("Z1A | Z1B S20 | Z1C\n", types14, "!AS\n");
        SemanticParserTest s15;
        ActionType types15[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s15.shouldHandleActionTypesAndIO("Z1A | Z1B | Z1C S30\n", types15, "!AS\n");
        SemanticParserTest s16;
        ActionType types16[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s16.shouldHandleActionTypesAndIO("Z1A S10 | Z1B S20 | Z1C\n", types16, "!AS10\n");
        SemanticParserTest s17;
        ActionType types17[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s17.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S20 | Z1C S1\n", types17, "!AS1|BS20\n");

        // 3 cmds with ANDTHEN/ORELSE combo
        SemanticParserTest s18;
        ActionType types18[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s18.shouldHandleActionTypesAndIO("Z1A S10 & Z1B | Z1C\n", types18, "!AS10\n");
        SemanticParserTest s19;
        ActionType types19[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s19.shouldHandleActionTypesAndIO("Z1A S10 & Z1B S1| Z1C\n", types19, "!AS10\n");
        SemanticParserTest s20;
        ActionType types20[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s20.shouldHandleActionTypesAndIO("Z1A S10 & Z1B S20 | Z1C\n", types20, "!AS10\n");
        SemanticParserTest s21;
        ActionType types21[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s21.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S20 | Z1C\n", types21, "!AS1|CS\n");
        SemanticParserTest s22;
        ActionType types22[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s22.shouldHandleActionTypesAndIO("Z1A S1 & Z1B S20 | Z1CS30\n", types22, "!AS1|CS30\n");

        SemanticParserTest s23;
        ActionType types23[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s23.shouldHandleActionTypesAndIO("Z1A S10 | Z1B & Z1C\n", types23, "!AS10\n");
        SemanticParserTest s24;
        ActionType types24[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s24.shouldHandleActionTypesAndIO("Z1A S10 | Z1B S1 & Z1C\n", types24, "!AS10\n");
        SemanticParserTest s25;
        ActionType types25[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s25.shouldHandleActionTypesAndIO("Z1A S10 | Z1B S20 & Z1C\n", types25, "!AS10\n");
        SemanticParserTest s26;
        ActionType types26[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s26.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S20 & Z1C\n", types26, "!AS1|BS20\n");
        SemanticParserTest s27;
        ActionType types27[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s27.shouldHandleActionTypesAndIO("Z1A S1 | Z1B S20 & Z1CS30\n", types27, "!AS1|BS20\n");
    }

    static void shouldProduceActionsForParenthesizedLogicalCommandSeries() {
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("(Z1A) \n", types1, "!(AS)\n");
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("(Z1A)Z1B \n", types2, "!(AS)BS\n");
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("(Z1A S1)Z1B \n", types3, "!(AS1)\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("Z1A(Z1B) \n", types4, "!AS(BS)\n");
        SemanticParserTest s5;
        ActionType types5[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s5.shouldHandleActionTypesAndIO("Z1A S1(Z1B) \n", types5, "!AS1\n");

        SemanticParserTest s6;
        ActionType types6[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s6.shouldHandleActionTypesAndIO("Z1A & (Z1B & Z1C) & Z1D \n", types6, "!AS&(BS&CS)&DS\n");
        SemanticParserTest s7;
        ActionType types7[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s7.shouldHandleActionTypesAndIO("Z1A S1 & (Z1B & Z1C) & Z1D \n", types7, "!AS1\n");
        SemanticParserTest s8;
        ActionType types8[] = { RUN_FIRST_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s8.shouldHandleActionTypesAndIO("Z1A S1 & (Z1B & Z1C) | Z1D \n", types8, "!AS1|DS\n");

        SemanticParserTest s9;
        ActionType types9[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s9.shouldHandleActionTypesAndIO("Z1A | (Z1B | Z1C) | Z1D \n", types9, "!AS\n");
        SemanticParserTest s10;
        ActionType types10[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s10.shouldHandleActionTypesAndIO("Z1A & (Z1B | Z1C) | Z1D \n", types10, "!AS&(BS)\n");

        SemanticParserTest s11;
        ActionType types11[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s11.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B | Z1C) | Z1D \n", types11, "!AS1|(BS)\n");
        SemanticParserTest s12;
        ActionType types12[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s12.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B S1 | Z1C) | Z1D \n", types12, "!AS1|(BS1|CS)\n");
        SemanticParserTest s13;
        ActionType types13[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s13.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B S1 | Z1C) & Z1D \n", types13, "!AS1|(BS1|CS)&DS\n");
        SemanticParserTest s14;
        ActionType types14[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, CLOSE_PAREN, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s14.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B S1 | Z1C S1) | Z1D \n", types14, "!AS1|(BS1|CS1)|DS\n");
        SemanticParserTest s15;
        ActionType types15[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s15.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B S10 | Z1C) | Z1D \n", types15, "!AS1|(BS10\n");
        SemanticParserTest s16;
        ActionType types16[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s16.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", types16, "!AS1|(BS)\n");
        SemanticParserTest s17;
        ActionType types17[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s17.shouldHandleActionTypesAndIO("Z1A S1 | (Z1B | Z1C S10) | Z1D \n", types17, "!AS1|(BS)\n");
        SemanticParserTest s18;
        ActionType types18[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s18.shouldHandleActionTypesAndIO("Z1A | (Z1B | Z1C) | Z1D S10 \n", types18, "!AS\n");
        SemanticParserTest s19;
        ActionType types19[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s19.shouldHandleActionTypesAndIO("Z1A | (Z1B | Z1C) & Z1D \n", types19, "!AS\n");
        SemanticParserTest s20;
        ActionType types20[] = { RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s20.shouldHandleActionTypesAndIO("Z1A | (Z1B | Z1C S10) & Z1D \n", types20, "!AS\n");
    }

    static void shouldProduceActionsForNestedParenthesizedLogicalCommandSeries() {
        SemanticParserTest s1;
        ActionType types1[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s1.shouldHandleActionTypesAndIO("((Z1A)) \n", types1, "!((AS))\n");
        SemanticParserTest s2;
        ActionType types2[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s2.shouldHandleActionTypesAndIO("((Z1A)Z1B) \n", types2, "!((AS)BS)\n");
        SemanticParserTest s3;
        ActionType types3[] = { RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s3.shouldHandleActionTypesAndIO("(Z1A S1)Z1B \n", types3, "!(AS1)\n");
        SemanticParserTest s4;
        ActionType types4[] = { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s4.shouldHandleActionTypesAndIO("(Z1A(Z1B)) \n", types4, "!(AS(BS))\n");
        SemanticParserTest s5;
        ActionType types5[] = { RUN_FIRST_COMMAND, RUN_COMMAND, CLOSE_PAREN, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s5.shouldHandleActionTypesAndIO("(Z1A S1(Z1B)) \n", types5, "!(AS1)\n");

        SemanticParserTest s6;
        ActionType types6[] =
                { RUN_FIRST_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, RUN_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS, INVALID };
        s6.shouldHandleActionTypesAndIO("Z1A & ((Z1B & Z1C)) & Z1D \n", types6, "!AS&((BS&CS))&DS\n");
    }

};

}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    Zscript::GenericCore::SemanticParserTest s1;
    s1.shouldStartNoActionWithEmptyTokens();

    Zscript::GenericCore::SemanticParserTest s2;
    s2.shouldProduceActionForCommand();

    Zscript::GenericCore::SemanticParserTest s3;
    s3.shouldProduceActionForTwoCommands();

    Zscript::GenericCore::SemanticParserTest s4;
    s4.shouldProduceActionForComment();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForSingleCommands();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForLogicalCommandSeries();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForLogicalCommandSeriesWithFailures();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForLogicalCommandSeriesWithErrors();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForParenthesizedLogicalCommandSeries();

    Zscript::GenericCore::SemanticParserTest::shouldProduceActionsForNestedParenthesizedLogicalCommandSeries();
}


/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_PARSERACTIONTESTER_HPP_
#define SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_PARSERACTIONTESTER_HPP_
#include <iostream>
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "BufferOutStream.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ParserActionTester {
    Zscript<ZP> *zscript;
    TokenRingBuffer<ZP> *buffer;
    ZscriptTokenizer<ZP> *tokenizer;
    SemanticParser<ZP> *parser;
    BufferOutStream<ZP> *outStream;

public:
    ParserActionTester(Zscript<ZP> *zscript,
            TokenRingBuffer<ZP> *buffer,
            ZscriptTokenizer<ZP> *tokenizer,
            SemanticParser<ZP> *parser,
            BufferOutStream<ZP> *outStream) :
            zscript(zscript), buffer(buffer), tokenizer(tokenizer), parser(parser), outStream(outStream) {
    }

    bool parseSnippet(const char *text, SemanticActionType *actionTypes, uint16_t actionTypeCount) {
        // feed all chars into tokens/buffer
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer->accept(text[i]);
        }
        for (uint16_t i = 0; i < actionTypeCount; ++i) {
            SemanticActionType t = actionTypes[i];
//            std::cout << "Expecting actionType=" << (uint8_t)t << "\n";
            ZscriptAction<ZP> action = parser->getAction();

//            std::cout << "  Received action: actionType=" << (uint8_t)action.getType() << "; state=" << parser->getState();
            if ((SemanticActionType) action.getType() != t) {
                std::cerr << "Action type does not match\nExpected: " << (uint8_t) t << "\nActual: " << (uint8_t) action.getType() << "\n";
                return false;
            }
            action.performAction(zscript, outStream);

//            std::cout << "   - After execute action: state=" + parser->getState() << "\n";
        }
        return true;

    }

    bool parseSnippet(const char *text, const char *output) {
        uint16_t outlength = 0;
        for (uint16_t i = 0; output[i] != 0; ++i) {
            outlength++;
        }

        // feed all chars into tokens/buffer
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer->accept(text[i]);
        }

        ZscriptAction<ZP> a1;
        while ((SemanticActionType) (a1 = parser->getAction()).getType() != SemanticActionType::WAIT_FOR_TOKENS) {
//            std::cout << "  Received action: actionType=" << (uint8_t)a1.getType() << "; state=" << parser->getState() << "\n";
            a1.performAction(zscript, outStream);
//            std::cout << "   - After execute action: state=" << parser->getState() << "\n";
        }
        DataArrayWLeng16 result = outStream->getData();

        if (result.length != outlength) {
            std::cerr << "Result length does not match\nExpected: " << outlength << "\nActual: " << result.length << "\n";
            return false;
        }
        for (uint16_t i = 0; i < outlength; ++i) {
            if (result.data[i] != output[i]) {
                std::cerr << "Result value does not match\nExpected: " << output[i] << "\nActual: " << result.data[i] << "\nAt index: " << i << "\n";
                return false;
            }
        }
        return true;
    }

    bool parseSnippet(const char *text, SemanticActionType expectedSemanticActionType, State endState, const char *output) {
        uint16_t outlength = 0;
        for (uint16_t i = 0; output[i] != 0; ++i) {
            outlength++;
        }

        // feed all chars into tokens/buffer
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer->accept(text[i]);
        }

        ZscriptAction<ZP> action = parser->getAction();
//        std::cout << "  Received action: actionType=" <<(uint8_t) action.getType() << " (expected=" <<(uint8_t) expectedSemanticActionType << "); state=" << parser->getState() << "\n";
        if (expectedSemanticActionType != SemanticActionType::INVALID) {
            if ((SemanticActionType) action.getType() != expectedSemanticActionType) {
                std::cerr << "Action type does not match\nExpected: " << (uint8_t) expectedSemanticActionType << "\nActual: " << (uint8_t) action.getType() << "\n";
                return false;
            }
        }

        action.performAction(zscript, outStream);
//        std::cout << "   - After execute action: state=" << parser->getState() << "\n";
        if (parser->getState() != endState) {
            std::cerr << "End state does not match\nExpected: " << (uint8_t) endState << "\nActual: " << (uint8_t) parser->getState() << "\n";
            return false;
        }

        DataArrayWLeng16 result = outStream->getData();
        if (result.length != outlength) {
            std::cerr << "Result length does not match\nExpected: " << outlength << "\nActual: " << result.length << "\n";
            return false;
        }
        for (uint16_t i = 0; i < outlength; ++i) {
            if (result.data[i] != output[i]) {
                std::cerr << "Result value does not match\nExpected: " << output[i] << "\nActual: " << result.data[i] << "\nAt index: " << i << "\n";
                return false;
            }
        }
        return true;
    }
};

}
}

#endif /* SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_PARSERACTIONTESTER_HPP_ */

/*
 * ParserActionTester.hpp
 *
 *  Created on: 1 Aug 2023
 *      Author: alicia
 */

#ifndef SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_PARSERACTIONTESTER_HPP_
#define SRC_TEST_C___ZSCRIPT_SEMANTICPARSER_PARSERACTIONTESTER_HPP_
#include <iostream>
#include "../../../../main/c++/zscript/ZscriptTokenizer.hpp"
#include "../../../../main/c++/zscript/Zscript.hpp"
#include "../../../../main/c++/zscript/semanticParser/SemanticParser.hpp"
#include "BufferOutStream.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ParserActionTester {
    Zscript<ZP> *zcode;
    TokenRingBuffer<ZP> *buffer;
    ZscriptTokenizer<ZP> *tokenizer;
    SemanticParser<ZP> *parser;
    BufferOutStream<ZP> *outStream;

public:
    ParserActionTester(Zscript<ZP> *zcode,
            TokenRingBuffer<ZP> *buffer,
            ZscriptTokenizer<ZP> *tokenizer,
            SemanticParser<ZP> *parser,
            BufferOutStream<ZP> *outStream) :
            zcode(zcode), buffer(buffer), tokenizer(tokenizer), parser(parser), outStream(outStream) {
    }

    bool parseSnippet(const char *text, ActionType *actionTypes, uint16_t actionTypeCount) {
        // feed all chars into tokens/buffer
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer->accept(text[i]);
        }
        for (uint16_t i = 0; i < actionTypeCount; ++i) {
            ActionType t = actionTypes[i];
            std::cout << "Expecting actionType=" << t << "\n";
            SemanticParserAction<ZP> action = parser->getAction();

            std::cout << "  Received action: actionType=" << action.getType() << "; state=" << parser->getState();
            if (action.getType() != t) {
                std::cerr << "Action type does not match\nExpected: " << t << "\nActual: " << action.getType() << "\n";
                return false;
            }
            action.performAction(zcode, outStream);

            std::cout << "   - After execute action: state=" + parser->getState() << "\n";
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

        SemanticParserAction<ZP> a1;
        while ((a1 = parser->getAction()).getType() != ActionType::WAIT_FOR_TOKENS) {
            std::cout << "  Received action: actionType=" << a1 << "; state=" << parser->getState() << "\n";
            a1.performAction(zcode, outStream);
            std::cout << "   - After execute action: state=" << parser->getState() << "\n";
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

    bool parseSnippet(const char *text, ActionType expectedActionType, State endState, const char *output) {
        uint16_t outlength = 0;
        for (uint16_t i = 0; output[i] != 0; ++i) {
            outlength++;
        }

        // feed all chars into tokens/buffer
        for (uint16_t i = 0; text[i] != 0; ++i) {
            tokenizer->accept(text[i]);
        }

        SemanticParserAction<ZP> action = parser->getAction();
        std::cout << "  Received action: actionType=" << action << " (expected=" << expectedActionType << "); state=" << parser->getState() << "\n";
        if (expectedActionType != INVALID) {
            if (action.getType() != expectedActionType) {
                std::cerr << "Action type does not match\nExpected: " << expectedActionType << "\nActual: " << action.getType() << "\n";
                return false;
            }
        }

        action.performAction(zcode, outStream);
        std::cout << "   - After execute action: state=" << parser->getState() << "\n";
        if (parser->getState() != endState) {
            std::cerr << "End state does not match\nExpected: " << endState << "\nActual: " << parser->getState() << "\n";
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

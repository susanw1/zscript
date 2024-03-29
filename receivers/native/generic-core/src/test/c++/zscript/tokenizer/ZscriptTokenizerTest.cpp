/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#define ZSCRIPT_SUPPORT_ADDRESSING

#include "../test-defines.hpp"
#include "../../../../main/c++/zscript/tokenizer/ZscriptTokenizer.hpp"

class zp {
public:
    typedef uint16_t tokenBufferSize_t;
};
namespace Zscript {

bool testBufferContent(uint8_t *instructions, uint8_t *data, uint16_t instructionsLength, uint8_t *dataExpected,
                       uint16_t dataLengthExpected) {
    uint8_t ringBuffer[1000];
    for (int i = 0; i < 100; ++i) {
        ringBuffer[i] = 0xAA;
    }
    GenericCore::TokenRingBuffer<zp> buffer = GenericCore::TokenRingBuffer<zp>(ringBuffer, 1000);
    ZscriptTokenizer<zp> tokenizer = ZscriptTokenizer<zp>(buffer.getWriter(), 2);
    for (int i = 0; i < instructionsLength; ++i) {
        if (instructions[i] == 'c') {
            if (tokenizer.checkCapacity() != data[i]) {
                std::cerr << "Unexpected return value of checkCapacity: " << tokenizer.checkCapacity() << "\n";
                return false;
            }
        } else if (instructions[i] == 'l') {
            tokenizer.dataLost();
        } else if (instructions[i] == 'a') {
            tokenizer.accept(data[i]);
        } else if (instructions[i] == 'o') {
            if (!tokenizer.offer(data[i])) {
                std::cerr << "Unexpected return value of offer: false at index: " << i << "\n";
                return false;
            }
        } else if (instructions[i] == 'r') {
            if (tokenizer.offer(data[i])) {
                std::cerr << "Unexpected return value of offer: true at index: " << i << "\n";
                return false;
            }
        } else {
            std::cerr << "unknown instruction " << (int) instructions[i] << " at index: " << i << "\n";
            return false;
        }
    }
    for (int i = 0; i < dataLengthExpected; ++i) {
        if (ringBuffer[i] != dataExpected[i]) {
            std::cerr << "data mismatch detected\nExpected: " << (int) dataExpected[i] << "\nActual: "
                      << (int) ringBuffer[i] << "\nat index: " << i << "\n";
            return false;
        }
    }
    if (ringBuffer[dataLengthExpected] != 0xAA) {
        std::cerr << "ring buffer data too long, containing: " << (int) ringBuffer[dataLengthExpected] << "\n";
        return false;
    }
    return true;
}

bool shouldMarkOverrunOnce() {
    uint8_t instructions[] = "ll";
    uint8_t data[] = {0, 0};
    uint16_t instructionsLength = 2;
    uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_BUFFER_OVERRUN};
    uint16_t dataLengthExpected = 1;
    return testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected);
}

bool shouldHandleNumericalFields() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "o";
        uint8_t data[] = "Z";
        uint16_t instructionsLength = 1;
        uint8_t dataExpected[] = {'Z', 0};
        uint16_t dataLengthExpected = 2;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Single zero-valued key, no NL\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oo";
        uint8_t data[] = "Z\n";
        uint16_t instructionsLength = 2;
        uint8_t dataExpected[] = {'Z', 0, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 3;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Single zero-valued key with NL\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "A A A\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'A', 0, 'A', 0, 'A', 0, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on 3 empty keys\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooo";
        uint8_t data[] = "AA1Afa\n";
        uint16_t instructionsLength = 7;
        uint8_t dataExpected[] = {'A', 0, 'A', 1, 1, 'A', 1, 0xfa, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 9;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on 3 keys\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "A 12";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'A', 1, 0x12};
        uint16_t dataLengthExpected = 3;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key with 2-nibble value, spaced\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oo";
        uint8_t data[] = "A0";
        uint16_t instructionsLength = 2;
        uint8_t dataExpected[] = {'A', 0};
        uint16_t dataLengthExpected = 2;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key with explicit zero\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "A0a\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'A', 1, 0xa, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key 1-nibble value with leading zero\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooo";
        uint8_t data[] = "A0ab\n";
        uint16_t instructionsLength = 5;
        uint8_t dataExpected[] = {'A', 1, 0xab, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key 2-nibble value with leading zero\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "A0abc\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'A', 2, 0x0a, 0xbc, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 5;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key 3-nibble value with leading zero\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooooo";
        uint8_t data[] = "A000abcd\n";
        uint16_t instructionsLength = 9;
        uint8_t dataExpected[] = {'A', 2, 0xab, 0xcd, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 5;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Key 4-nibble value with leading zeros\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooo";
        uint8_t data[] = "A12345\n\n";
        uint16_t instructionsLength = 8;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_FIELD_TOO_LONG,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END, 0x12, 0x34}; // buffer shrapnel
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Too long number\n\n";
            worked = false;
        }
    }
    return worked;
}

bool shouldRejectInvalidKeys() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "ooooo";
        uint8_t data[] = "A5\f1\n";
        uint16_t instructionsLength = 5;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN, 1,
                                  0x50}; // here we are just measuring shrapnel in the buffer
        uint16_t dataLengthExpected = 3;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal low-value key check\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooo";
        uint8_t data[] = "A5\u0080a\n";
        uint16_t instructionsLength = 5;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN, 1,
                                  0x50}; // here we are just measuring shrapnel in the buffer
        uint16_t dataLengthExpected = 3;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal high-value key check x80\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooo";
        uint8_t data[] = "A5\u00f0a\n";
        uint16_t instructionsLength = 5;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN, 1,
                                  0x50}; // here we are just measuring shrapnel in the buffer
        uint16_t dataLengthExpected = 3;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal high-value key check xf0\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "A5\"\"a\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'A', 1, 0x5, '"', 0, ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal hex key check 'a'\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "A5\"\"f\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'A', 1, 0x5, '"', 0, ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal hex key check 'f'\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "A5\"\"7\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'A', 1, 0x5, '"', 0, ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal hex key check '7'\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oo";
        uint8_t data[] = "7\n";
        uint16_t instructionsLength = 2;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_ILLEGAL_TOKEN};
        uint16_t dataLengthExpected = 1;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Illegal hex key check '7' at start\n\n";
            worked = false;
        }
    }
    return worked;
}

bool shouldHandleLogicalSequencesOfNumericFields() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "Y&Z\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'Y', 0, ZscriptTokenizer<zp>::CMD_END_ANDTHEN, 'Z', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Two keys sep with &\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "Y|Z\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'Y', 0, ZscriptTokenizer<zp>::CMD_END_ORELSE, 'Z', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Two keys sep with |\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooo";
        uint8_t data[] = "A|B&C|D\n";
        uint16_t instructionsLength = 8;
        uint8_t dataExpected[] = {'A', 0, ZscriptTokenizer<zp>::CMD_END_ORELSE,
                                  'B', 0, ZscriptTokenizer<zp>::CMD_END_ANDTHEN,
                                  'C', 0, ZscriptTokenizer<zp>::CMD_END_ORELSE,
                                  'D', 0, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 12;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Several keys sep with several & and |\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "Y(Z\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'Y', 0, ZscriptTokenizer<zp>::CMD_END_OPEN_PAREN, 'Z', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Two keys sep with (\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "Y)Z\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'Y', 0, ZscriptTokenizer<zp>::CMD_END_CLOSE_PAREN, 'Z', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Two keys sep with )\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooo";
        uint8_t data[] = "A(B|C)D&E\n";
        uint16_t instructionsLength = 8;
        uint8_t dataExpected[] = {'A', 0, ZscriptTokenizer<zp>::CMD_END_OPEN_PAREN,
                                  'B', 0, ZscriptTokenizer<zp>::CMD_END_ORELSE,
                                  'C', 0, ZscriptTokenizer<zp>::CMD_END_CLOSE_PAREN,
                                  'D', 0, ZscriptTokenizer<zp>::CMD_END_ANDTHEN,
                                  'E', 0, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 12;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Several keys sep with &, |, ( and )\n\n";
            worked = false;
        }
    }
    return worked;
}

bool shouldHandleBigFields() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "ooooooooo";
        uint8_t data[] = "A0 +1234\n";
        uint16_t instructionsLength = 9;
        uint8_t dataExpected[] = {'A', 0, '+', 2, 0x12, 0x34, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield hex\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooooooo";
        uint8_t data[] = "A1 \"hello\"\n";
        uint16_t instructionsLength = 11;
        uint8_t dataExpected[] = {'A', 1, 1, '"', 5, 'h', 'e', 'l', 'l', 'o',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 11;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield string\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooooooooo";
        uint8_t data[] = "A1 \"\000hel\000lo\"\000\n";
        uint16_t instructionsLength = 14;
        uint8_t dataExpected[] = {'A', 1, 1, '"', 5, 'h', 'e', 'l', 'l', 'o',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 11;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield string with nulls\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooooooooo";
        uint8_t data[] = "A1 \"h=65llo\"\n";
        uint16_t instructionsLength = 13;
        uint8_t dataExpected[] = {'A', 1, 1, '"', 5, 'h', 'e', 'l', 'l', 'o',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 11;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield string with escape\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooo";
        uint8_t data[] = "+12\"a\"\n";
        uint16_t instructionsLength = 7;
        uint8_t dataExpected[] = {'+', 1, 0x12, '"', 1, 'a', ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield hex and string\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooooo";
        uint8_t data[] = "A0 +123\nV\n";
        uint16_t instructionsLength = 10;
        uint8_t dataExpected[] = {'A', 0, ZscriptTokenizer<zp>::ERROR_CODE_ODD_BIGFIELD_LENGTH, 'V', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield odd length NL terminated\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooooooooo";
        uint8_t data[] = "A0 +123ffB\nW\n";
        uint16_t instructionsLength = 13;
        uint8_t dataExpected[] = {'A', 0, ZscriptTokenizer<zp>::ERROR_CODE_ODD_BIGFIELD_LENGTH, 'W', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END, 0xf0};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield odd length token terminated\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooo";
        uint8_t data[] = "\"a\nA\n";
        uint16_t instructionsLength = 5;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_STRING_NOT_TERMINATED, 'A', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield string not terminated\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "ooooooooooo";
        uint8_t data[] = "\"h=6Ao\"A\nB\n";
        uint16_t instructionsLength = 11;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::ERROR_CODE_STRING_ESCAPING, 'B', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Bigfield invalid escape\n\n";
            worked = false;
        }
    }
    return worked;
}

bool shouldHandleComment() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "ooooooooooooo";
        uint8_t data[] = "A0#abc&|+\"\nB\n";
        uint16_t instructionsLength = 13;
        uint8_t dataExpected[] = {'A', 0, ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END, 'B', 0, ZscriptTokenizer<zp
        >::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 6;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Discarding comment\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "#\nX\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END, 'X', 0,
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 4;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Empty comment\n\n";
            worked = false;
        }
    }
    return worked;
}

bool shouldHandleAddressing() {
    bool worked = true;
    if (true) {
        uint8_t instructions[] = "oooo";
        uint8_t data[] = "@2Z\n";
        uint16_t instructionsLength = 4;
        uint8_t dataExpected[] = {'@', 1, 0x2, ZscriptTokenizer<zp>::ADDRESSING_FIELD_KEY, 1, 'Z',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Simple address\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooo";
        uint8_t data[] = "\000@\000a\000Z\000\n";
        uint16_t instructionsLength = 8;
        uint8_t dataExpected[] = {'@', 1, 0xa, ZscriptTokenizer<zp>::ADDRESSING_FIELD_KEY, 1, 'Z',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 7;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Simple address with nulls\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooooooo";
        uint8_t data[] = "@2Z12345\"a=\n";
        uint16_t instructionsLength = 12;
        uint8_t dataExpected[] = {'@', 1, 0x2, ZscriptTokenizer<zp>::ADDRESSING_FIELD_KEY, 9, 'Z', '1', '2', '3', '4',
                                  '5', '"', 'a', '=', ZscriptTokenizer<zp
                >::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 15;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Simple address, complex content\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooo";
        uint8_t data[] = "@2.1Z\n";
        uint16_t instructionsLength = 6;
        uint8_t dataExpected[] = {'@', 1, 0x2, '.', 1, 0x1, ZscriptTokenizer<zp>::ADDRESSING_FIELD_KEY, 1, 'Z',
                                  ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 10;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Multilevel address\n\n";
            worked = false;
        }
    }
    if (true) {
        uint8_t instructions[] = "oooooooooo";
        uint8_t data[] = "@2.3@4.5Z\n";
        uint16_t instructionsLength = 10;
        uint8_t dataExpected[] = {'@', 1, 0x2, '.', 1, 0x3, ZscriptTokenizer<zp>::ADDRESSING_FIELD_KEY, 5, '@', '4',
                                  '.', '5', 'Z', ZscriptTokenizer<zp>::NORMAL_SEQUENCE_END};
        uint16_t dataLengthExpected = 14;
        if (!testBufferContent(instructions, data, instructionsLength, dataExpected, dataLengthExpected)) {
            std::cerr << "Failed on Multilevel address\n\n";
            worked = false;
        }
    }
    return worked;
}
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    if (!Zscript::shouldMarkOverrunOnce()) {
        std::cerr << "Failed on repeated buffer overrun \n";
        return 1;
    }
    if (!Zscript::shouldHandleNumericalFields()) {
        std::cerr << "Failed on simple numerical fields \n";
        return 1;
    }
    if (!Zscript::shouldRejectInvalidKeys()) {
        std::cerr << "Failed on rejecting invalid keys \n";
        return 1;
    }
    if (!Zscript::shouldHandleLogicalSequencesOfNumericFields()) {
        std::cerr << "Failed on handling logical sequences of numeric fields \n";
        return 1;
    }
    if (!Zscript::shouldHandleBigFields()) {
        std::cerr << "Failed on handling big fields \n";
        return 1;
    }
    if (!Zscript::shouldHandleComment()) {
        std::cerr << "Failed on handling comments \n";
        return 1;
    }
    if (!Zscript::shouldHandleAddressing()) {
        std::cerr << "Failed on handling addressing \n";
        return 1;
    }
    return 0;
}


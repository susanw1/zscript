/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */
#include <iostream>

#include "../test-defines.hpp"
#include "../../../../main/c++/zscript/tokenizer/TokenRingBuffer.hpp"

class zp {
public:
    typedef uint16_t tokenBufferSize_t;
};
namespace Zscript {
const int MAX_TOKEN_COUNT = 1000;
const int BUFSIZE = 1000;
long rng = 21434563;

long getNextRng() {
    rng += 9677;
    rng *= 263;
    return rng;
}
void nextRngBytes(uint8_t *data, uint16_t length) {
    for (uint16_t i = 0; i < length; ++i) {
        data[i] = (uint8_t) getNextRng();
    }
}

uint8_t data[BUFSIZE];

GenericCore::TokenRingBuffer<zp> buffer(data, BUFSIZE);
class TokenExpectation {
public:
    bool isMarker;
    bool isExtended;
    uint8_t key;
    uint8_t *data;
    uint16_t dataLen;

    TokenExpectation(bool isMarker, bool isExtended, uint8_t key, uint8_t *data, uint16_t dataLen) :
            isMarker(isMarker), isExtended(isExtended), key(key), data(data), dataLen(dataLen) {
    }
    TokenExpectation() :
            isMarker(false), isExtended(false), key(0), data(NULL), dataLen(0) {
    }

    uint16_t getData16() {
        if (dataLen == 0) {
            return 0;
        } else if (dataLen == 1) {
            return data[0];
        } else {
            return (data[0] << 8) | data[1];
        }
    }
};

TokenExpectation expectations[MAX_TOKEN_COUNT];
int expIndex = 0;

TokenExpectation marker(uint8_t key) {
    return TokenExpectation(true, false, key, NULL, 0);
}

TokenExpectation extended(uint8_t key, uint8_t *data, uint16_t dataLen) {
    return TokenExpectation(false, true, key, data, dataLen);
}

TokenExpectation normal(uint8_t key, uint8_t *data, uint16_t dataLen) {
    return TokenExpectation(false, false, key, data, dataLen);
}

void writeNormalTokens(uint16_t count, uint16_t uint8_tCount) {
    for (uint16_t i = 0; i < count; i++) {
        uint8_t *data = (uint8_t*) malloc(uint8_tCount);
        nextRngBytes(data, uint8_tCount);
        TokenExpectation exp = normal((uint8_t) (getNextRng() & 0x7F), data, uint8_tCount);
        buffer.getWriter().startToken(exp.key, true);
        for (int j = 0; j < uint8_tCount; j++) {
            buffer.getWriter().continueTokenByte(data[j]);
        }
        expectations[expIndex++] = exp;
    }
    buffer.getWriter().endToken();
}

void writeMarkerTokens(uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        TokenExpectation exp = marker((uint8_t) (0xe0 + (getNextRng() & 0x1F)));
        buffer.getWriter().writeMarker(exp.key);
        expectations[expIndex++] = exp;
    }
    buffer.getWriter().endToken();
}

void writeExtendedToken(uint16_t uint8_tCount) {
    uint8_t *data = (uint8_t*) malloc(uint8_tCount);
    nextRngBytes(data, uint8_tCount);
    TokenExpectation exp = extended((uint8_t) (getNextRng() & 0x7F), data, uint8_tCount);
    buffer.getWriter().startToken(exp.key, false);
    for (uint16_t j = 0; j < uint8_tCount; j++) {
        buffer.getWriter().continueTokenByte(data[j]);
    }
    expectations[expIndex++] = exp;
    buffer.getWriter().endToken();
}

bool testTokenSimilarity(GenericCore::RingBufferToken<zp> found, TokenExpectation expected) {
    if (found.isMarker(&buffer) != expected.isMarker) {
        std::cerr << "isMarker did not match\n";
        return false;
    }
    if (found.getKey(&buffer) != expected.key) {
        std::cerr << "key did not match\n";
        return false;
    }
    if (!found.isMarker(&buffer)) {
        if (expected.dataLen <= 2) {
            uint16_t exp16 = 0;
            for (int i = 0; i < expected.dataLen; ++i) {
                exp16 <<= 8;
                exp16 += expected.data[i];
            }
            if (found.getData16(&buffer) != exp16) {
                std::cerr << "data as uint16_t did not match\n";
                return false;
            }
        }
        GenericCore::RawTokenBlockIterator<zp> tbi = found.rawBlockIterator(&buffer);
        for (uint16_t i = 0; i < expected.dataLen; ++i) {
            if (!tbi.hasNext(&buffer)) {
                std::cerr << "iterator exited early\n";
                return false;
            }
            uint8_t value = tbi.next(&buffer);
            if (value != expected.data[i]) {
                std::cerr << "wrong data value on byte: " << i << "\nExpected: " << (uint16_t) expected.data[i] << "\nActual: " << (uint16_t) value << "\n";
                return false;
            }
        }
        if (tbi.hasNext(&buffer)) {
            std::cerr << "iterator exited late\n";
            return false;
        }

    }
    if (expected.isExtended) {
        if (found.getDataSize(&buffer) != expected.dataLen) {
            std::cerr << "Reported length does not match\n";
            return false;
        }
    }
    return true;
}

bool testIteratorCorrectness(GenericCore::RingBufferTokenIterator<zp> iterator, int offset) {
    int i = offset;

    for (GenericCore::OptionalRingBufferToken<zp> opt = iterator.next(&buffer); opt.isPresent; i++, opt = iterator.next(&buffer)) {
        if (i > expIndex) {
            std::cerr << "Iterator produced more Tokens than expected\n";
            return false;
        }
        if (!testTokenSimilarity(opt.token, expectations[i])) {
            std::cerr << "On token: " << i << "\n";
            return false;
        }
    }
    if (i != expIndex) {
        std::cerr << "Iterator produced fewer Tokens than expected\n";
        return false;
    }
    return true;
}

bool shouldIterateNormalTokensCorrectly() {
    writeNormalTokens(100, 2);
    if (!buffer.getReader().hasReadToken()) {
        std::cerr << "Buffer not reporting read token\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getKey(&buffer) != expectations[0].key) {
        std::cerr << "First read token does not have correct key\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getData16(&buffer) != expectations[0].getData16()) {
        std::cerr << "First read token does not have correct value:\nExpected: " << expectations[0].getData16()
                << "\nActual: " << buffer.getReader().getFirstReadToken().getData16(&buffer) << "\n";
        return false;
    }
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 0);
}

bool shouldIterateMarkerTokensCorrectly() {
    writeMarkerTokens(800);
    if (!buffer.getReader().hasReadToken()) {
        std::cerr << "Buffer not reporting read token\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getKey(&buffer) != expectations[0].key) {
        std::cerr << "First read token does not have correct key\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getData16(&buffer) != expectations[0].getData16()) {
        std::cerr << "First read token does not have correct value\n";
        return false;
    }
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 0);
}

bool shouldIterateMixedTokensCorrectly() {
    writeNormalTokens(10, 2);
    writeMarkerTokens(50);
    writeNormalTokens(10, 2);
    writeExtendedToken(400);
    writeNormalTokens(10, 2);
    if (!buffer.getReader().hasReadToken()) {
        std::cerr << "Buffer not reporting read token\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getKey(&buffer) != expectations[0].key) {
        std::cerr << "First read token does not have correct key\n";
        return false;
    }
    if (buffer.getReader().getFirstReadToken().getData16(&buffer) != expectations[0].getData16()) {
        std::cerr << "First read token does not have correct value\n";
        return false;
    }
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 0);
}

bool flushFirstShouldFlush() {
    writeNormalTokens(10, 2);
    writeMarkerTokens(50);
    writeNormalTokens(10, 2);
    writeExtendedToken(400);
    writeNormalTokens(10, 2);
    buffer.getReader().flushFirstReadToken();
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 1);
}

bool iteratorsStartedFromTokensShouldMatch() {
    writeNormalTokens(10, 2);
    writeMarkerTokens(50);
    writeNormalTokens(10, 2);
    writeExtendedToken(400);
    writeNormalTokens(10, 2);
    int i = 0;

    GenericCore::RingBufferTokenIterator<zp> iterator = buffer.getReader().rawIterator();
    for (GenericCore::OptionalRingBufferToken<zp> opt = iterator.next(&buffer); opt.isPresent; i++, opt = iterator.next(&buffer)) {
        if (!testIteratorCorrectness(opt.token.getNextTokens(), i)) {
            return false;
        }
    }
    return true;
}

bool shouldIterateTokenDataThroughExtensions() {
    writeExtendedToken(800);
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 0);
}

bool shouldIterateTokenDataThroughLoopingBuffer() {
    writeExtendedToken(800);
    GenericCore::RingBufferTokenIterator<zp> it = buffer.getReader().rawIterator();
    it.next(&buffer);
    it.flushBuffer(&buffer);
    writeExtendedToken(800);
    return testIteratorCorrectness(buffer.getReader().rawIterator(), 1);
}

bool shouldIterateTokenDataInContiguousChunks() {
    writeExtendedToken(255);
    GenericCore::RawTokenBlockIterator<zp> iterator = buffer.getReader().rawIterator().next(&buffer).token.rawBlockIterator(&buffer);
    uint16_t i = 0;
    for (; iterator.hasNext(&buffer);) {
        DataArrayWLeng16 nextCont = iterator.nextContiguous(&buffer);
        if (nextCont.length != (i + 255 > 800 ? 800 - i : 255)) {
            std::cerr << "Contiguous segment length wrong\nExpected: " << (i + 255 > 800 ? 800 - i : 255) << "\nActual: " << nextCont.length << "\n";
            return false;
        }
        for (uint16_t j = 0; j < nextCont.length; ++j) {
            if (nextCont.data[j] != expectations[0].data[i + j]) {
                std::cerr << "Contiguous segment value wrong\n";
                return false;
            }
        }
        i += 255;
    }
    return true;
}

bool shouldIterateTokenDataInLimtedContiguousChunks() {
    writeExtendedToken(800);
    GenericCore::RawTokenBlockIterator<zp> iterator = buffer.getReader().rawIterator().next(&buffer).token.rawBlockIterator(&buffer);
    uint16_t i = 0;
    for (; iterator.hasNext(&buffer);) {
        DataArrayWLeng16 nextCont = iterator.nextContiguous(&buffer, 200);
        if (nextCont.length != (i + 200 > 800 ? 800 - i : 200)) {
            std::cerr << "Contiguous segment length wrong\n";
            return false;
        }
        for (uint16_t j = 0; j < nextCont.length; ++j) {
            if (nextCont.data[j] != expectations[0].data[i + j]) {
                std::cerr << "Contiguous segment value wrong\n";
                return false;
            }
        }
        if (iterator.hasNext(&buffer)) {
            nextCont = iterator.nextContiguous(&buffer, 200);
            if (nextCont.length != (i + 255 > 800 ? 800 - (i + 200) : 55)) {
                std::cerr << "Contiguous segment length wrong\n";
                return false;
            }
            for (uint16_t j = 0; j < nextCont.length; ++j) {
                if (nextCont.data[j] != expectations[0].data[i + 200 + j]) {
                    std::cerr << "Contiguous segment value wrong (2)\n";
                    return false;
                }
            }
        }
        i += 255;
    }
    return true;
}

bool shouldIterateTokenDataInLimtedContiguousChunksAroundEnd() {
    uint16_t initialLength = 700;
    writeExtendedToken(initialLength);
    GenericCore::RingBufferTokenIterator<zp> it = buffer.getReader().rawIterator();
    it.next(&buffer);
    it.flushBuffer(&buffer);
    writeExtendedToken(800);
    GenericCore::RawTokenBlockIterator<zp> iterator = buffer.getReader().rawIterator().next(&buffer).token.rawBlockIterator(&buffer);
    int i = 0;
    for (; iterator.hasNext(&buffer);) {
        DataArrayWLeng16 nextCont = iterator.nextContiguous(&buffer, 200);
        int offset = 200;
        if (i + initialLength + (initialLength + 254) / 255 * 2 + i / 255 * 2 < BUFSIZE
                && i + 200 + initialLength + (initialLength + 254) / 255 * 2 + i / 255 * 2 + 2 > BUFSIZE) {
            offset = BUFSIZE - (initialLength + i + (initialLength + 254) / 255 * 2 + i / 255 * 2 + 2);
        }
        if (nextCont.length != (i + offset > 800 ? 800 - i : offset)) {
            std::cerr << "Contiguous segment length wrong\n";
            return false;
        }
        for (uint16_t j = 0; j < nextCont.length; ++j) {
            if (nextCont.data[j] != expectations[1].data[i + j]) {
                std::cerr << "Contiguous segment value wrong\n";
                return false;
            }
        }
        if (iterator.hasNext(&buffer)) {
            if (1 + offset > 800) {
                std::cerr << "Iterator didn't terminate when it should have\n";
                return false;

            }
            if (offset + 200 < 255) {
                nextCont = iterator.nextContiguous(&buffer, 200);
                if (nextCont.length != (i + offset + 200 > 800 ? 800 - (i + offset) : 200)) {
                    std::cerr << "Contiguous segment length wrong\n";
                    return false;
                }
                for (uint16_t j = 0; j < nextCont.length; ++j) {
                    if (nextCont.data[j] != expectations[1].data[i + offset + j]) {
                        std::cerr << "Contiguous segment value wrong\n";
                        return false;
                    }
                }
                offset += 200;
            }
            nextCont = iterator.nextContiguous(&buffer, 200);
            if (nextCont.length != (i + 255 > 800 ? 800 - (i + offset) : 255 - offset)) {
                std::cerr << "Contiguous segment length wrong\n";
                return false;
            }
            for (uint16_t j = 0; j < nextCont.length; ++j) {
                if (nextCont.data[j] != expectations[1].data[i + offset + j]) {
                    std::cerr << "Contiguous segment value wrong\n";
                    return false;
                }
            }
        }
        i += 255;
    }
    return true;
}

}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    Zscript::GenericCore::TokenRingBuffer<zp> blank(Zscript::data, Zscript::BUFSIZE);
    if (!Zscript::shouldIterateNormalTokensCorrectly()) {
        std::cerr << "Failed on iterating normal tokens \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateMarkerTokensCorrectly()) {
        std::cerr << "Failed on iterating marker tokens \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateMixedTokensCorrectly()) {
        std::cerr << "Failed on iterating mixed tokens \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::flushFirstShouldFlush()) {
        std::cerr << "Failed on flushFirst \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::iteratorsStartedFromTokensShouldMatch()) {
        std::cerr << "Failed on starting iterators from tokens \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateTokenDataThroughExtensions()) {
        std::cerr << "Failed on extension tokens \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateTokenDataThroughLoopingBuffer()) {
        std::cerr << "Failed on buffer loops \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateTokenDataInContiguousChunks()) {
        std::cerr << "Failed on contiguous chunks \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateTokenDataInLimtedContiguousChunks()) {
        std::cerr << "Failed on limited contiguous chunks \n";
        return 1;
    }
    Zscript::expIndex = 0;
    Zscript::buffer = blank;
    if (!Zscript::shouldIterateTokenDataInLimtedContiguousChunksAroundEnd()) {
        std::cerr << "Failed on limited contiguous chunks while looping \n";
        return 1;
    }
    return 0;
}

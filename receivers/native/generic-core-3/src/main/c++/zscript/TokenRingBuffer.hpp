/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_TOKENRINGBUFFER_HPP_
#define SRC_MAIN_C___ZSCRIPT_TOKENRINGBUFFER_HPP_

#include "ZscriptIncludes.hpp"
#include "TokenBufferFlags.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class RingBufferToken;

template<class ZP>
class RingBufferTokenIterator;

template<class ZP>
class RingBufferToken;

template<class ZP>
class RawTokenBlockIterator;

template<class ZP>
class TokenRingBuffer {
    friend class RingBufferToken<ZP> ;
    friend class RingBufferTokenIterator<ZP> ;
    friend class RawTokenBlockIterator<ZP> ;
    public:
    /**
     * Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
     */
    static const uint8_t TOKEN_EXTENSION = 0x81;

    /**
     * Determines whether the supplied byte is a valid "marker" key.
     *
     * @param key any byte
     * @return true if it's a marker; false otherwise
     */
    static bool isMarker(uint8_t key) {
        return (key & 0xe0) == 0xe0;
    }

    static bool isSequenceEndMarker(uint8_t key) {
        return (key & 0xf0) == 0xf0;
    }
private:
    static const uint8_t MAX_TOKEN_DATA_LENGTH = 255;

    uint8_t *data;
    uint16_t data_length;

    uint16_t readStart = 0;
    uint16_t writeStart = 0;

    //Writer:
    /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
    uint16_t writeLastLen = 0;
    /** the current write index into data array */
    uint16_t writeCursor = 0;

    bool inNibble = false;
    bool numeric = false;
    ZcodeTokenBufferFlags<ZP> flags;

    uint16_t offset(uint16_t index, uint16_t offset) {
        return (uint16_t) (index + offset) % data_length;
    }

    void W_writeNewTokenStart(uint8_t key) {
        data[writeCursor] = key;
        W_moveCursor();
        data[writeCursor] = 0;
        writeLastLen = writeCursor;
        W_moveCursor();
    }

    void W_moveCursor() {
        writeCursor = offset(writeCursor, 1);
    }

public:

    TokenRingBuffer(uint8_t *data, uint16_t data_length) :
            data(data), data_length(data_length) {
    }

    ZcodeTokenBufferFlags<ZP>* getFlags() {
        return &flags;
    }

    //Writer:
    void W_startToken(uint8_t key, bool numeric) {
        W_endToken();
        this->numeric = numeric;
        W_writeNewTokenStart(key);
    }

    void W_continueTokenNibble(uint8_t nibble) {
        if (inNibble) {
            data[writeCursor] |= nibble;
            W_moveCursor();
        } else {
            if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
                W_writeNewTokenStart(TOKEN_EXTENSION);
            }
            data[writeCursor] = nibble << 4;
            data[writeLastLen]++;
        }
        inNibble = !inNibble;
    }
    void W_continueTokenByte(uint8_t b) {
        if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
            W_writeNewTokenStart(TOKEN_EXTENSION);
        }
        data[writeCursor] = b;
        W_moveCursor();
        data[writeLastLen]++;
    }
    void W_endToken() {
        if (inNibble) {
            if (numeric) {
                // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
                uint8_t hold = 0;
                uint16_t pos = offset(writeStart, 1);
                do {
                    pos = offset(pos, 1);
                    uint8_t tmp = data[pos] & 0xF;
                    data[pos] = hold | ((data[pos] >> 4) & 0xF);
                    hold = tmp << 4;
                } while (pos != writeCursor);
            }
            W_moveCursor();
        }

        writeStart = writeCursor;
        inNibble = false;
    }

    bool W_checkAvailableCapacity(uint16_t size) {
        return ((writeCursor >= readStart ? data_length : 0) + readStart - writeCursor - 1) >= size;
    }

    bool W_isInNibble() {
        return inNibble;
    }

    uint8_t W_getCurrentWriteTokenKey() {
        return data[writeStart];
    }

    uint8_t W_getCurrentWriteTokenLength() {
        return data[offset(writeStart, 1)];
    }

    bool W_isTokenComplete() {
        return writeStart == writeCursor;
    }

    void W_writeMarker(uint8_t key) {
        if (isSequenceEndMarker(key)) {
            flags.setSeqMarkerWritten();
            //TODO: this
        }
        flags.setMarkerWritten();
        W_endToken();
        data[writeCursor] = key;
        W_moveCursor();
        W_endToken();
    }
    void W_fail(uint8_t errorCode) {
        if (!W_isTokenComplete()) {
            // reset current token back to writeStart
            writeCursor = writeStart;
            inNibble = false;
        }
        W_writeMarker(errorCode);
    }

    RingBufferTokenIterator<ZP> R_iterator() {
        return RingBufferTokenIterator<ZP>(readStart);
    }

    bool R_hasReadToken() {
        return readStart != writeStart;
    }

    RingBufferToken<ZP> R_getFirstReadToken() {
        return RingBufferToken<ZP>(readStart);
    }

    void R_flushFirstReadToken();
};
template<class ZP>
struct OptionalRingBufferToken {
    RingBufferToken<ZP> token;
    bool isPresent;
};

template<class ZP>
class RawTokenBlockIterator {
    uint16_t itIndex;
    uint8_t segRemaining;

public:

    RawTokenBlockIterator(uint16_t itIndex, uint8_t segRemaining) :
            itIndex(itIndex), segRemaining(segRemaining) {
    }

    uint8_t next(TokenRingBuffer<ZP> *buffer) {
        if (!hasNext(buffer)) {
//TODO: die
        }
        uint8_t res = buffer->data[itIndex];
        itIndex = buffer->offset(itIndex, 1);
        segRemaining--;
        return res;
    }

    bool hasNext(TokenRingBuffer<ZP> *buffer) {
        // Copes with empty tokens which still have extensions... probably not a valid thing anyway, but still, it didn't cost anything.
        while (segRemaining == 0) {
            if ((itIndex == buffer->writeStart) || (buffer->data[itIndex] != TokenRingBuffer<ZP>::TOKEN_EXTENSION)) {
                return false;
            }
            segRemaining = buffer->data[buffer->offset(itIndex, 1)];
            itIndex = buffer->offset(itIndex, 2);
        }
        return true;
    }

    DataArrayWLeng16 nextContiguous(TokenRingBuffer<ZP> *buffer) {
        return nextContiguous(buffer, 0xff);
    }

    DataArrayWLeng16 nextContiguous(TokenRingBuffer<ZP> *buffer, uint8_t maxLength) {
        if (!hasNext(buffer)) {
            //TODO: die
        }
        uint16_t length = buffer->data_length - itIndex;
        if (segRemaining < length) {
            length = segRemaining;
        }
        if (maxLength < length) {
            length = maxLength;
        }
        uint8_t *dataArray = buffer->data + itIndex;

        itIndex = buffer->offset(itIndex, length);
        segRemaining -= (uint8_t) length;
        return {dataArray, length};
    }
};

template<class ZP>
class RingBufferTokenIterator {
    uint16_t index;

public:

    RingBufferTokenIterator(uint16_t index) :
            index(index) {
    }

    OptionalRingBufferToken<ZP> next(TokenRingBuffer<ZP> *buffer) {
        if (index == buffer->writeStart) {
            return {RingBufferToken<ZP>(0), false};
        }

        uint16_t initialIndex = index;
        if (TokenRingBuffer<ZP>::isMarker(buffer->data[index])) {
            index = buffer->offset(index, 1);
        } else {
            do {
                uint16_t tokenDataLength = buffer->data[buffer->offset(index, 1)];
                index = buffer->offset(index, tokenDataLength + 2);
            } while (buffer->data[index] == TokenRingBuffer<ZP>::TOKEN_EXTENSION);
        }

        return {RingBufferToken<ZP>(initialIndex), true};
    }

    void flushBuffer(TokenRingBuffer<ZP> *buffer) {
        buffer->readStart = index;
    }
};

template<class ZP>
class RingBufferToken {
    friend class TokenRingBuffer<ZP> ;
    friend class RingBufferTokenIterator<ZP> ;

private:
    uint16_t index;

    RingBufferToken(uint16_t index) :
            index(index) {
    }

    uint8_t getSegmentDataSize(TokenRingBuffer<ZP> *buffer) {
        return TokenRingBuffer<ZP>::isMarker(buffer->data[index]) ? 0 : buffer->data[buffer->offset(index, 1)];
    }

public:
    RingBufferTokenIterator<ZP> getNextTokens() {
        return RingBufferTokenIterator<ZP>(index);
    }

    RawTokenBlockIterator<ZP> blockIterator(TokenRingBuffer<ZP> *buffer) {
        return RawTokenBlockIterator<ZP>(buffer->offset(index, 2), buffer->data[buffer->offset(index, 1)]);
    }

    uint16_t getData16(TokenRingBuffer<ZP> *buffer) {
        uint16_t value = 0;
        for (uint16_t i = 0; i < getSegmentDataSize(buffer); i++) {
            // Check before we left shift. Avoids overflowing data type.
            value <<= 8;
            value += buffer->data[buffer->offset(index, i + 2)];
        }
        return value;
    }

    uint8_t getKey(TokenRingBuffer<ZP> *buffer) {
        return buffer->data[index];
    }

    uint16_t getDataSize(TokenRingBuffer<ZP> *buffer) {
        if (isMarker(buffer)) {
            return 0;
        }
        uint16_t totalSz = 0;
        uint16_t index = this->index;

        do {
            uint8_t segSz = buffer->data[buffer->offset(index, 1)];
            totalSz += segSz;
            index = buffer->offset(index, segSz + 2);
        } while (index != buffer->writeStart && buffer->data[index] == TokenRingBuffer<ZP>::TOKEN_EXTENSION);

        return totalSz;
    }
    bool isMarker(TokenRingBuffer<ZP> *buffer) {
        return TokenRingBuffer<ZP>::isMarker(getKey(buffer));
    }
    bool isSequenceEndMarker(TokenRingBuffer<ZP> *buffer) {
        return TokenRingBuffer<ZP>::isSequenceEndMarker(getKey(buffer));
    }
};
template<class ZP>
void TokenRingBuffer<ZP>::R_flushFirstReadToken() {
    RingBufferTokenIterator<ZP> it(readStart);
    it.next(this);
    it.flushBuffer(this);
}
}
}
#endif /* SRC_MAIN_C___ZSCRIPT_TOKENRINGBUFFER_HPP_ */

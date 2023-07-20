/*
 * ZcodeTokenRingBuffer.hpp
 *
 *  Created on: 18 Jul 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ZCODE_ZCODETOKENRINGBUFFER_HPP_
#define SRC_MAIN_C___ZCODE_ZCODETOKENRINGBUFFER_HPP_

#include "ZcodeIncludes.hpp"

template<class ZP>
class RingBufferToken;

template<class ZP>
class ZcodeTokenRingBuffer {
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
    ZcodeTokenRingBuffer(uint8_t *data, uint16_t data_length) :
            data(data), data_length(data_length) {
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
//            flags.setSeqMarkerWritten();
            //TODO: this
        }
//        flags.setMarkerWritten();
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

    uint8_t next(ZcodeTokenRingBuffer<ZP> token) {
        if (!hasNext(token)) {
//TODO: die
        }
        uint8_t res = token->data[itIndex];
        itIndex = token->offset(itIndex, 1);
        segRemaining--;
        return res;
    }

    bool hasNext(ZcodeTokenRingBuffer<ZP> token) {
        // Copes with empty tokens which still have extensions... probably not a valid thing anyway, but still, it didn't cost anything.
        while (segRemaining == 0) {
            if ((itIndex == token->writeStart) || (token->data[itIndex] != ZcodeTokenRingBuffer<ZP>::TOKEN_EXTENSION)) {
                return false;
            }
            segRemaining = token->data[ZcodeTokenRingBuffer<ZP>::offset(itIndex, 1)];
            itIndex = ZcodeTokenRingBuffer<ZP>::offset(itIndex, 2);
        }
        return true;
    }

    DataArrayWLeng16 nextContiguous(ZcodeTokenRingBuffer<ZP> token) {
        return nextContiguous(token, 0xffff);
    }

    DataArrayWLeng16 nextContiguous(ZcodeTokenRingBuffer<ZP> token, uint16_t maxLength) {
        if (maxLength < 0) {
            //TODO: die
        }
        if (!hasNext(token)) {
            //TODO: die
        }
        uint16_t length = token->data_length - itIndex;
        if (segRemaining < length) {
            length = segRemaining;
        }
        if (maxLength < length) {
            length = maxLength;
        }
        uint8_t *dataArray = token->data + itIndex;

        itIndex = ZcodeTokenRingBuffer<ZP>::offset(itIndex, length);
        segRemaining -= length;
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

    OptionalRingBufferToken<ZP> next(ZcodeTokenRingBuffer<ZP> *buffer) {
        if (index == buffer->writeStart) {
            return {RingBufferToken<ZP>(0), false};
        }

        int initialIndex = index;
        if (ZcodeTokenRingBuffer<ZP>::isMarker(buffer->data[index])) {
            index = ZcodeTokenRingBuffer<ZP>::offset(index, 1);
        } else {
            do {
                int tokenDataLength = buffer->data[ZcodeTokenRingBuffer<ZP>::offset(index, 1)];
                index = ZcodeTokenRingBuffer<ZP>::offset(index, tokenDataLength + 2);
            } while (buffer->data[index] == ZcodeTokenRingBuffer<ZP>::TOKEN_EXTENSION);
        }

        return {RingBufferToken<ZP>(initialIndex), true};
    }

    void flushBuffer(ZcodeTokenRingBuffer<ZP> *buffer) {
        buffer->readStart = index;
    }
};

template<class ZP>
class RingBufferToken {
private:
    uint16_t index;

    RingBufferToken(uint16_t index) :
            index(index) {
    }

    uint8_t getSegmentDataSize(ZcodeTokenRingBuffer<ZP> *buffer) {
        return ZcodeTokenRingBuffer<ZP>::isMarker(buffer->data[index]) ? 0 : buffer->data[ZcodeTokenRingBuffer<ZP>::offset(index, 1)];
    }

public:
    RingBufferTokenIterator<ZP> getNextTokens() {
        return RingBufferTokenIterator<ZP>(index);
    }

    RawTokenBlockIterator<ZP> blockIterator(ZcodeTokenRingBuffer<ZP> *buffer) {
        return RawTokenBlockIterator<ZP>(index, buffer->data[ZcodeTokenRingBuffer<ZP>::offset(index, 1)]);
    }

    uint16_t getData16(ZcodeTokenRingBuffer<ZP> *buffer) {
        uint16_t value = 0;
        for (int i = 0; i < getSegmentDataSize(buffer); i++) {
            // Check before we left shift. Avoids overflowing data type.
            value <<= 8;
            value += buffer->data[ZcodeTokenRingBuffer<ZP>::offset(index, i + 2)];
        }
        return value;
    }

    uint8_t getKey(ZcodeTokenRingBuffer<ZP> *buffer) {
        return buffer->data[index];
    }

    uint16_t getDataSize(ZcodeTokenRingBuffer<ZP> *buffer) {
        if (isMarker(buffer)) {
            return 0;
        }
        uint16_t totalSz = 0;
        uint16_t index = this->index;

        do {
            uint8_t segSz = buffer->data[ZcodeTokenRingBuffer<ZP>::offset(index, 1)];
            totalSz += segSz;
            index = ZcodeTokenRingBuffer<ZP>::offset(index, segSz + 2);
        } while (index != buffer->writeStart && buffer->data[index] == ZcodeTokenRingBuffer<ZP>::TOKEN_EXTENSION);

        return totalSz;
    }
    bool isMarker(ZcodeTokenRingBuffer<ZP> *buffer) {
        return ZcodeTokenRingBuffer<ZP>::isMarker(getKey(buffer));
    }

};
#endif /* SRC_MAIN_C___ZCODE_ZCODETOKENRINGBUFFER_HPP_ */

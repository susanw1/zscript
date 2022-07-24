/*
 * ZcodeBigField.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODEBIGFIELD_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODEBIGFIELD_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeBigField {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;

    uint8_t *big;
    bigFieldAddress_t capacity;
    bigFieldAddress_t pos;
    uint8_t currentHex;
    bool inNibble;

public:
    ZcodeBigField(uint8_t *big, bigFieldAddress_t capacity) :
            big(big), capacity(capacity), pos(0), currentHex(0), inNibble(false) {

    }

    const uint8_t* getData() const {
        return big;
    }
    bigFieldAddress_t getLength() const {
        return pos;
    }

    void reset() {
        pos = 0;
        currentHex = 0;
        inNibble = 0;
    }
    bool isAtEnd() {
        return pos == capacity;
    }

    bool addByte(uint8_t c) {
        if (pos == capacity || inNibble) {
            return false;
        }
        big[pos++] = c;
        return true;
    }

    bool addNibble(uint8_t nibble) {
        if (pos == capacity) {
            return false;
        }
        if (inNibble) {
            currentHex = (uint8_t) (currentHex << 4);
            currentHex |= nibble;
            big[pos++] = currentHex;
            currentHex = 0;
        } else {
            currentHex = nibble;
        }
        inNibble = !inNibble;
        return true;
    }
    bool isInNibble() {
        return inNibble;
    }
    void copyTo(ZcodeOutStream<ZP> *out) const {
        if (pos > 0) {
            out->writeBigHexField(big, pos);
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEBIGFIELD_HPP_ */

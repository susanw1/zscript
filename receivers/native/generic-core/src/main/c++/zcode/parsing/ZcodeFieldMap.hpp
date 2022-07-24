/*
 * ZcodeFieldMap.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeFieldMap {
private:
    typedef typename ZP::fieldMapSize_t fieldMapSize_t;

    uint16_t values[ZP::fieldNum];
    char fields[ZP::fieldNum];
    fieldMapSize_t size = 0;
    uint8_t pos = 0;

public:
    bool add16(char f, uint16_t v) {
        pos = 0;
        if (size == ZP::fieldNum) {
            return false;
        }
        for (uint8_t i = 0; i < size; ++i) {
            if (fields[i] == f) {
                return false;
            }
        }
        fields[size] = f;
        values[size++] = v;
        return true;
    }
    bool addBlank(char f) {
        pos = 0;
        if (size == ZP::fieldNum) {
            return false;
        }
        for (uint8_t i = 0; i < size; ++i) {
            if (fields[i] == f) {
                return false;
            }
        }
        fields[size] = f;
        values[size] = 0;
        size++;
        return true;
    }

    bool add4(uint8_t nibble) {
        if (pos >= 4) {
            return false;
        }
        values[size - 1] = (uint16_t) ((values[size - 1] << 4) | nibble);
        pos++;
        return true;
    }

    bool add8(uint8_t byte) {
        if (pos >= 3) {
            return false;
        }
        values[size - 1] = (uint16_t) ((values[size - 1] << 8) | byte);
        pos += 2;
        return true;
    }

    bool has(char f) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                return true;
            }
        }
        return false;
    }

    uint16_t get(char f, uint16_t def) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                return values[i];
            }
        }
        return def;
    }

    bool get(char f, uint16_t *dest) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                *dest = values[i];
                return true;
            }
        }
        return false;
    }

    int32_t get(char f) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                return values[i];
            }
        }
        return -1;
    }

    fieldMapSize_t getFieldCount() const {
        return size;
    }

    void reset() {
        size = 0;
        pos = 0;
    }

    void copyFieldTo(ZcodeOutStream<ZP> *out, char c) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == c) {
                out->writeField16(c, values[i]);
            }
        }
    }

    void copyTo(ZcodeOutStream<ZP> *out) const {
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] != Zchars::ECHO_PARAM && fields[i] != Zchars::CMD_PARAM && fields[i] != Zchars::STATUS_RESP_PARAM) {
                out->writeField16(fields[i], values[i]);
            }
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_ */

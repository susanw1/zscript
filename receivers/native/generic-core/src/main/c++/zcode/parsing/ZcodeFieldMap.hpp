/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_
#define SRC_MAIN_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_

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
    uint8_t nibbleCount = 0;

public:
    bool add16(char f, uint16_t v) {
        nibbleCount = 0;
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
        return add16(f, 0);
    }

    bool add4(uint8_t nibble) {
        if (nibbleCount >= 4) {
            return false;
        }
        values[size - 1] = (uint16_t) ((values[size - 1] << 4) | nibble);
        nibbleCount++;
        return true;
    }

    bool add8(uint8_t byte) {
        if (nibbleCount >= 3) {
            return false;
        }
        values[size - 1] = (uint16_t) ((values[size - 1] << 8) | byte);
        nibbleCount += 2;
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
        nibbleCount = 0;
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

#endif /* SRC_MAIN_CPP_ZCODE_PARSING_ZCODEFIELDMAP_HPP_ */

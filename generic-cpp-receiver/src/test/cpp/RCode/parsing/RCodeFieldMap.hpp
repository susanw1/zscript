/*
 * RCodeFieldMap.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_

#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"

class RCodeOutStream;

class RCodeFieldMap {
private:
    char fields[RCodeParameters::fieldNum];
    fieldUnit values[RCodeParameters::fieldNum];
    uint8_t size = 0;
    mutable char lastSearchedField = 0;
    mutable fieldUnit lastFoundValue = 0;
public:
    bool add(char f, fieldUnit v) {
        if (size == RCodeParameters::fieldNum) {
            return false;
        }
        fields[size] = f;
        values[size++] = v;
        lastSearchedField = f;
        lastFoundValue = v;
        return true;
    }

    bool has(char f) const {
        if (RCodeParameters::fieldNum && f == lastSearchedField) {
            return true;
        }
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (RCodeParameters::cacheFieldResults) {
                    lastSearchedField = f;
                    lastFoundValue = values[i];
                }
                return true;
            }
        }
        return false;
    }
    bool has(char f, int fieldSectionNum) const {
        int j = 0;
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (j++ == fieldSectionNum) {
                    return true;
                }
            }
        }
        return false;
    }
    fieldUnit get(char f, fieldUnit def) const {
        if (RCodeParameters::cacheFieldResults && f == lastSearchedField) {
            return lastFoundValue;
        }
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (RCodeParameters::cacheFieldResults) {
                    lastSearchedField = f;
                    lastFoundValue = values[i];
                }
                return values[i];
            }
        }
        return def;
    }
    fieldUnit get(char f, int fieldSectionNum, fieldUnit def) const {
        int j = 0;
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (j++ == fieldSectionNum) {
                    return values[i];
                }
            }
        }
        return def;
    }
    uint8_t countFieldSections(char f) const {
        uint8_t j = 0;
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                j++;
            }
        }
        return j;
    }

    uint8_t getFieldCount() const {
        return size;
    }

    void reset() {
        size = 0;
        if (RCodeParameters::cacheFieldResults) {
            lastSearchedField = 0;
            lastFoundValue = 0;
        }
    }

    void copyFieldTo(RCodeOutStream *out, char c) const;

    void copyTo(RCodeOutStream *out) const;
};

#include "../RCodeOutStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_ */

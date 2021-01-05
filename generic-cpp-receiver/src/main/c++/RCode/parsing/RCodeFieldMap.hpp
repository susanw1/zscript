/*
 * RCodeFieldMap.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_

#include "../RCodeIncludes.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeFieldMap {
    typedef typename RP::fieldUnit_t fieldUnit_t;
    typedef typename RP::fieldMapSize_t fieldMapSize_t;

private:
    char fields[RP::fieldNum];
    fieldUnit_t values[RP::fieldNum];
    fieldMapSize_t size = 0;
    mutable char lastSearchedField = 0;
    mutable fieldUnit_t lastFoundValue = 0;
    public:
    bool add(char f, fieldUnit_t v) {
        if (size == RP::fieldNum) {
            return false;
        }
        fields[size] = f;
        values[size++] = v;
        lastSearchedField = f;
        lastFoundValue = v;
        return true;
    }

    bool has(char f) const {
        if (RP::fieldNum && f == lastSearchedField) {
            return true;
        }
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (RP::cacheFieldResults) {
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

    fieldUnit_t get(char f, fieldUnit_t def) const {
        if (RP::cacheFieldResults && f == lastSearchedField) {
            return lastFoundValue;
        }
        for (uint8_t i = 0; i < size; i++) {
            if (fields[i] == f) {
                if (RP::cacheFieldResults) {
                    lastSearchedField = f;
                    lastFoundValue = values[i];
                }
                return values[i];
            }
        }
        return def;
    }

    fieldUnit_t get(char f, int fieldSectionNum, fieldUnit_t def) const {
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

    uint8_t getByte(char f, uint8_t byteNum, uint8_t def) const {
        if (sizeof(fieldUnit_t) == 1) {
            return get(f, byteNum, def);
        }
        fieldUnit_t start = get(f, def);
        uint8_t startOffset = sizeof(fieldUnit_t);
        for (; start != 0; start = (uint8_t) (start >> 8))
            startOffset--;
        fieldUnit_t found;
        if (byteNum + startOffset < sizeof(fieldUnit_t)) {
            found = start;
        } else {
            found = get(f, (byteNum + startOffset) / sizeof(fieldUnit_t), def);
        }
        if (found == def) {
            if (!has(f)) {
                return def;
            }
        }
        return (found >> (8 * ((byteNum + startOffset) % sizeof(fieldUnit_t)))) & 0xFF;
    }

    uint8_t getByteCount(char f) const {
        uint8_t sections = countFieldSections(f);
        if (sizeof(fieldUnit_t) == 1) {
            return sections;
        }

        int startOffset = sizeof(fieldUnit_t);
        for (fieldUnit_t start = get(f, 0); start != 0; start = (uint8_t) (start >> 8))
            startOffset--;
        return (uint8_t) (sections * sizeof(fieldUnit_t) - startOffset);
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

    fieldMapSize_t getFieldCount() const {
        return size;
    }

    void reset() {
        size = 0;
        if (RP::cacheFieldResults) {
            lastSearchedField = 0;
            lastFoundValue = 0;
        }
    }

    void copyFieldTo(RCodeOutStream<RP> *out, char c) const {
        bool hasSeenBefore = false;
        for (int i = 0; i < size; i++) {
            if (fields[i] == c) {
                if (!hasSeenBefore) {
                    out->writeField(c, values[i]);
                    hasSeenBefore = true;
                } else {
                    out->continueField(values[i]);
                }
            } else if (hasSeenBefore) {
                break;
            }
        }

    }

    void copyTo(RCodeOutStream<RP> *out) const {
        char last = 0;
        for (int i = 0; i < size; i++) {
            if (fields[i] != 'E' && fields[i] != 'R' && fields[i] != 'S') {
                if (last == fields[i]) {
                    out->continueField(values[i]);
                } else {
                    out->writeField(fields[i], values[i]);
                    last = fields[i];
                }
            }
        }
    }
};

#include "../RCodeOutStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEFIELDMAP_HPP_ */

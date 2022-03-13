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
protected:
    bigFieldAddress_t length = 0;
    bool string = false;
public:
    virtual bool addByteToBigField(uint8_t b) = 0;

    virtual void copyTo(ZcodeOutStream<ZP> *out) const = 0;

    virtual uint8_t const* getData() const = 0;

    bool isInUse() const {
        return length != 0;
    }

    bigFieldAddress_t getLength() const {
        return length;
    }

    bool isString() const {
        return string;
    }

    void reset() {
        length = 0;
    }

    void setIsString(bool isString) {
        this->string = isString;
    }

    virtual ~ZcodeBigField() {
    }
};

template<class ZP>
class ZcodeStandardBigField: public ZcodeBigField<ZP> {
private:
    uint8_t data[ZP::bigFieldLength];
public:
    virtual uint8_t const* getData() const {
        return data;
    }

    virtual bool addByteToBigField(uint8_t b) {
        if (this->length >= ZP::bigFieldLength) {
            return false;
        }
        data[this->length++] = b;
        return true;
    }

    virtual void copyTo(ZcodeOutStream<ZP> *out) const {
        if (this->length != 0) {
            if (ZcodeStandardBigField<ZP>::isString()) {
                out->writeBigStringField(data, this->length);
            } else {
                out->writeBigHexField(data, this->length);
            }
        }
    }
};

template<class ZP>
class ZcodeHugeField: public ZcodeBigField<ZP> {
private:
    uint8_t data[ZP::hugeFieldLength];
public:
    virtual bool addByteToBigField(uint8_t b) {
        if (this->length >= ZP::hugeFieldLength) {
            return false;
        }
        data[this->length++] = b;
        return true;
    }

    virtual uint8_t const* getData() const {
        return data;
    }

    virtual void copyTo(ZcodeOutStream<ZP> *out) const {
        if (this->length != 0) {
            if (this->isString()) {
                out->writeBigStringField(data, this->length);
            } else {
                out->writeBigHexField(data, this->length);
            }
        }
    }

    void copyFrom(ZcodeStandardBigField<ZP> const *source) {
        this->length = source->getLength();
        uint8_t const *sourceData = source->getData();
        for (int i = 0; i < this->length; i++) {
            data[i] = sourceData[i];
        }
        this->string = source->isString();
    }
};

#include "../ZcodeOutStream.hpp"

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEBIGFIELD_HPP_ */

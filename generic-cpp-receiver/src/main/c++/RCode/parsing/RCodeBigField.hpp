/*
 * RCodeBigField.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_
#include "../RCodeIncludes.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeBigField {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
protected:
    bigFieldAddress_t length = 0;
    bool string = false;
public:
    virtual bool addByteToBigField(uint8_t b) = 0;

    virtual void copyTo(RCodeOutStream<RP> *out) const = 0;

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

    virtual ~RCodeBigField() {
    }
};

template<class RP>
class RCodeStandardBigField: public RCodeBigField<RP> {
private:
    uint8_t data[RP::bigFieldLength];
public:
    virtual uint8_t const* getData() const {
        return data;
    }

    virtual bool addByteToBigField(uint8_t b) {
        if (this->length >= RP::bigFieldLength) {
            return false;
        }
        data[this->length++] = b;
        return true;
    }

    virtual void copyTo(RCodeOutStream<RP> *out) const {
        if (this->length != 0) {
            if (RCodeStandardBigField<RP>::isString()) {
                out->writeBigStringField(data, this->length);
            } else {
                out->writeBigHexField(data, this->length);
            }
        }
    }
};

template<class RP>
class RCodeBigBigField: public RCodeBigField<RP> {
private:
    uint8_t data[RP::bigBigFieldLength];
public:
    virtual bool addByteToBigField(uint8_t b) {
        if (this->length >= RP::bigBigFieldLength) {
            return false;
        }
        data[this->length++] = b;
        return true;
    }

    virtual uint8_t const* getData() const {
        return data;
    }

    virtual void copyTo(RCodeOutStream<RP> *out) const {
        if (this->length != 0) {
            if (this->isString()) {
                out->writeBigStringField(data, this->length);
            } else {
                out->writeBigHexField(data, this->length);
            }
        }
    }

    void copyFrom(RCodeStandardBigField<RP> const *source) {
        this->length = source->getLength();
        uint8_t const *sourceData = source->getData();
        for (int i = 0; i < this->length; i++) {
            data[i] = sourceData[i];
        }
        this->string = source->isString();
    }
};

#include "../RCodeOutStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_ */

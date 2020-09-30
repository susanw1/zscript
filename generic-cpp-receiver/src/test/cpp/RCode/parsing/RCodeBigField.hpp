/*
 * RCodeBigField.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"

class RCodeOutStream;

class RCodeBigField {
protected:
    bigFieldAddress_t length = 0;
    bool string = false;
public:
    virtual bool addByteToBigField(uint8_t b) = 0;

    virtual void copyTo(RCodeOutStream *out) const = 0;

    virtual uint8_t const* getData() const = 0;

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
class RCodeStandardBigField: public RCodeBigField {
private:
    uint8_t data[RCodeParameters::bigFieldLength];
public:
    virtual uint8_t const* getData() const {
        return data;
    }

    virtual bool addByteToBigField(uint8_t b) {
        if (length >= RCodeParameters::bigFieldLength) {
            return false;
        }
        data[length++] = b;
        return true;
    }

    virtual void copyTo(RCodeOutStream *out) const;
};

class RCodeBigBigField: public RCodeBigField {
private:
    uint8_t data[RCodeParameters::bigBigFieldLength];
public:
    virtual bool addByteToBigField(uint8_t b) {
        if (length >= RCodeParameters::bigBigFieldLength) {
            return false;
        }
        data[length++] = b;
        return true;
    }

    virtual uint8_t const* getData() const {
        return data;
    }

    virtual void copyTo(RCodeOutStream *out) const;

    void copyFrom(RCodeStandardBigField const *source) {
        length = source->getLength();
        uint8_t const *sourceData = source->getData();
        for (int i = 0; i < length; i++) {
            data[i] = sourceData[i];
        }
        string = source->isString();
    }

};

#include "../RCodeOutStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEBIGFIELD_HPP_ */

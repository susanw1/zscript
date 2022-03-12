/*
 * ZcodeMarkerInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODEMARKERINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODEMARKERINSTREAM_HPP_
#include "../ZcodeIncludes.hpp"

template<class RP>
class ZcodeSequenceInStream;

template<class RP>
class ZcodeMarkerInStream {
private:
    ZcodeSequenceInStream<RP> *sequenceIn;
    bool hasRead = false;

public:
    ZcodeMarkerInStream(ZcodeSequenceInStream<RP> *sequenceIn) :
            sequenceIn(sequenceIn) {
    }

    void reset() {
        hasRead = false;
    }

    char read() {
        if (hasRead) {
            sequenceIn->read();
        }
        hasRead = true;
        return sequenceIn->peek();
    }

    char reread() {
        return sequenceIn->peek();
    }

    bool hasNext() {
        return sequenceIn->hasNext();
    }

    void close() {
    }

    void unread() {
        hasRead = false;
    }

    void eatWhitespace() {
        char c = reread();
        while (c == ' ' || c == '\t' || c == '\r') {
            c = read();
        }
    }
};

#include "ZcodeSequenceInStream.hpp"
#endif /* SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODEMARKERINSTREAM_HPP_ */

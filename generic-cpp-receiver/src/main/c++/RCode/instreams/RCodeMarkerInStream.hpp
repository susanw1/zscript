/*
 * RCodeMarkerInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODEMARKERINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODEMARKERINSTREAM_HPP_
#include "../RCodeIncludes.hpp"

template<class RP>
class RCodeSequenceInStream;

template<class RP>
class RCodeMarkerInStream {
private:
    RCodeSequenceInStream<RP> *sequenceIn;
    bool hasRead = false;

public:
    RCodeMarkerInStream(RCodeSequenceInStream<RP> *sequenceIn) :
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

#include "RCodeSequenceInStream.hpp"
#endif /* SRC_MAIN_CPP_RCODE_INSTREAMS_RCODEMARKERINSTREAM_HPP_ */

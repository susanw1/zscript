/*
 * RCodeMarkerInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODEMARKERINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODEMARKERINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

class RCodeSequenceInStream;

class RCodeMarkerInStream {
private:
    RCodeSequenceInStream *sequenceIn;
    bool hasRead = false;

public:
    RCodeMarkerInStream(RCodeSequenceInStream *sequenceIn) :
            sequenceIn(sequenceIn) {
    }

    void reset() {
        hasRead = false;
    }

    char read();

    char reread();

    bool hasNext();

    void close();

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

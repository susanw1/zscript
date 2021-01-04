/*
 * RCodeChannelInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECHANNELINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECHANNELINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeLookaheadStream.hpp"
#include "RCodeSequenceInStream.hpp"

template<class RP>
class RCodeChannelInStream {
private:
    RCodeSequenceInStream<RP> seqInStream;

public:
    RCodeChannelInStream() :
            seqInStream(this) {
    }

    RCodeSequenceInStream<RP>* getSequenceInStream() {
        return &seqInStream;
    }

    virtual int16_t read() = 0;

    virtual RCodeLookaheadStream<RP>* getLookahead() = 0;

    virtual ~RCodeChannelInStream() {
    }
};



#endif /* SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECHANNELINSTREAM_HPP_ */

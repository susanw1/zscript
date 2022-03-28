/*
 * ZcodeChannelInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeLookaheadStream.hpp"
#include "ZcodeSequenceInStream.hpp"

template<class ZP>
class ZcodeChannelInStream {
    private:
        ZcodeSequenceInStream<ZP> seqInStream;

    public:
        ZcodeChannelInStream() :
                seqInStream(this) {
        }

        ZcodeSequenceInStream<ZP>* getSequenceInStream() {
            return &seqInStream;
        }

        virtual int16_t read() = 0;

        virtual ZcodeLookaheadStream<ZP>* getLookahead() = 0;

        virtual ~ZcodeChannelInStream() {
        }
};

#endif /* SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_ */

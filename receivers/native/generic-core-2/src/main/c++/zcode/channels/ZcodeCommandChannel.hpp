/*
 * ZcodeCommandChannel.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeLockSet;

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSequence;

enum ZcodeCommandChannelStateChange {
    SET_AS_NOTIFICATION, RELEASED_FROM_NOTIFICATION, SET_AS_DEBUG, RELEASED_FROM_DEBUG
};

template<class ZP>
class ZcodeCommandChannel {
public:
    ZcodeRunningCommandSlot<ZP> runner;
    ZcodeChannelInStream<ZP> *in;
    ZcodeOutStream<ZP> *out;
    bool packetBased;

    ZcodeCommandChannel(Zcode<ZP> *zcode, ZcodeChannelInStream<ZP> *in, ZcodeOutStream<ZP> *out, bool packetBased) :
            runner(zcode, out, in->getSlot()), in(in), out(out), packetBased(packetBased) {
    }

    virtual void stateChange(ZcodeCommandChannelStateChange change) {
    }

};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_ */

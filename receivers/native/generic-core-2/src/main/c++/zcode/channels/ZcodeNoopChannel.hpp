/*
 * ZcodeNoopChannel.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_

#include "ZcodeCommandChannel.hpp"
#include "ZcodeChannelInStream.hpp"
#include "../ZcodeOutStream.hpp"

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeNoopChannel: public ZcodeCommandChannel<ZP> {
    template<class ZP1>
    class ZcodeNoopOutStream: public ZcodeOutStream<ZP1> {
    public:

        void writeByte(uint8_t value) {
        }
        void open(ZcodeCommandChannel<ZP1> *target, ZcodeOutStreamOpenType type) {
        }

        bool isOpen() {
            return true;
        }

        void close() {
        }
    };
    template<class ZP1>
    class ZcodeNoopInStream: public ZcodeChannelInStream<ZP1> {
    public:

        ZcodeNoopInStream(Zcode<ZP1> *zcode, ZcodeCommandChannel<ZP1> *channel) :
                ZcodeChannelInStream<ZP1>(zcode, channel, NULL, 0) {
        }

        bool pushData() {
            return false;
        }
    };
    ZcodeNoopInStream<ZP> inA;
    ZcodeNoopOutStream<ZP> outA;

public:
    ZcodeNoopChannel<ZP>(Zcode<ZP> *zcode) :
            ZcodeCommandChannel<ZP>(zcode, &inA, &outA, false), inA(zcode, this), outA() {
    }
};
#endif /* SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_ */

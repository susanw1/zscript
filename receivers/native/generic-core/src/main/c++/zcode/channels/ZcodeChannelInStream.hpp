/*
 * ZcodeChannelInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_

#include "../parsing/ZcodeChannelCommandSlot.hpp"
#include "../parsing/ZcodeBigField.hpp"
#include "../ZcodeIncludes.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeChannelInStream {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;

protected:
    ZcodeChannelCommandSlot<ZP> slot;

public:
    ZcodeChannelInStream(ZcodeCommandChannel<ZP> *channel, uint8_t *big, bigFieldAddress_t length) :
            slot(channel, big, length) {
    }

    virtual bool pushData() = 0;

    ZcodeChannelCommandSlot<ZP>* getSlot() {
        return &slot;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECHANNELINSTREAM_HPP_ */

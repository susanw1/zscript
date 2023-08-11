/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECHANNELINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECHANNELINSTREAM_HPP_

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

    virtual ~ZcodeChannelInStream() {
    }

    virtual bool pushData() = 0;

    ZcodeChannelCommandSlot<ZP>* getSlot() {
        return &slot;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECHANNELINSTREAM_HPP_ */

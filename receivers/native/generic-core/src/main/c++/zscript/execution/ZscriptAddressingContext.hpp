/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_

#include "../ZscriptIncludes.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"
#include "../tokenizer/ZscriptTokenizer.hpp"
#include "CommandOutStream.hpp"

namespace Zscript {
template<class ZP>
class Zscript;
template<class ZP>
class AbstractOutStream;

namespace GenericCore {
template<class ZP>
class SemanticParser;
}

template<class ZP>
class AddressOptIterator {
    CombinedTokenIterator<ZP> iterator;

public:
    AddressOptIterator(TokenRingReader<ZP> reader) :
            iterator(reader.iterator()) {

    }

    OptInt16 next() {
        GenericCore::OptionalRingBufferToken<ZP> rbt = iterator.next();
        if (!rbt.isPresent) {
            return {0, false};
        }
        uint8_t key = rbt.token.getKey(iterator.getBuffer());
        if (key != Zchars::Z_ADDRESSING && key != Zchars::Z_ADDRESSING_CONTINUE) {
            return {0, false};
        }
        return {rbt.token.getData16(iterator.getBuffer()), true};
    }
};

template<class ZP>
class ZscriptAddressingContext {
    GenericCore::SemanticParser<ZP> *parseState;

public:
    ZscriptAddressingContext(GenericCore::SemanticParser<ZP> *parseState) :
            parseState(parseState) {
        commandComplete();
    }
    ZscriptAddressingContext() :
            parseState(NULL) {
    }

    CombinedTokenBlockIterator<ZP> getAddressedData() {
        TokenRingReader<ZP> reader = parseState->getReader();
        GenericCore::RingBufferTokenIterator<ZP> iterator = reader.rawIterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(reader.asBuffer()); opt.isPresent; opt = iterator.next(reader.asBuffer())) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(reader.asBuffer()) == ZscriptTokenizer<ZP>::ADDRESSING_FIELD_KEY) {
                return token.blockIterator(reader.asBuffer());
            }
        }
    }

    AddressOptIterator<ZP> getAddressSegments() {
        return AddressOptIterator<ZP>(parseState->getReader());
    }

    uint16_t getAddressedDataSize() {
        TokenRingReader<ZP> reader = parseState->getReader();
        GenericCore::RingBufferTokenIterator<ZP> iterator = reader.rawIterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(reader.asBuffer()); opt.isPresent; opt = iterator.next(reader.asBuffer())) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(reader.asBuffer()) == ZscriptTokenizer<ZP>::ADDRESSING_FIELD_KEY) {
                return token.getDataSize(reader.asBuffer());
            }
        }
        return 0;
    }

    bool verify() {
        bool hasReachedData = false;
        int i = 0;

        TokenRingReader<ZP> reader = parseState->getReader();
        GenericCore::RingBufferTokenIterator<ZP> iterator = reader.rawIterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(reader.asBuffer()); opt.isPresent; opt = iterator.next(reader.asBuffer())) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            i++;
            if (hasReachedData) {
                if (token.isMarker(reader.asBuffer())) {
                    return true;
                }
                status(ResponseStatus::INVALID_KEY);
                return false;
            }
            if (token.getKey(reader.asBuffer()) == Zchars::Z_ADDRESSING || token.getKey(reader.asBuffer()) == Zchars::Z_ADDRESSING_CONTINUE) {
                continue;
            }
            if (token.getKey(reader.asBuffer()) != ZscriptTokenizer<ZP>::ADDRESSING_FIELD_KEY) {
                status(ResponseStatus::INVALID_KEY);
                return false;
            }
            if (i == 1) {
                status(ResponseStatus::INTERNAL_ERROR);
                return false;
            }
            hasReachedData = true;
        }
        status(ResponseStatus::MISSING_KEY);
        return false;
    }

    void status(uint8_t status) {
        AbstractOutStream<ZP> *out = Zscript<ZP>::zscript.getNotificationOutStream();
        if (!out->isOpen()) {
            out->open(Zscript<ZP>::zscript.getNotificationChannelIndex());
        }
        out->writeField('!', 0);
        out->writeField(Zchars::Z_STATUS, status);
        out->endSequence();
        out->close();
        parseState->setStatus(status);
    }

    void commandComplete() {
        parseState->setCommandComplete(true);
    }

    void commandNotComplete() {
        parseState->setCommandComplete(false);
    }

    bool isCommandComplete() {
        return parseState->isCommandComplete();
    }

    bool isActivated() {
        return parseState->isActivated();
    }

    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return parseState->getAsyncActionNotifier();
    }
};
}
#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_ */

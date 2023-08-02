/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_

#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"
#include "../Zchars.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"

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
class ZscriptAddressingContext {
    GenericCore::SemanticParser<ZP> *parseState;

public:
    ZscriptAddressingContext(GenericCore::SemanticParser<ZP> *parseState) :
            parseState(parseState) {
    }
    ZscriptAddressingContext() :
            parseState(NULL) {
    }

    CombinedTokenBlockIterator<ZP> getAddressedData() {
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        GenericCore::RingBufferTokenIterator<ZP> iterator = buffer->R_iterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(buffer) == ZscriptTokenizer < ZP > ::ADDRESSING_FIELD_KEY) {
                return CombinedTokenBlockIterator<ZP>(buffer, token.blockIterator(buffer));
            }
        }
    }

    class AddressOptIterator {
        GenericCore::TokenRingBuffer<ZP> *buffer;
        GenericCore::RingBufferTokenIterator<ZP> iterator;

    public:
        AddressOptIterator(GenericCore::TokenRingBuffer<ZP> *buffer) :
                buffer(buffer), iterator(buffer->R_iterator()) {

        }

        OptInt16 next() {
            GenericCore::OptionalRingBufferToken<ZP> rbt = iterator.next(buffer);
            if (!rbt.isPresent) {
                return {0, false};
            }
            uint8_t key = rbt.token.getKey(buffer);
            if (key == Zchars::Z_ADDRESSING || key == Zchars::Z_ADDRESSING_CONTINUE) {
                return {0, false};
            }
            return {rbt.token.getData16(buffer), true};
        }
    };
    AddressOptIterator getAddressSegments() {
        return AddressOptIterator(parseState->getBuffer());
    }

    uint16_t getAddressedDataSize() {
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        GenericCore::RingBufferTokenIterator<ZP> iterator = buffer->R_iterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(buffer) == ZscriptTokenizer < ZP > ::ADDRESSING_FIELD_KEY) {
                return token.getDataSize(buffer);
            }
        }
        return 0;
    }

    bool verify() {
        bool hasReachedData = false;
        int i = 0;

        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        GenericCore::RingBufferTokenIterator<ZP> iterator = buffer->R_iterator();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            i++;
            if (hasReachedData) {
                if (token.isMarker(buffer)) {
                    return true;
                }
                status(ResponseStatus::INVALID_KEY);
                return false;
            }
            if (token.getKey(buffer) == Zchars::Z_ADDRESSING || token.getKey(buffer) == Zchars::Z_ADDRESSING_CONTINUE) {
                continue;
            }
            if (token.getKey(buffer) != ZscriptTokenizer < ZP > ::ADDRESSING_FIELD_KEY) {
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
};
}
#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTADDRESSINGCONTEXT_HPP_ */

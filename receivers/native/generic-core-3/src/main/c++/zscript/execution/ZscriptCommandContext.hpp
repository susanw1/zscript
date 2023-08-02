/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_

#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"

namespace Zscript {

namespace GenericCore {
template<class ZP>
class SemanticParser;
}
template<class ZP>
class Zscript;
template<class ZP>
class AbstractOutStream;

template<class ZP>
class CommandTokenIterator {
    GenericCore::RingBufferTokenIterator<ZP> internal;

public:

    CommandTokenIterator(GenericCore::RingBufferTokenIterator<ZP> internal) :
            internal(internal) {
    }

    GenericCore::OptionalRingBufferToken<ZP> next(GenericCore::TokenRingBuffer<ZP> *buffer) {
        GenericCore::OptionalRingBufferToken<ZP> val = internal.next(buffer);
        if (val.isPresent && val.token.isMarker(buffer)) {
            return GenericCore::OptionalRingBufferToken<ZP>();
        } else {
            return val;
        }
    }
};

template<class ZP>
class BigFieldBlockIterator {
    GenericCore::TokenRingBuffer<ZP> *buffer;
    CommandTokenIterator<ZP> iterator;
    GenericCore::RawTokenBlockIterator<ZP> internal;
    bool hasInternal = false;

public:
    BigFieldBlockIterator(GenericCore::TokenRingBuffer<ZP> *buffer, CommandTokenIterator<ZP> iterator) :
            buffer(buffer), iterator(iterator), internal() {
    }

    bool hasNext() {
        if (hasInternal && internal.hasNext()) {
            return true;
        }
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (ZcharsUtils<ZP>::isBigField(token.getKey(buffer))) {
                internal = token.blockIterator(buffer);
                if (internal.hasNext(buffer)) {
                    return true;
                }
            }
        }
        return false;
    }

    uint8_t next() {
        hasNext();
        return internal.next();
    }

    DataArrayWLeng16 nextContiguous() {
        hasNext();
        return internal.nextContiguous();
    }

    DataArrayWLeng16 nextContiguous(uint8_t maxLength) {
        hasNext();
        return internal.nextContiguous(maxLength);
    }

};

template<class ZP>
class ZscriptCommandContext {
    Zscript<ZP> *zscript;
    GenericCore::SemanticParser<ZP> *parseState;
    AbstractOutStream<ZP> *out;

    CommandTokenIterator<ZP> iteratorToMarker() {
        return CommandTokenIterator<ZP>(parseState->getBuffer()->R_iterator());
    }

public:
    ZscriptCommandContext(Zscript<ZP> *zscript, GenericCore::SemanticParser<ZP> *parseState, AbstractOutStream<ZP> *out) :
            zscript(zscript), parseState(parseState), out(out) {
    }
    ZscriptCommandContext() :
            zscript(NULL), parseState(NULL), out(NULL) {
    }

    OptInt16 getField(uint8_t key) {
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(buffer) == key) {
                return {token.getData16(buffer), true};
            }
        }
        return {0, false};
    }

    bool hasField(uint8_t key) {
        return getField(key).isPresent;
    }

    uint16_t getField(uint8_t key, uint16_t def) {
        OptInt16 opt = getField(key);
        if (opt.isPresent) {
            return opt.value;
        } else {
            return def;
        }
    }

    int getFieldCount() {
        int count = 0;

        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            if (ZcharsUtils<ZP>::isNumericKey(opt.get().getKey())) {
                count++;
            }
        }
        return count;
    }

    BigFieldBlockIterator<ZP> getBigField() {
        return BigFieldBlockIterator<ZP>(parseState->getBuffer(), iteratorToMarker());
    }

    int getBigFieldSize() {
        int size = 0;

        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (ZcharsUtils<ZP>::isBigField(token.getKey(buffer))) {
                size += token.getDataSize(buffer);
            }
        }
        return size;
    }

    void status(uint8_t status) {
        parseState->setStatus(status);
        out->writeField(Zchars::Z_STATUS, status);
    }

    bool isEmpty() {
        return parseState->getBuffer()->R_getFirstReadToken().isMarker(parseState->getBuffer());
    }
    AbstractOutStream<ZP>* getOutStream() {
        return out;
    }

    bool verify() {
        uint32_t foundCommands = 0;
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> rt = opt.token;
            uint8_t key = rt.getKey(buffer);
            if (ZcharsUtils<ZP>::isNumericKey(key)) {
                if ((foundCommands & (1 << (key - 'A'))) != 0) {
                    commandComplete();
                    status(ResponseStatus::REPEATED_KEY);
                    return false;
                }
                foundCommands |= (1 << (key - 'A'));
                if (rt.getDataSize(buffer) > 2) {
                    commandComplete();
                    status(ResponseStatus::FIELD_TOO_LONG);
                    return false;
                }
            } else if (!ZcharsUtils<ZP>::isBigField(key)) {
                commandComplete();
                status(ResponseStatus::INVALID_KEY);
                return false;
            }
        }
        return true;
    }

    void activate() {
        parseState->activate();
    }

    void silentSucceed() {
        parseState->silentSucceed();
    }

    Zscript<ZP>* getZscript() {
        return zscript;
    }

    int getChannelIndex() {
        return parseState->getChannelIndex();
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
#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_ */

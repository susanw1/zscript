/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_

#include "../ZscriptIncludes.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"
#include "CommandOutStream.hpp"

namespace Zscript {

namespace GenericCore {
template<class ZP>
class SemanticParser;
template<class ZP>
class ZscriptNotificationSource;
}

template<class ZP>
class AsyncActionNotifier {
    void *source;
    #ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    bool isSemantic;
#endif

public:
    AsyncActionNotifier(GenericCore::SemanticParser<ZP> *parser) :
            source(parser)

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    ,isSemantic(true)
#endif

    {
    }

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    AsyncActionNotifier(GenericCore::ZscriptNotificationSource<ZP> *notification) :
            source(notification), isSemantic(false) {
    }
#endif
    AsyncActionNotifier() :
            source(NULL)

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    ,isSemantic(true)
#endif
    {
    }

    void notifyNeedsAction() {
        if (source == NULL) {
            return;
        }
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        if (isSemantic) {
#endif
        ((GenericCore::SemanticParser<ZP>*) source)->notifyNeedsAction();
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        } else {
            ((GenericCore::ZscriptNotificationSource<ZP>*) source)->notifyNeedsAction();
        }
#endif
    }
};
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
    TokenRingReader<ZP> reader;
    CommandTokenIterator<ZP> iterator;
    GenericCore::RawTokenBlockIterator<ZP> internal;
    bool hasInternal = false;

public:
    BigFieldBlockIterator(TokenRingReader<ZP> reader, CommandTokenIterator<ZP> iterator) :
            reader(reader), iterator(iterator), internal() {
    }

    bool hasNext() {
        if (hasInternal && internal.hasNext(reader.asBuffer())) {
            return true;
        }
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(reader.asBuffer()); opt.isPresent; opt = iterator.next(reader.asBuffer())) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (ZcharsUtils<ZP>::isBigField(token.getKey(reader.asBuffer()))) {
                internal = token.rawBlockIterator(reader.asBuffer());
                if (internal.hasNext(reader.asBuffer())) {
                    return true;
                }
            }
        }
        return false;
    }

    uint8_t next() {
        hasNext();
        return internal.next(reader.asBuffer());
    }

    DataArrayWLeng16 nextContiguous() {
        hasNext();
        return internal.nextContiguous(reader.asBuffer());
    }

    DataArrayWLeng16 nextContiguous(uint8_t maxLength) {
        hasNext();
        return internal.nextContiguous(reader.asBuffer(), maxLength);
    }

};
namespace GenericCore {
template<class ZP>
class EchoCommand;
}
template<class ZP>
class ZscriptCommandContext {
    friend class GenericCore::EchoCommand<ZP>;

    GenericCore::SemanticParser<ZP> *parseState;
    AbstractOutStream<ZP> *out;

    CommandTokenIterator<ZP> iteratorToMarker() {
        return CommandTokenIterator<ZP>(parseState->getReader().rawIterator());
    }

public:
    ZscriptCommandContext(GenericCore::SemanticParser<ZP> *parseState, AbstractOutStream<ZP> *out) :
            parseState(parseState), out(out) {
        commandComplete();
    }
    ZscriptCommandContext() :
            parseState(NULL), out(NULL) {
    }

    OptInt16 getField(uint8_t key) {
        OptInt16 ret;
        ret.isPresent = getField(key, &ret.value);
        return ret;
    }
    bool getField(uint8_t key, uint16_t *dest) {
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getReader().asBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (token.getKey(buffer) == key) {
                *dest = token.getData16(buffer);
                return true;
            }
        }
        return false;
    }

    bool hasField(uint8_t key) {
        return getField(key).isPresent;
    }

    uint16_t getField(uint8_t key, uint16_t def) {
        uint16_t val = def;
        getField(key, &val);
        return val;
    }

    uint8_t getFieldCount() {
        int count = 0;

        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getReader().asBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            if (ZcharsUtils<ZP>::isNumericKey(opt.token.getKey())) {
                count++;
            }
        }
        return count;
    }

    BigFieldBlockIterator<ZP> getBigField() {
        return BigFieldBlockIterator<ZP>(parseState->getReader(), iteratorToMarker());
    }

    uint16_t getBigFieldSize() {
        int size = 0;

        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getReader().asBuffer();
        CommandTokenIterator<ZP> iterator = iteratorToMarker();
        for (GenericCore::OptionalRingBufferToken<ZP> opt = iterator.next(buffer); opt.isPresent; opt = iterator.next(buffer)) {
            GenericCore::RingBufferToken<ZP> token = opt.token;
            if (ZcharsUtils<ZP>::isBigField(token.getKey(buffer))) {
                size += token.getDataSize(buffer);
            }
        }
        return size;
    }

    void status(uint16_t status) {
        if (status > 0xff) {
            parseState->setStatus(0xff);
        } else {
            parseState->setStatus((uint8_t) status);
        }
        out->writeField(Zchars::Z_STATUS, status);
    }

    bool isEmpty() {
        return parseState->getReader().getFirstReadToken().isMarker(parseState->getBuffer());
    }

    bool verify() {
        uint32_t foundCommands = 0;
        GenericCore::TokenRingBuffer<ZP> *buffer = parseState->getReader().asBuffer();
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
                if (rt.hasSizeGreaterThan(buffer, 2)) {
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

    CommandOutStream<ZP> getOutStream() {
        return CommandOutStream<ZP>(out);
    }

    void activate() {
        parseState->activate();
    }

    void silentSucceed() {
        parseState->silentSucceed();
    }

    uint8_t getChannelIndex() {
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

    AsyncActionNotifier<ZP> getAsyncActionNotifier() {
        return parseState->getAsyncActionNotifier();
    }

};
}
#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_ZSCRIPTCOMMANDCONTEXT_HPP_ */

/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_COMMANDOUTSTREAM_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_COMMANDOUTSTREAM_HPP_

#include "../ZscriptIncludes.hpp"

namespace Zscript {
template<class ZP>
class AbstractOutStream;

template<class ZP>
class QuotedStringFieldWriter {
    AbstractOutStream<ZP> *internal;

public:
    QuotedStringFieldWriter(AbstractOutStream<ZP> *internal) :
            internal(internal) {
    }

    void continueString(DataArrayWLeng8 data) {
        internal->continueBigString(data);
    }

    void continueString(DataArrayWLeng16 data) {
        internal->continueBigString(data);
    }

    void continueString(const uint8_t *data, uint16_t length) {
        internal->continueBigString(data, length);
    }

    void continueString(const char *text) {
        internal->continueBigString(text);
    }

    void end() {
        internal->endBigString();
    }

    QuotedStringFieldWriter *operator&() = delete;
};

template<class ZP>
class BigHexFieldWriter {
    AbstractOutStream<ZP> *internal;

public:
    BigHexFieldWriter(AbstractOutStream<ZP> *internal) :
            internal(internal) {
    }

    void continueBigHex(DataArrayWLeng8 data) {
        internal->continueBigHex(data);
    }

    void continueBigHex(DataArrayWLeng16 data) {
        internal->continueBigHex(data);
    }

    void continueBigHex(uint8_t *data, uint16_t length) {
        internal->continueBigHex(data, length);
    }

    void continueBigHex(const char *text) {
        internal->continueBigHex(text);
    }

    BigHexFieldWriter *operator&() = delete;
};

template<class ZP>
class CommandOutStream {
protected:
    AbstractOutStream<ZP> *internal;

public:
    CommandOutStream(AbstractOutStream<ZP> *internal) :
            internal(internal) {
    }

    void writeBytes(uint8_t *bytes, uint16_t count) {
        internal->writeBytes(bytes, count);
    }

    void writeField(uint8_t field, uint16_t value) {
        internal->writeField(field, value);
    }

    QuotedStringFieldWriter<ZP> beginQuotedString() {
        internal->beginBigString();
        return QuotedStringFieldWriter<ZP>(internal);
    }

    void writeQuotedString(const char *text) {
        internal->writeQuotedString(text);
    }

    void writeQuotedString(DataArrayWLeng8 data) {
        internal->writeQuotedString(data);
    }

    void writeQuotedString(DataArrayWLeng16 data) {
        internal->writeQuotedString(data);
    }

    void writeQuotedString(const uint8_t *data, uint16_t length) {
        internal->writeQuotedString(data, length);
    }

    BigHexFieldWriter<ZP> beginBigHex() {
        internal->beginBigHex();
        return BigHexFieldWriter<ZP>(internal);
    }

    void writeBigHex(DataArrayWLeng8 data) {
        internal->writeBigHex(data);
    }

    void writeBigHex(DataArrayWLeng16 data) {
        internal->writeBigHex(data);
    }

    void writeBigHex(uint8_t *data, uint16_t length) {
        internal->writeBigHex(data, length);
    }

    void writeBigHex(const char *text) {
        internal->writeBigHex(text);
    }

    CommandOutStream *operator&() = delete;
};

template<class ZP>
class NotificationOutStream : public CommandOutStream<ZP> {
public:
    NotificationOutStream(AbstractOutStream<ZP> *internal) :
            CommandOutStream<ZP>(internal) {
    }

    void endSequence() {
        this->internal->endSequence();
    }

    void writeOrElse() {
        this->internal->writeOrElse();
    }

    void writeAndThen() {
        this->internal->writeAndThen();
    }

    void writeOpenParen() {
        this->internal->writeOpenParen();
    }

    void writeCloseParen() {
        this->internal->writeCloseParen();
    }

    NotificationOutStream *operator&() = delete;
};

}
#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_COMMANDOUTSTREAM_HPP_ */

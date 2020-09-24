/*
 * RCodeDebugOutput.cpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#include "RCodeDebugOutput.hpp"

void RCodeDebugOutput::flushBuffer(RCodeOutStream *stream) {
    int curPos = 0;
    int prevPos = 0;
    while (curPos < position) {
        prevPos = curPos;
        while (curPos < RCodeParameters::debugBufferLength
                && debugBuffer[curPos] != '\n') {
            curPos++;
        }
        stream->markDebug()->writeBytes(debugBuffer + prevPos, curPos - prevPos)->writeCommandSequenceSeperator();
        curPos++;
    }
    if (position == RCodeParameters::debugBufferLength + 1) {
        const char *overRunMessage =
                "#Debug buffer out of space, some data lost\n";
        int l = 0;
        while (overRunMessage[l] != 0) {
            l++;
        }
        stream->writeBytes((const uint8_t*) overRunMessage, l);
    }
    position = 0;
}

void RCodeDebugOutput::writeToBuffer(const uint8_t *b, int length) {
    if (position + length >= RCodeParameters::debugBufferLength) {
        int lenToCopy = RCodeParameters::debugBufferLength - position;
        if (lenToCopy > 0) {
            for (int i = 0; i < lenToCopy; ++i) {
                debugBuffer[position + i] = b[i];
            }
        }
        position = RCodeParameters::debugBufferLength + 1;
    } else {
        for (int i = 0; i < length; ++i) {
            debugBuffer[position + i] = b[i];
        }
        position += length;
    }
}
void RCodeDebugOutput::setDebugChannel(RCodeCommandChannel *channel) {
    if (this->channel != NULL) {
        this->channel->releaseFromDebugChannel();
    }
    this->channel = channel;
    this->channel->setAsDebugChannel();
}
void RCodeDebugOutput::println(const char *s) {
    int l = 0;
    while (s[l] != 0) {
        l++;
    }
    println(s, l);
}

void RCodeDebugOutput::println(const char *s, int length) {
    if (channel != NULL) {
        RCodeOutStream *stream = channel->getOutStream();
        if (stream->lock()) {
            if (stream->mostRecent == this) {
                if (!stream->isOpen()) {
                    stream->openDebug(channel);
                }
            } else {
                stream->mostRecent = this;
                if (stream->isOpen()) {
                    stream->close();
                }
                stream->openDebug(channel);
            }
            if (position != 0) {
                flushBuffer(stream);
            }
            char c = '#';
            stream->writeBytes((uint8_t*) &c, 1);
            stream->writeBytes((const uint8_t*) s, length);
            c = '\n';
            stream->writeBytes((uint8_t*) &c, 1);
            stream->unlock();
        } else {
            writeToBuffer((const uint8_t*) s, length);
            char c = '\n';
            writeToBuffer((uint8_t*) &c, 1);
        }
    }
}

void RCodeDebugOutput::attemptFlush() {
    if (position != 0 && channel != NULL && channel->getOutStream()->lock()) {
        RCodeOutStream *stream = channel->getOutStream();
        if (stream->mostRecent == this) {
            if (!stream->isOpen()) {
                stream->openDebug(channel);
            }
        } else {
            stream->mostRecent = this;
            if (stream->isOpen()) {
                stream->close();
            }
            stream->openDebug(channel);
        }
        flushBuffer(channel->getOutStream());
        stream->unlock();
    }
}

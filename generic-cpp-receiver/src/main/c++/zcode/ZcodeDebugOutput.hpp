/*
 * ZcodeDebugOutput.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_
#include "ZcodeIncludes.hpp"
#include "AbstractZcodeOutStream.hpp"

enum ZcodeDebugOutputMode {
    hex, decimal, character, normal, endl
};
struct ZcodeDebugOutputState {
    bool isHex :1;
    bool isDecimal :1;
    bool isCharacter :1;
};

template<class RP>
class ZcodeDebugOutput;

template<class RP>
class ZcodeDebugOutStream: public AbstractZcodeOutStream<RP> {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename RP::debugOutputBufferLength_t debugOutputBufferLength_t;

    ZcodeDebugOutput<RP> *debug;
    public:
    ZcodeDebugOutStream(ZcodeDebugOutput<RP> *debug) :
            debug(debug) {
    }

    virtual void writeByte(uint8_t value);

    virtual ZcodeOutStream<RP>* writeBytes(uint8_t const *value, bigFieldAddress_t length);

    void openResponse(ZcodeCommandChannel<RP> *target) {
    }

    void openNotification(ZcodeCommandChannel<RP> *target) {
    }

    void openDebug(ZcodeCommandChannel<RP> *target) {
    }

    bool isOpen() {
        return true;
    }

    void close() {
    }
};

template<class RP>
class ZcodeCommandChannel;

template<class RP>
class ZcodeOutStream;

template<class RP>
class ZcodeCommandSlot;

template<class RP>
class ZcodeDebugOutput {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename RP::debugOutputBufferLength_t debugOutputBufferLength_t;
    private:
    uint8_t debugBuffer[RP::debugBufferLength];
    ZcodeCommandChannel<RP> *channel = NULL;
    debugOutputBufferLength_t position;
    ZcodeDebugOutputState state;

    friend ZcodeDebugOutStream<RP> ;

    void flushBuffer(ZcodeOutStream<RP> *stream);

    void writeToBuffer(const uint8_t *b, debugOutputBufferLength_t length);

    char toHex(int i) {
        return (char) (i >= 10 ? i + 'a' - 10 : i + '0');
    }

public:
    void setDebugChannel(ZcodeCommandChannel<RP> *channel);

    void println(const char *s);

    ZcodeDebugOutput<RP>& operator <<(ZcodeDebugOutputMode m);

    ZcodeDebugOutput<RP>& operator <<(const char *s);

    ZcodeDebugOutput<RP>& operator <<(bool b);

    ZcodeDebugOutput<RP>& operator <<(char c);

    ZcodeDebugOutput<RP>& operator <<(int8_t i);

    ZcodeDebugOutput<RP>& operator <<(uint8_t i);

    ZcodeDebugOutput<RP>& operator <<(int16_t i);

    ZcodeDebugOutput<RP>& operator <<(uint16_t i);

    ZcodeDebugOutput<RP>& operator <<(int32_t i);

    ZcodeDebugOutput<RP>& operator <<(uint32_t i);

    ZcodeDebugOutput<RP>& operator <<(int64_t i);

    ZcodeDebugOutput<RP>& operator <<(uint64_t i);

    ZcodeDebugOutput<RP>& operator <<(ZcodeCommandSlot<RP> *s);

    void println(const char *s, debugOutputBufferLength_t length);

    void attemptFlush();

    ZcodeDebugOutStream<RP> getAsOutStream() {
        return ZcodeDebugOutStream<RP>(this);
    }
};

template<class RP>
void ZcodeDebugOutStream<RP>::writeByte(uint8_t value) {
    debug->writeToBuffer(&value, 1);
}

template<class RP>
ZcodeOutStream<RP>* ZcodeDebugOutStream<RP>::writeBytes(uint8_t const *value, bigFieldAddress_t length) {
    debug->writeToBuffer(value, length);
    return this;
}

template<class RP>
void ZcodeDebugOutput<RP>::flushBuffer(ZcodeOutStream<RP> *stream) {
    state.isCharacter = false;
    state.isDecimal = false;
    state.isHex = false;
    int curPos = 0;
    int prevPos = 0;
    while (curPos < position) {
        prevPos = curPos;
        while (curPos < RP::debugBufferLength && debugBuffer[curPos] != '\n') {
            curPos++;
        }
        stream->markDebug()->writeBytes(debugBuffer + prevPos, (bigFieldAddress_t) (curPos - prevPos))->writeCommandSequenceSeperator();
        curPos++;
    }
    if (position == RP::debugBufferLength + 1) {
        const char *overRunMessage = "#Debug buffer out of space, some data lost\n";
        bigFieldAddress_t l = 0;
        while (overRunMessage[l] != 0) {
            l++;
        }
        stream->writeBytes((const uint8_t*) overRunMessage, l);
    }
    position = 0;
}

template<class RP>
void ZcodeDebugOutput<RP>::writeToBuffer(const uint8_t *b, debugOutputBufferLength_t length) {
    if (position >= RP::debugBufferLength) {
        return;
    }
    debugOutputBufferLength_t lenToCopy = length;
    if (position + length >= RP::debugBufferLength) {
        lenToCopy = RP::debugBufferLength - position;
    }
    if (position < RP::debugBufferLength) {
        for (debugOutputBufferLength_t i = 0; i < lenToCopy; ++i) {
            debugBuffer[position + i] = b[i];
        }
    }
    position = (debugOutputBufferLength_t) (position + lenToCopy);
}

template<class RP>
void ZcodeDebugOutput<RP>::setDebugChannel(ZcodeCommandChannel<RP> *channel) {
    if (this->channel != NULL) {
        this->channel->releaseFromDebugChannel();
    }
    this->channel = channel;
    this->channel->setAsDebugChannel();
}

template<class RP>
void ZcodeDebugOutput<RP>::println(const char *s) {
    debugOutputBufferLength_t l = 0;
    while (s[l] != '\0') {
        l++;
    }
    println(s, l);
}

template<class RP>
void ZcodeDebugOutput<RP>::println(const char *s, debugOutputBufferLength_t length) {
    if (channel != NULL && (!channel->hasOutStream() || !channel->acquireOutStream()->lock())) {
        ZcodeOutStream<RP> *stream = channel->acquireOutStream();
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
        int prevPos = 0;
        for (int i = 0; i < length; ++i) {
            if (s[i] == '\n') {
                stream->markDebug()
                        ->writeBytes((const uint8_t*) (s + prevPos), (bigFieldAddress_t) (i - prevPos))
                        ->writeCommandSequenceSeperator();
                prevPos = i + 1;
            }
        }
        if (prevPos != length) {
            stream->markDebug()->writeBytes((const uint8_t*) (s + prevPos), (bigFieldAddress_t) (length - prevPos))->writeCommandSequenceSeperator();
        }
        stream->close();
        stream->unlock();
        channel->releaseOutStream();
    } else {
        writeToBuffer((const uint8_t*) s, length);
        char c = '\n';
        writeToBuffer((uint8_t*) &c, 1);
    }
}

template<class RP>
void ZcodeDebugOutput<RP>::attemptFlush() {
    if (position != 0 && channel != NULL && (!channel->hasOutStream() || !channel->acquireOutStream()->isLocked())) {
        channel->acquireOutStream()->lock();
        ZcodeOutStream<RP> *stream = channel->acquireOutStream();
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
        flushBuffer(channel->acquireOutStream());
        stream->close();
        stream->unlock();
        channel->releaseOutStream();
    }
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(ZcodeDebugOutputMode m) {
    switch (m) {
    case ZcodeDebugOutputMode::hex:
        state.isCharacter = false;
        state.isDecimal = false;
        state.isHex = true;
        break;
    case decimal:
        state.isCharacter = false;
        state.isDecimal = false;
        state.isHex = true;
        break;
    case character:
        state.isCharacter = false;
        state.isDecimal = false;
        state.isHex = true;
        break;
    case normal:
        state.isCharacter = false;
        state.isDecimal = false;
        state.isHex = false;
        break;
    case endl:
        char c = '\n';
        writeToBuffer((const uint8_t*) &c, 1);
        attemptFlush();
        break;
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(const char *s) {
    debugOutputBufferLength_t l = 0;
    while (s[l] != 0) {
        l++;
    }
    writeToBuffer((const uint8_t*) s, l);
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(bool b) {
    if (b) {
        writeToBuffer((const uint8_t*) "true", 4);
    } else {
        writeToBuffer((const uint8_t*) "false", 5);
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(char c) {
    if (state.isHex || state.isDecimal) {
        *this << (uint8_t) c;
    } else {
        writeToBuffer((const uint8_t*) &c, 1);
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(int8_t i) {
    if (state.isCharacter) {
        *this << (char) i;
    } else if (state.isHex) {
        *this << (uint8_t) i;
    } else {
        char c = 0;
        if (i < 0) {
            i = -i;
            c = '-';
            writeToBuffer((const uint8_t*) &c, 1);
        }
        int8_t h = i / 100;
        if (h != 0) {
            c = '0' + h;
            writeToBuffer((const uint8_t*) &c, 1);
        }
        int8_t t = (i / 10) % 10;
        if (t != 0 || h != 0) {
            c = '0' + t;
            writeToBuffer((const uint8_t*) &c, 1);
        }
        int8_t u = i % 10;
        c = '0' + u;
        writeToBuffer((const uint8_t*) &c, 1);
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(uint8_t i) {
    if (state.isCharacter) {
        *this << (char) i;
    } else if (state.isHex) {
        char c;
        if (i > 15) {
            c = toHex(i >> 4);
            writeToBuffer((const uint8_t*) &c, 1);
        }
        c = toHex(i & 0x0F);
        writeToBuffer((const uint8_t*) &c, 1);
    } else {
        char c = 0;
        int8_t h = i / 100;
        if (h != 0) {
            c = (char) ('0' + h);
            writeToBuffer((const uint8_t*) &c, 1);
        }
        int8_t t = (i / 10) % 10;
        if (t != 0 || h != 0) {
            c = (char) ('0' + t);
            writeToBuffer((const uint8_t*) &c, 1);
        }
        int8_t u = i % 10;
        c = (char) ('0' + u);
        writeToBuffer((const uint8_t*) &c, 1);
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(int16_t i) {
    if (state.isCharacter) {
        *this << (char) (i >> 8);
        *this << (char) i;
    } else if (state.isHex) {
        *this << (uint16_t) i;
    } else {
        char c;
        if (i < 0) {
            i = -i;
            c = '-';
            writeToBuffer((const uint8_t*) &c, 1);
        }
        *this << (uint16_t) i;
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(uint16_t v) {
    if (state.isCharacter) {
        *this << (char) (v >> 8);
        *this << (char) v;
    } else if (state.isHex) {
        for (int i = 3; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int i = 4; i >= 0; i--) {
            uint16_t div = 1;
            for (int j = 0; j < i; ++j) {
                div *= 10;
            }
            if (hasStartedPrint || v >= div) {
                hasStartedPrint = true;
                char c = '0' + (v / div) % 10;
                writeToBuffer((const uint8_t*) &c, 1);
            }
        }
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(int32_t i) {
    if (state.isCharacter) {
        *this << (char) (i >> 24);
        *this << (char) (i >> 16);
        *this << (char) (i >> 8);
        *this << (char) i;
    } else if (state.isHex) {
        *this << (uint32_t) i;
    } else {
        char c = 0;
        if (i < 0) {
            i = -i;
            c = '-';
            writeToBuffer((const uint8_t*) &c, 1);
        }
        *this << (uint32_t) i;
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(uint32_t v) {
    if (state.isCharacter) {
        *this << (char) (v >> 24);
        *this << (char) (v >> 16);
        *this << (char) (v >> 8);
        *this << (char) v;
    } else if (state.isHex) {
        for (int i = 7; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int i = 9; i >= 0; i--) {
            uint16_t div = 1;
            for (int j = 0; j < i; ++j) {
                div *= 10;
            }
            if (hasStartedPrint || v >= div) {
                hasStartedPrint = true;
                char c = '0' + (v / div) % 10;
                writeToBuffer((const uint8_t*) &c, 1);
            }
        }
    }
    return *this;

}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(int64_t i) {
    if (state.isCharacter) {
        *this << (char) i;
    } else if (state.isHex) {
        *this << (uint64_t) i;
    } else {
        char c = 0;
        if (i < 0) {
            i = -i;
            c = '-';
            writeToBuffer((const uint8_t*) &c, 1);
        }
        *this << (uint64_t) i;
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(uint64_t v) {
    if (state.isCharacter) {
        for (int i = 7; i >= 0; i--) {
            *this << (char) (v >> (i * 8));
        }
        *this << (char) v;
    } else if (state.isHex) {
        for (int i = 15; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int i = 19; i >= 0; i--) {
            uint16_t div = 1;
            for (int j = 0; j < i; ++j) {
                div *= 10;
            }
            if (hasStartedPrint || v >= div) {
                hasStartedPrint = true;
                char c = '0' + (v / div) % 10;
                writeToBuffer((const uint8_t*) &c, 1);
            }
        }
    }
    return *this;
}

template<class RP>
ZcodeDebugOutput<RP>& ZcodeDebugOutput<RP>::operator <<(ZcodeCommandSlot<RP> *s) {
    ZcodeDebugOutStream<RP> str = getAsOutStream();
    s->getFields()->copyFieldTo(&str, 'R');
    s->getFields()->copyFieldTo(&str, 'E');
    s->getFields()->copyTo(&str);
    s->getBigField()->copyTo(&str);
    s->getFields()->copyFieldTo(&str, 'S');
    return *this;
}

#include "parsing/ZcodeCommandChannel.hpp"
#include "ZcodeOutStream.hpp"
#include "parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_ */

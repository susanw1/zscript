/*
 * ZcodeDebugOutput.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_

#include "ZcodeIncludes.hpp"
#include  "ZcodeOutStream.hpp"

enum ZcodeDebugOutputMode {
    hex, decimal, character, normal, endl
};

struct ZcodeDebugOutputState {
    bool isHex :1;
    bool isDecimal :1;
    bool isCharacter :1;
};

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class ZcodeDebugOutStream: public ZcodeOutStream<ZP> {
private:

    ZcodeDebugOutput<ZP> *debug;

public:
    ZcodeDebugOutStream(ZcodeDebugOutput<ZP> *debug) :
            debug(debug) {
    }

    void writeByte(uint8_t value);

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) {
    }

    bool isOpen() {
        return true;
    }

    void close() {
    }
};

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSlot;

template<class ZP>
class ZcodeDebugOutput {
private:
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename ZP::debugOutputBufferLength_t debugOutputBufferLength_t;
    typedef typename ZP::Strings::string_t string_t;

    uint8_t debugBuffer[ZP::debugBufferLength];
    ZcodeCommandChannel<ZP> *channel = NULL;
    debugOutputBufferLength_t position;
    ZcodeDebugOutputState state;

    friend ZcodeDebugOutStream<ZP> ;

    void flushBuffer(ZcodeOutStream<ZP> *stream);

    void writeToBuffer(const uint8_t *b, debugOutputBufferLength_t length);

    char toHex(uint8_t i) {
        return (char) (i >= 10 ? i + 'a' - 10 : i + '0');
    }

public:
    void setDebugChannel(ZcodeCommandChannel<ZP> *channel);

    void println(const char *s);

    ZcodeDebugOutput<ZP>& operator <<(ZcodeDebugOutputMode m);

    ZcodeDebugOutput<ZP>& operator <<(string_t s);

    ZcodeDebugOutput<ZP>& operator <<(const char *s);

    ZcodeDebugOutput<ZP>& operator <<(bool b);

    ZcodeDebugOutput<ZP>& operator <<(char c);

    ZcodeDebugOutput<ZP>& operator <<(int8_t i);

    ZcodeDebugOutput<ZP>& operator <<(uint8_t i);

    ZcodeDebugOutput<ZP>& operator <<(int16_t i);

    ZcodeDebugOutput<ZP>& operator <<(uint16_t i);

    ZcodeDebugOutput<ZP>& operator <<(int32_t i);

    ZcodeDebugOutput<ZP>& operator <<(uint32_t i);

    ZcodeDebugOutput<ZP>& operator <<(int64_t i);

    ZcodeDebugOutput<ZP>& operator <<(uint64_t i);

    ZcodeDebugOutput<ZP>& operator <<(ZcodeCommandSlot<ZP> *s);

    void println(const char *s, debugOutputBufferLength_t length);

    void attemptFlush();

    ZcodeDebugOutStream<ZP> getAsOutStream() {
        return ZcodeDebugOutStream<ZP>(this);
    }
};

template<class ZP>
void ZcodeDebugOutStream<ZP>::writeByte(uint8_t value) {
    debug->writeToBuffer(&value, 1);
}

template<class ZP>
void ZcodeDebugOutput<ZP>::flushBuffer(ZcodeOutStream<ZP> *stream) {
    state.isCharacter = false;
    state.isDecimal = false;
    state.isHex = false;
    debugOutputBufferLength_t curPos = 0;
    while (curPos < position) {
        stream->markDebug();
        while (curPos < ZP::debugBufferLength && debugBuffer[curPos] != Zchars::EOL_SYMBOL) {
            stream->writeByte(debugBuffer[curPos++]);
        }
        stream->writeCommandSequenceSeparator();
        curPos++;
    }
    if (position == ZP::debugBufferLength + 1) {
        char c;
        bigFieldAddress_t i = 0;
        while ((c = ZP::Strings::getChar(ZP::Strings::debugOverrun, i++)) != 0) {
            stream->writeByte(c);
        }
    }
    position = 0;
}

template<class ZP>
void ZcodeDebugOutput<ZP>::writeToBuffer(const uint8_t *b, debugOutputBufferLength_t length) {
    if (position >= ZP::debugBufferLength) {
        return;
    }
    debugOutputBufferLength_t lenToCopy = length;
    if (position + length >= ZP::debugBufferLength) {
        lenToCopy = (debugOutputBufferLength_t) (ZP::debugBufferLength - position);
    }
    if (position < ZP::debugBufferLength) {
        for (debugOutputBufferLength_t i = 0; i < lenToCopy; ++i) {
            debugBuffer[position + i] = b[i];
        }
    }
    position = (debugOutputBufferLength_t) (position + lenToCopy);
}

template<class ZP>
void ZcodeDebugOutput<ZP>::setDebugChannel(ZcodeCommandChannel<ZP> *channel) {
    if (this->channel != NULL) {
        this->channel->stateChange(RELEASED_FROM_DEBUG);
    }
    this->channel = channel;
    this->channel->stateChange(SET_AS_DEBUG);
}

template<class ZP>
void ZcodeDebugOutput<ZP>::println(const char *s) {
    debugOutputBufferLength_t l = 0;
    while (s[l] != '\0') {
        l++;
    }
    println(s, l);
}

template<class ZP>
void ZcodeDebugOutput<ZP>::println(const char *s, debugOutputBufferLength_t length) {
    if (channel != NULL && channel->out->lock()) {
        ZcodeOutStream<ZP> *stream = channel->out;
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
        debugOutputBufferLength_t prevPos = 0;
        for (debugOutputBufferLength_t i = 0; i < length; ++i) {
            if (s[i] == Zchars::EOL_SYMBOL) {
                stream->markDebug()
                        ->writeBytes((const uint8_t*) (s + prevPos), (bigFieldAddress_t) (i - prevPos))
                        ->writeCommandSequenceSeparator();
                prevPos = (debugOutputBufferLength_t) (i + 1);
            }
        }
        if (prevPos != length) {
            stream->markDebug()->writeBytes((const uint8_t*) (s + prevPos), (bigFieldAddress_t) (length - prevPos))->writeCommandSequenceSeparator();
        }
        stream->close();
        stream->unlock();
    } else {
        writeToBuffer((const uint8_t*) s, length);
        char c = Zchars::EOL_SYMBOL;
        writeToBuffer((uint8_t*) &c, 1);
    }
}

template<class ZP>
void ZcodeDebugOutput<ZP>::attemptFlush() {
    if (position != 0 && channel != NULL && channel->out->lock()) {
        ZcodeOutStream<ZP> *stream = channel->out;
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
        flushBuffer(stream);
        stream->close();
        stream->unlock();
    }
}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(ZcodeDebugOutputMode m) {
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
        char c = Zchars::EOL_SYMBOL;
        writeToBuffer((const uint8_t*) &c, 1);
        attemptFlush();
        break;
    }
    return *this;
}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(const char *s) {
    debugOutputBufferLength_t l = 0;
    while (s[l] != 0) {
        l++;
    }
    writeToBuffer((const uint8_t*) s, l);
    return *this;
}
template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(string_t s) {

    debugOutputBufferLength_t l = 0;
    while (ZP::Strings::getChar(s, l) != 0) {
        l++;
    }
    char tmp[l];
    for (debugOutputBufferLength_t i = 0; i < l; ++i) {
        tmp[i] = ZP::Strings::getChar(s, i);
    }
    writeToBuffer((const uint8_t*) tmp, l);
    return *this;
}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(bool b) {
    if (b) {
        this << ZP::Strings::boolTrue;
    } else {
        this << ZP::Strings::boolFalse;
    }
    return *this;
}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(char c) {
    if (state.isHex || state.isDecimal) {
        *this << (uint8_t) c;
    } else {
        writeToBuffer((const uint8_t*) &c, 1);
    }
    return *this;
}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(int8_t i) {
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(uint8_t i) {
    if (state.isCharacter) {
        *this << (char) i;
    } else if (state.isHex) {
        char c;
        if (i > 15) {
            c = toHex((uint8_t) (i >> 4));
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(int16_t i) {
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(uint16_t v) {
    if (state.isCharacter) {
        *this << (char) (v >> 8);
        *this << (char) v;
    } else if (state.isHex) {
        for (uint8_t i = 3; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int8_t i = 4; i >= 0; i--) {
            uint16_t div = 1;
            for (uint8_t j = 0; j < i; ++j) {
                div = (uint16_t) (div * 10);
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(int32_t i) {
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(uint32_t v) {
    if (state.isCharacter) {
        *this << (char) (v >> 24);
        *this << (char) (v >> 16);
        *this << (char) (v >> 8);
        *this << (char) v;
    } else if (state.isHex) {
        for (uint8_t i = 7; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int8_t i = 9; i >= 0; i--) {
            uint32_t div = 1;
            for (uint8_t j = 0; j < i; ++j) {
                div = (uint32_t) (div * 10);
            }
            if (hasStartedPrint || v >= div) {
                hasStartedPrint = true;
                char c = (char) ('0' + (v / div) % 10);
                writeToBuffer((const uint8_t*) &c, 1);
            }
        }
    }
    return *this;

}

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(int64_t i) {
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(uint64_t v) {
    if (state.isCharacter) {
        for (uint8_t i = 7; i >= 0; i--) {
            *this << (char) (v >> (i * 8));
        }
        *this << (char) v;
    } else if (state.isHex) {
        for (uint8_t i = 15; i >= 0; i--) {
            char c = toHex((v >> (4 * i)) & 0x0F);
            writeToBuffer((const uint8_t*) &c, 1);
        }
    } else {
        bool hasStartedPrint = false;
        for (int8_t i = 19; i >= 0; i--) {
            uint64_t div = 1;
            for (uint8_t j = 0; j < i; ++j) {
                div = (uint64_t) (div * 10);
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

template<class ZP>
ZcodeDebugOutput<ZP>& ZcodeDebugOutput<ZP>::operator <<(ZcodeCommandSlot<ZP> *s) {
    ZcodeDebugOutStream<ZP> str = getAsOutStream();
    s->getFields()->copyFieldTo(&str, Zchars::CMD_PARAM);
    s->getFields()->copyFieldTo(&str, Zchars::ECHO_PARAM);
    s->getFields()->copyTo(&str);
    s->getBigField()->copyTo(&str);
    s->getFields()->copyFieldTo(&str, Zchars::STATUS_RESP_PARAM);
    return *this;
}

#endif /* SRC_TEST_CPP_ZCODE_ZCODEDEBUGOUTPUT_HPP_ */

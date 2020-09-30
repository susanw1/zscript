/*
 * RCodeDebugOutput.hpp
 *
 *  Created on: 18 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEDEBUGOUTPUT_HPP_
#define SRC_TEST_CPP_RCODE_RCODEDEBUGOUTPUT_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "AbstractRCodeOutStream.hpp"

enum RCodeDebugOutputMode {
    hex, decimal, character, normal, endl
};
struct RCodeDebugOutputState {
    bool isHex :1;
    bool isDecimal :1;
    bool isCharacter :1;
};
class RCodeDebugOutput;

class RCodeDebugOutStream: public AbstractRCodeOutStream {
    RCodeDebugOutput *debug;
public:
    RCodeDebugOutStream(RCodeDebugOutput *debug) :
            debug(debug) {
    }
    virtual void writeByte(uint8_t value);

    virtual RCodeOutStream* writeBytes(uint8_t const *value,
            bigFieldAddress_t length);

    void openResponse(RCodeCommandChannel *target) {
    }

    void openNotification(RCodeCommandChannel *target) {
    }

    void openDebug(RCodeCommandChannel *target) {
    }

    bool isOpen() {
        return true;
    }

    void close() {
    }
};
class RCodeCommandChannel;
class RCodeOutStream;
class RCodeCommandSlot;
class RCodeDebugOutput {
private:
    uint8_t debugBuffer[RCodeParameters::debugBufferLength];
    RCodeCommandChannel *channel = NULL;
    debugOutputBufferLength_t position;
    RCodeDebugOutputState state;

    friend RCodeDebugOutStream;

    void flushBuffer(RCodeOutStream *stream);

    void writeToBuffer(const uint8_t *b, debugOutputBufferLength_t length);
public:

    void setDebugChannel(RCodeCommandChannel *channel);

    void println(const char *s);

    RCodeDebugOutput& operator <<(RCodeDebugOutputMode m);

    RCodeDebugOutput& operator <<(const char *s);

    RCodeDebugOutput& operator <<(bool b);

    RCodeDebugOutput& operator <<(char c);

    RCodeDebugOutput& operator <<(int8_t i);

    RCodeDebugOutput& operator <<(uint8_t i);

    RCodeDebugOutput& operator <<(int16_t i);

    RCodeDebugOutput& operator <<(uint16_t i);

    RCodeDebugOutput& operator <<(int32_t i);

    RCodeDebugOutput& operator <<(uint32_t i);

    RCodeDebugOutput& operator <<(int64_t i);

    RCodeDebugOutput& operator <<(uint64_t i);

    RCodeDebugOutput& operator <<(RCodeCommandSlot *s);

    void println(const char *s, debugOutputBufferLength_t length);

    void attemptFlush();

    RCodeDebugOutStream getAsOutStream() {
        return RCodeDebugOutStream(this);
    }
};

#include "parsing/RCodeCommandChannel.hpp"
#include "RCodeOutStream.hpp"
#include "parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODEDEBUGOUTPUT_HPP_ */

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

class RCodeCommandChannel;
class RCodeOutStream;
class RCodeDebugOutput {
private:
    uint8_t debugBuffer[RCodeParameters::debugBufferLength];
    RCodeCommandChannel *channel = NULL;
    uint16_t position;

    void flushBuffer(RCodeOutStream *stream);

    void writeToBuffer(const uint8_t *b, int length);
public:

    void setDebugChannel(RCodeCommandChannel *channel);

    void println(const char *s);

    void println(const char *s, int length);

    void attemptFlush();
};

#include "parsing/RCodeCommandChannel.hpp"
#include "RCodeOutStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODEDEBUGOUTPUT_HPP_ */

/*
 * RCodeCommandChannel.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"

class RCodeLockSet;
class RCodeChannelInStream;
class RCodeOutStream;
class RCodeCommandSequence;

class RCodeCommandChannel {
public:
    virtual RCodeChannelInStream* acquireInStream() = 0;

    virtual bool hasInStream() = 0; // This method must be implemented such that if it returns true, calling acquireInStream() does not change the state of the channel,
                                    // and if it returns false, acquireInStream() returns an unlocked out stream.

    virtual RCodeOutStream* acquireOutStream() = 0;

    virtual bool hasOutStream() = 0; // This method must be implemented such that if it returns true, calling acquireOutStream() does not change the state of the channel,
                                     // and if it returns false, acquireOutStream() returns an unlocked out stream.

    virtual bool hasCommandSequence() = 0;

    virtual RCodeCommandSequence* getCommandSequence() = 0;

    virtual bool isPacketBased() = 0;

    virtual void releaseInStream() = 0;

    virtual void releaseOutStream() = 0;

    virtual void setAsNotificationChannel() = 0;

    virtual void releaseFromNotificationChannel() = 0;

    virtual void setAsDebugChannel() = 0;

    virtual void releaseFromDebugChannel() = 0;

    virtual void lock() = 0;

    virtual bool canLock() = 0;

    virtual void unlock() = 0;

    virtual ~RCodeCommandChannel() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_ */

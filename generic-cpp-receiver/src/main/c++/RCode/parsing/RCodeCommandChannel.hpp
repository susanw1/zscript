/*
 * RCodeCommandChannel.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_
#include "../RCodeIncludes.hpp"

template <class RP>
class RCodeLockSet;

template <class RP>
class RCodeChannelInStream;

template <class RP>
class RCodeOutStream;

template <class RP>
class RCodeCommandSequence;


template <class RP>
class RCodeCommandChannel {
public:
    virtual RCodeChannelInStream<RP>* acquireInStream() = 0;

    virtual bool hasInStream() = 0; // This method must be implemented such that if it returns true, calling acquireInStream() does not change the state of the channel,
                                    // and if it returns false, acquireInStream() returns an unlocked out stream.

    virtual RCodeOutStream<RP>* acquireOutStream() = 0;

    virtual bool hasOutStream() = 0; // This method must be implemented such that if it returns true, calling acquireOutStream() does not change the state of the channel,
                                     // and if it returns false, acquireOutStream() returns an unlocked out stream.

    virtual bool hasCommandSequence() = 0;

    virtual RCodeCommandSequence<RP>* getCommandSequence() = 0;

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

};

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDCHANNEL_HPP_ */

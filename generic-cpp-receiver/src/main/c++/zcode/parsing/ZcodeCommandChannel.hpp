/*
 * ZcodeCommandChannel.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeLockSet;

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeCommandChannel {
    public:
        virtual ZcodeChannelInStream<ZP>* acquireInStream() = 0;

        virtual bool hasInStream() = 0; // This method must be implemented such that if it returns true, calling acquireInStream() does not change the state of the channel,
                                        // and if it returns false, acquireInStream() returns an unlocked out stream.

        virtual ZcodeOutStream<ZP>* acquireOutStream() = 0;

        virtual bool hasOutStream() = 0; // This method must be implemented such that if it returns true, calling acquireOutStream() does not change the state of the channel,
                                         // and if it returns false, acquireOutStream() returns an unlocked out stream.

        virtual bool hasCommandSequence() = 0;

        virtual ZcodeCommandSequence<ZP>* getCommandSequence() = 0;

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

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDCHANNEL_HPP_ */

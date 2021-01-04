/*
 * RCodeVoidChannel.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEVOIDCHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_RCODEVOIDCHANNEL_HPP_

#include "parsing/RCodeCommandChannel.hpp"
#include "RCodeOutStream.hpp"

template<class RP>
class RCodeCommandSequence;

template<class RP>
class RCodeChannelInStream;

template<class RP>
class RCodeVoidChannel: public RCodeCommandChannel<RP> {
    template<class RP1>
    class RCodeVoidOutStream : public RCodeOutStream<RP1> {
        bool isLocked() {
            return false;
        }
        virtual RCodeOutStream<RP1>* markDebug() {
            return this;
        }

        virtual RCodeOutStream<RP1>* markNotification() {
            return this;
        }

        virtual RCodeOutStream<RP1>* markBroadcast() {
            return this;
        }
        virtual RCodeOutStream<RP1>* writeStatus(RCodeResponseStatus st) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeField(char f, typename RP1::fieldUnit_t v) {
            return this;
        }

        virtual RCodeOutStream<RP1>* continueField(typename RP1::fieldUnit_t v) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeBigHexField(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeBigStringField(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeBigStringField(char const *s) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeBytes(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeCommandSeperator() {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeCommandSequenceErrorHandler() {
            return this;
        }

        virtual RCodeOutStream<RP1>* writeCommandSequenceSeperator() {
            return this;
        }

        virtual void openResponse(RCodeCommandChannel<RP1> *target) {
        }

        virtual void openNotification(RCodeCommandChannel<RP1> *target) {
        }

        virtual void openDebug(RCodeCommandChannel<RP1> *target) {
        }

        virtual bool isOpen() {
            return false;
        }

        virtual void close() {
        }
    };
    RCodeVoidOutStream<RP> out;
public:
    virtual RCodeChannelInStream<RP>* acquireInStream() {
        return NULL;
    }
    bool hasInStream() {
        return true;
    }

    virtual RCodeOutStream<RP>* acquireOutStream() {
        return &out;
    }
    virtual bool hasOutStream() {
        return true;
    }
    virtual bool hasCommandSequence() {
        return false;
    }

    virtual RCodeCommandSequence<RP>* getCommandSequence() {
        return NULL;
    }

    virtual bool isPacketBased() {
        return false;
    }

    virtual void releaseInStream() {
    }

    virtual void releaseOutStream() {
    }

    virtual void setAsNotificationChannel() {
    }

    virtual void releaseFromNotificationChannel() {
    }

    virtual void setAsDebugChannel() {
    }

    virtual void releaseFromDebugChannel() {
    }

    virtual void lock() {
    }

    virtual bool canLock() {
        return true;
    }

    virtual void unlock() {
    }
};
#endif /* SRC_TEST_CPP_RCODE_RCODEVOIDCHANNEL_HPP_ */

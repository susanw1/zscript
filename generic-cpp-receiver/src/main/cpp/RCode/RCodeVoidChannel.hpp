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

class RCodeCommandSequence;
class RCodeInStream;

class RCodeVoidChannel: public RCodeCommandChannel {
    class RCodeVoidOutStream: public RCodeOutStream {
        bool isLocked() {
            return false;
        }
        virtual RCodeOutStream* markDebug() {
            return this;
        }

        virtual RCodeOutStream* markNotification() {
            return this;
        }

        virtual RCodeOutStream* markBroadcast() {
            return this;
        }
        virtual RCodeOutStream* writeStatus(RCodeResponseStatus st) {
            return this;
        }

        virtual RCodeOutStream* writeField(char f, fieldUnit v) {
            return this;
        }

        virtual RCodeOutStream* continueField(fieldUnit v) {
            return this;
        }

        virtual RCodeOutStream* writeBigHexField(uint8_t const *value,
                bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream* writeBigStringField(uint8_t const *value,
                bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream* writeBigStringField(char const *s) {
            return this;
        }

        virtual RCodeOutStream* writeBytes(uint8_t const *value,
                bigFieldAddress_t length) {
            return this;
        }

        virtual RCodeOutStream* writeCommandSeperator() {
            return this;
        }

        virtual RCodeOutStream* writeCommandSequenceErrorHandler() {
            return this;
        }

        virtual RCodeOutStream* writeCommandSequenceSeperator() {
            return this;
        }

        virtual void openResponse(RCodeCommandChannel *target) {
        }

        virtual void openNotification(RCodeCommandChannel *target) {
        }

        virtual void openDebug(RCodeCommandChannel *target) {
        }

        virtual bool isOpen() {
            return false;
        }

        virtual void close() {
        }
    };
    RCodeVoidOutStream out;
public:
    virtual RCodeInStream* getInStream() {
        return NULL;
    }

    virtual RCodeOutStream* getOutStream() {
        return &out;
    }

    virtual bool hasCommandSequence() {
        return false;
    }

    virtual RCodeCommandSequence* getCommandSequence() {
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

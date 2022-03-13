/*
 * ZcodeNoopChannel.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_

#include "parsing/ZcodeCommandChannel.hpp"
#include "ZcodeOutStream.hpp"

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeNoopChannel: public ZcodeCommandChannel<ZP> {
    template<class RP1>
    class ZcodeNoopOutStream : public ZcodeOutStream<RP1> {
        bool isLocked() {
            return false;
        }
        virtual ZcodeOutStream<RP1>* markDebug() {
            return this;
        }

        virtual ZcodeOutStream<RP1>* markNotification() {
            return this;
        }

        virtual ZcodeOutStream<RP1>* markBroadcast() {
            return this;
        }
        virtual ZcodeOutStream<RP1>* writeStatus(ZcodeResponseStatus st) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeField(char f, typename RP1::fieldUnit_t v) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* continueField(typename RP1::fieldUnit_t v) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeBigHexField(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeBigStringField(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeBigStringField(char const *s) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeBytes(uint8_t const *value, typename RP1::bigFieldAddress_t length) {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeCommandSeperator() {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeCommandSequenceErrorHandler() {
            return this;
        }

        virtual ZcodeOutStream<RP1>* writeCommandSequenceSeperator() {
            return this;
        }

        virtual void openResponse(ZcodeCommandChannel<RP1> *target) {
        }

        virtual void openNotification(ZcodeCommandChannel<RP1> *target) {
        }

        virtual void openDebug(ZcodeCommandChannel<RP1> *target) {
        }

        virtual bool isOpen() {
            return false;
        }

        virtual void close() {
        }
    };
    ZcodeNoopOutStream<ZP> out;
public:
    virtual ZcodeChannelInStream<ZP>* acquireInStream() {
        return NULL;
    }
    bool hasInStream() {
        return true;
    }

    virtual ZcodeOutStream<ZP>* acquireOutStream() {
        return &out;
    }
    virtual bool hasOutStream() {
        return true;
    }
    virtual bool hasCommandSequence() {
        return false;
    }

    virtual ZcodeCommandSequence<ZP>* getCommandSequence() {
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
#endif /* SRC_TEST_CPP_ZCODE_ZCODENOOPCHANNEL_HPP_ */

/*
 * ZcodeExecutionSpaceChannel.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_

#include "../parsing/ZcodeCommandSequence.hpp"
#include "../parsing/ZcodeCommandChannel.hpp"
#include "ZcodeExecutionSpaceChannelIn.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeExecutionSpace;

template<class ZP>
class ZcodeExecutionSpaceChannel: public ZcodeCommandChannel<ZP> {
    private:
        typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;

        Zcode<ZP> *zcode;
        ZcodeExecutionSpace<ZP> *space;
        ZcodeCommandSequence<ZP> sequence;
        ZcodeExecutionSpaceChannelIn<ZP> *in;
        ZcodeOutStream<ZP> *out = NULL;
        ZcodeLockSet<ZP> locks;
        executionSpaceAddress_t position = 0;
        uint8_t delayCounter = 0;
        uint8_t lockCount = 0;

    public:
        ZcodeExecutionSpaceChannel(Zcode<ZP> *zcode, ZcodeExecutionSpace<ZP> *space) :
                zcode(zcode), space(space), sequence(zcode, this), in(NULL) {
            locks.addLock(ZP::executionSpaceLock, false);
        }

        ZcodeCommandSequence<ZP>* getCommandSequence() {
            return &sequence;
        }

        void move(executionSpaceAddress_t position) {
            this->position = position;
        }

        bool isPacketBased() {
            return false;
        }

        ZcodeExecutionSpaceChannelIn<ZP>* acquireInStream() {
            if (this->in == NULL) {
                in = space->acquireInStream(position);
            }
            return in;
        }

        bool hasInStream() {
            return in != NULL;
        }

        ZcodeOutStream<ZP>* acquireOutStream() {
            if (out == NULL) {
                out = space->acquireOutStream();
            }
            return out;
        }

        bool hasOutStream() {
            return out != NULL;
        }

        bool hasCommandSequence() {
            bool has = space->isRunning() && space->hasInStream() && space->hasOutStream() && in == NULL && out == NULL && delayCounter >= space->getDelay();
            delayCounter++;
            return has;
        }

        void releaseInStream() {
            delayCounter = 0;
            if (in != NULL) {
                position = (executionSpaceAddress_t) (in->getPosition() + 1);
                if (position >= space->getLength()) {
                    position = 0;
                }
                space->releaseInStream(in);
            }
            in = NULL;
        }

        void releaseOutStream() {
            space->releaseOutStream((ZcodeExecutionSpaceOut<ZP>*) out);
            out = NULL;
        }

        void lock() {
            if (lockCount == 0) {
                zcode->lock(&locks);
            }
            lockCount++;
        }

        bool canLock() {
            return lockCount > 0 || zcode->canLock(&locks);
        }

        void unlock() {
            lockCount--;
            if (lockCount == 0) {
                zcode->unlock(&locks);
            }
        }

        void setAsNotificationChannel() {
        }

        void releaseFromNotificationChannel() {
        }

        void setAsDebugChannel() {
        }

        void releaseFromDebugChannel() {
        }
};

#include "../Zcode.hpp"
#include "ZcodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_ */

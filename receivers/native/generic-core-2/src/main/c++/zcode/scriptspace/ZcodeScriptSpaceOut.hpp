/*
 * ZcodeExecutionSpaceOut.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeOutStream.hpp"

template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeScriptSpaceOut: public ZcodeOutStream<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    ZcodeScriptSpace<ZP> *space;
    uint8_t buffer[ZP::scriptOutBufferSize];
    scriptSpaceAddress_t length = 0;
    scriptSpaceAddress_t lastEndPos = 0;
    bool overLength = false;
    bool inUse = false;
    bool dataBufferFull = false;
    bool currentlyOpen = false;
    ZcodeResponseStatus status = OK;

public:
    ZcodeScriptSpaceOut(ZcodeScriptSpace<ZP> *space) :
            space(space) {
    }

    void initialSetup(ZcodeScriptSpace<ZP> *space) {
        this->space = space;
    }

    void writeByte(uint8_t value) {
        if (this->recentStatus != OK) {
            status = this->recentStatus;
        }
        if (this->recentBreak != 0) {
            if (this->recentBreak != EOL_SYMBOL) {
                if (!overLength) {
                    lastEndPos = length;
                }
            }
            this->recentBreak = (Zchars) 0;
        }
        if (length == ZP::scriptOutBufferSize) {
            overLength = true;
        }
        if (currentlyOpen && !overLength) {
            buffer[length++] = value;
        }
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) {
        if (type == ZcodeOutStreamOpenType::RESPONSE) {
            currentlyOpen = true;
            length = 0;
            overLength = false;
        }
    }

    bool isOpen() {
        return currentlyOpen;
    }

    void close() {
        currentlyOpen = false;
        if (status != OK) {
            flush();
        }
    }

    bool flush();

    bool isDataBufferFull() {
        return dataBufferFull;
    }
};

template<class ZP>
bool ZcodeScriptSpaceOut<ZP>::flush() {
    if (status != CMD_FAIL && status != OK) {
        space->setRunning(false);
        space->setFailed(true);
    }
    if (!space->getNotificationChannel()->out->isLocked()) {
        ZcodeOutStream<ZP> *out = space->getNotificationChannel()->out;
        out->lock();
        if (out->isOpen()) {
            out->close();
        }
        out->openNotification(space->getNotificationChannel());
        out->mostRecent = space;
        out->markNotification();
        out->writeField8(Zchars::NOTIFY_TYPE_PARAM, (uint8_t) 2);
        if (overLength) {
            out->writeBytes(buffer, lastEndPos);
            out->writeCommandSeparator();
            out->writeStatus(RESP_TOO_LONG);
            out->writeCommandSeparator();
            out->writeStatus(status);
            out->writeCommandSequenceSeparator();
        } else {
            out->writeBytes(buffer, length);
        }
        out->close();
        out->unlock();
        dataBufferFull = false;
        status = OK;
        return true;
    } else {
        dataBufferFull = true;
        return false;
    }
}

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_ */

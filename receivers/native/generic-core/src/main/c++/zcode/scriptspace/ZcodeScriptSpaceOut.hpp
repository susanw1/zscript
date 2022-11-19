/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACEOUT_HPP_
#define SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACEOUT_HPP_

#include "../ZcodeIncludes.hpp"
#include "../ZcodeOutStream.hpp"

template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeScriptSpaceOut: public ZcodeOutStream<ZP> {
private:
    struct ZcodeScriptSpaceOutStatus {
        bool overLength :1;
        bool inUse :1;
        bool dataBufferFull :1;
        bool currentlyOpen :1;
        void reset() {
            overLength = false;
            inUse = false;
            dataBufferFull = false;
            currentlyOpen = false;
        }
    };
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    ZcodeScriptSpace<ZP> *space;
    uint8_t buffer[ZP::scriptOutBufferSize];
    uint8_t readBuffer[ZP::scriptOutReadBufferSize];

    scriptSpaceAddress_t length = 0;
    scriptSpaceAddress_t lastEndPos = 0;
    ZcodeScriptSpaceOutStatus outStatus;
    uint16_t status = ZcodeResponseStatus::OK;

public:
    ZcodeScriptSpaceOut(ZcodeScriptSpace<ZP> *space) :
            ZcodeOutStream<ZP>(readBuffer, ZP::scriptOutReadBufferSize), space(space) {
        outStatus.reset();
    }
    void reset() {
        outStatus.reset();
        length = 0;
        lastEndPos = 0;
        status = ZcodeResponseStatus::OK;
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
                if (!outStatus.overLength) {
                    lastEndPos = length;
                }
            }
            this->recentBreak = (Zchars) 0;
        }
        if (length == ZP::scriptOutBufferSize) {
            outStatus.overLength = true;
        }
        if (outStatus.currentlyOpen && !outStatus.overLength) {
            buffer[length++] = value;
        }
    }

    void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) {
        (void) (target);
        if (outStatus.currentlyOpen && type == ZcodeOutStreamOpenType::RESPONSE) {
            outStatus.currentlyOpen = true;
            length = 0;
            outStatus.overLength = false;
        }
    }

    bool isOpen() {
        return outStatus.currentlyOpen;
    }

    void close() {
        if (outStatus.currentlyOpen) {
            outStatus.currentlyOpen = false;
            if (status != OK) {
                flush();
            }
        }
    }

    bool flush();

    bool isDataBufferFull() {
        return outStatus.dataBufferFull;
    }
};

template<class ZP>
bool ZcodeScriptSpaceOut<ZP>::flush() {
    if (status < CMD_FAIL && status != OK) {
        space->setRunning(false);
        space->setFailed(true);
    }
    if (space->getNotificationChannel() == NULL) {
        outStatus.dataBufferFull = false;
        status = OK;
        return true;
    } else if (!space->getNotificationChannel()->out->isLocked()) {
        ZcodeOutStream<ZP> *out = space->getNotificationChannel()->out;
        out->lock();
        out->openNotification(space->getNotificationChannel());
        out->markNotification(ScriptSpaceNotification);
        if (outStatus.overLength) {
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
        outStatus.dataBufferFull = false;
        status = OK;
        return true;
    } else {
        outStatus.dataBufferFull = true;
        return false;
    }
}

#endif /* SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACEOUT_HPP_ */

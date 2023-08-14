/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_
#define SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_
#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"
#include "../tokenizer/TokenRingBuffer.hpp"
#include "ZscriptAddressingContext.hpp"
#include "ZscriptCommandContext.hpp"
#include "../ZscriptResponseStatus.hpp"
#include "../modules/ModuleRegistry.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class SemanticParser;
template<class ZP>
class ZscriptNotificationSource;

enum class SemanticActionType : uint8_t {
    INVALID,
    GO_AROUND,
    WAIT_FOR_TOKENS,
    WAIT_FOR_ASYNC,
    ERROR,
    INVOKE_ADDRESSING,
    ADDRESSING_MOVEALONG,
    RUN_FIRST_COMMAND,
    RUN_COMMAND,
    COMMAND_MOVEALONG,
    END_SEQUENCE,
    CLOSE_PAREN,
    STOPPED
};

enum class NotificationActionType : uint8_t {
    INVALID,
    WAIT_FOR_NOTIFICATION,
    WAIT_FOR_ASYNC,
    NOTIFICATION_BEGIN,
    NOTIFICATION_MOVE_ALONG,
    NOTIFICATION_END
};

template<class ZP>
class ZscriptAction {
private:
    void *source;
    uint8_t typeStored;
    bool isSemantic;

    void performActionSemantic(AbstractOutStream<ZP> *out) {
        SemanticParser<ZP> *parseState = ((SemanticParser<ZP>*) source);
        ZscriptCommandContext<ZP> cmdCtx;
        ZscriptAddressingContext<ZP> addrCtx;
        SemanticActionType type = (SemanticActionType) typeStored;
        bool isMoveAlong;
        if (typeStored == (uint8_t) SemanticActionType::ERROR || typeStored == (uint8_t) SemanticActionType::RUN_FIRST_COMMAND) {
            startResponseSem(out, (uint8_t) 0); //TODO: Sort out what response type...
        }
        switch (type) {
        case SemanticActionType::ERROR:
            out->writeField('S', parseState->getErrorStatus());
            out->endSequence();
            out->close();
            parseState->unlock();
            break;
        case SemanticActionType::INVOKE_ADDRESSING:
            case SemanticActionType::ADDRESSING_MOVEALONG:
            isMoveAlong = typeStored == (uint8_t) SemanticActionType::ADDRESSING_MOVEALONG;
            addrCtx = ZscriptAddressingContext<ZP>(parseState);
            if (isMoveAlong || addrCtx.verify()) {
                ModuleRegistry<ZP>::execute(addrCtx, isMoveAlong);
            }
            break;
        case SemanticActionType::RUN_FIRST_COMMAND:
            case SemanticActionType::RUN_COMMAND:
            case SemanticActionType::COMMAND_MOVEALONG:
            if (typeStored == (uint8_t) SemanticActionType::RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            isMoveAlong = typeStored == (uint8_t) SemanticActionType::COMMAND_MOVEALONG;
            cmdCtx = ZscriptCommandContext<ZP>(parseState, out);
            if (isMoveAlong || (cmdCtx.verify() && !parseState->isEmpty())) {
                ModuleRegistry<ZP>::execute(cmdCtx, isMoveAlong);
                if (cmdCtx.isCommandComplete()) {
                    cmdCtx.status(ResponseStatus::SUCCESS);
                }
            }
            break;
        case SemanticActionType::END_SEQUENCE:
            if (out->isOpen()) {
                out->endSequence();
                out->close();
            }
            parseState->unlock();
            break;
        case SemanticActionType::CLOSE_PAREN:
            out->writeCloseParen();
            break;
        case SemanticActionType::GO_AROUND:
            case SemanticActionType::WAIT_FOR_TOKENS:
            case SemanticActionType::WAIT_FOR_ASYNC:
            case SemanticActionType::STOPPED:
            break;
        case SemanticActionType::INVALID:
            //Unreachable
            break;
        }
    }
    void startResponseSem(AbstractOutStream<ZP> *out, uint8_t respType) {
        if (!out->isOpen()) {
            out->open();
        }
        out->writeField('!', respType);
        if (((SemanticParser<ZP>*) source)->hasEcho()) {
            out->writeField('_', ((SemanticParser<ZP>*) source)->getEcho());
        }
    }
    void sendNormalMarkerPrefix(AbstractOutStream<ZP> *out) {
        uint8_t marker = ((SemanticParser<ZP>*) source)->takeCurrentMarker();
        if (marker != 0) {
            if (marker == ZscriptTokenizer<ZP>::CMD_END_ANDTHEN) {
                out->writeAndThen();
            } else if (marker == ZscriptTokenizer<ZP>::CMD_END_ORELSE) {
                out->writeOrElse();
            } else if (marker == ZscriptTokenizer<ZP>::CMD_END_OPEN_PAREN) {
                out->writeOpenParen();
            } else if (marker == ZscriptTokenizer<ZP>::CMD_END_CLOSE_PAREN) {
                out->writeCloseParen();
            } else if (marker == ZscriptTokenizer<ZP>::NORMAL_SEQUENCE_END) {
            } else {
                //unreachable
            }
        }
    }

    void performActionNotification(AbstractOutStream<ZP> *out) {
        ZscriptNotificationSource<ZP> *notifSrc = ((ZscriptNotificationSource<ZP>*) source);
        NotificationActionType type = (NotificationActionType) typeStored;
        switch (type) {
        case NotificationActionType::NOTIFICATION_BEGIN:
            startResponseNotif(out);
            ModuleRegistry<ZP>::notification(ZscriptNotificationContext<ZP>(notifSrc), false);
            break;
        case NotificationActionType::NOTIFICATION_END:
            out->endSequence();
            out->close();
            notifSrc->unlock();
            break;
        case NotificationActionType::NOTIFICATION_MOVE_ALONG:
            ModuleRegistry<ZP>::notification(ZscriptNotificationContext<ZP>(notifSrc), true);
            break;
        case NotificationActionType::WAIT_FOR_ASYNC:
            case NotificationActionType::WAIT_FOR_NOTIFICATION:
            break;
        case NotificationActionType::INVALID:
            //unreachable...
            break;
        }
        notifSrc->actionPerformed(type);
    }
    void startResponseNotif(AbstractOutStream<ZP> *out) {
        ZscriptNotificationSource<ZP> *notifSrc = ((ZscriptNotificationSource<ZP>*) source);
        if (!out->isOpen()) {
            out->open();
        }
        if (notifSrc->isAddressing()) {
            out->writeField('!', 0);
            //            commandStream.writeField(Zchars.Z_ADDRESSING, (source.getID() >> 4));
        } else {
            out->writeField('!', (notifSrc->getID() >> 4));
        }
    }

public:
    ZscriptAction(SemanticParser<ZP> *source, SemanticActionType type) :
            source(source), typeStored((uint8_t) type), isSemantic(true) {
    }
    ZscriptAction(ZscriptNotificationSource<ZP> *source, NotificationActionType type) :
            source(source), typeStored((uint8_t) type), isSemantic(false) {
    }
    ZscriptAction() :
            source(NULL), typeStored(0), isSemantic(true) {
    }

    bool isEmptyAction() {
        if (isSemantic) {
            SemanticActionType type = (SemanticActionType) typeStored;
            return type == SemanticActionType::GO_AROUND || type == SemanticActionType::WAIT_FOR_TOKENS || type == SemanticActionType::WAIT_FOR_ASYNC
                    || type == SemanticActionType::STOPPED;
        } else {
            NotificationActionType type = (NotificationActionType) typeStored;
            return type == NotificationActionType::WAIT_FOR_NOTIFICATION || type == NotificationActionType::WAIT_FOR_ASYNC;
        }
    }

    void performAction(AbstractOutStream<ZP> *out) {
        if (isSemantic) {
            performActionSemantic(out);
            ((SemanticParser<ZP>*) source)->actionPerformed((SemanticActionType) typeStored);
        } else {
            performActionNotification(out);
            ((ZscriptNotificationSource<ZP>*) source)->actionPerformed((NotificationActionType) typeStored);
        }
    }
    void performEmptyAction() {
        if (isSemantic) {
            ((SemanticParser<ZP>*) source)->actionPerformed((SemanticActionType) typeStored);
        } else {
            ((ZscriptNotificationSource<ZP>*) source)->actionPerformed((NotificationActionType) typeStored);
        }
    }

    bool canLock() {
        if (isSemantic) {
            return ((SemanticParser<ZP>*) source)->canLock();
        } else {
            return ((ZscriptNotificationSource<ZP>*) source)->canLock();
        }
    }

    bool lock() {
        if (isSemantic) {
            return ((SemanticParser<ZP>*) source)->lock();
        } else {
            return ((ZscriptNotificationSource<ZP>*) source)->lock();
        }
    }

    uint8_t getType() {
        return typeStored;
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_ */

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
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#include "ZscriptAddressingContext.hpp"
#endif
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
#include "../notifications/ZscriptNotificationSource.hpp"
#endif
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

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    INVOKE_ADDRESSING,
    ADDRESSING_MOVEALONG,
#endif
    RUN_FIRST_COMMAND,
    RUN_COMMAND,
    COMMAND_MOVEALONG,
    END_SEQUENCE,
    CLOSE_PAREN,
    STOPPED
};

template<class ZP>
class ZscriptAction {
private:
    void *source;
    uint8_t typeStored;

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    bool isSemantic;
#endif

    void performActionSemantic(AbstractOutStream<ZP> *out) {
        SemanticParser<ZP> *parseState = ((SemanticParser<ZP>*) source);
        ZscriptCommandContext<ZP> cmdCtx;
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        ZscriptAddressingContext<ZP> addrCtx;
#endif
        SemanticActionType type = (SemanticActionType) typeStored;
        bool isMoveAlong;
        if (typeStored == (uint8_t) SemanticActionType::ERROR || typeStored == (uint8_t) SemanticActionType::RUN_FIRST_COMMAND) {
            startResponseSem(out, (uint8_t) 0); //TODO: Sort out what response type...
        }
        switch (type) {
        case SemanticActionType::ERROR:
            out->writeField('S', parseState->getErrorStatus());
            // falls through
        case SemanticActionType::END_SEQUENCE:
            if (out->isOpen()) {
                out->endSequence();
                out->close();
            }
            parseState->unlock();
            break;
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        case SemanticActionType::INVOKE_ADDRESSING:
            case SemanticActionType::ADDRESSING_MOVEALONG:
            isMoveAlong = typeStored == (uint8_t) SemanticActionType::ADDRESSING_MOVEALONG;
            addrCtx = ZscriptAddressingContext<ZP>(parseState);
            if (isMoveAlong || addrCtx.verify()) {
                ModuleRegistry<ZP>::execute(addrCtx, isMoveAlong);
            }
            break;
#endif
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
            out->open(((SemanticParser<ZP>*) source)->getChannelIndex());
        }
        out->writeField('!', respType);
        if (((SemanticParser<ZP>*) source)->hasEcho()) {
            out->writeField('_', ((SemanticParser<ZP>*) source)->getEcho());
        }
    }
    void sendNormalMarkerPrefix(AbstractOutStream<ZP> *out) {
        uint8_t marker = ((SemanticParser<ZP>*) source)->takeCurrentMarker();
        if (marker != 0) {
            if (marker == ZscriptTokenizer < ZP > ::CMD_END_ANDTHEN) {
                out->writeAndThen();
            } else if (marker == ZscriptTokenizer < ZP > ::CMD_END_ORELSE) {
                out->writeOrElse();
            } else if (marker == ZscriptTokenizer < ZP > ::CMD_END_OPEN_PAREN) {
                out->writeOpenParen();
            } else if (marker == ZscriptTokenizer < ZP > ::CMD_END_CLOSE_PAREN) {
                out->writeCloseParen();
            } else if (marker == ZscriptTokenizer < ZP > ::NORMAL_SEQUENCE_END) {
            } else {
                //unreachable
            }
        }
    }
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
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
            out->open(Zscript<ZP>::zscript.getNotificationChannelIndex());
        }
        if (notifSrc->isAddressing()) {
            out->writeField('!', 0);
            //            commandStream.writeField(Zchars.Z_ADDRESSING, (source.getID() >> 4));
        } else {
            out->writeField('!', (notifSrc->getID() >> 4));
        }
    }
#endif

public:
    ZscriptAction(SemanticParser<ZP> *source, SemanticActionType type) :
            source(source), typeStored((uint8_t) type)

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    , isSemantic(true)
#endif
    {
    }
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    ZscriptAction(ZscriptNotificationSource<ZP> *source, NotificationActionType type) :
            source(source), typeStored((uint8_t) type), isSemantic(false) {
    }
#endif
    ZscriptAction() :
            source(NULL), typeStored(0)
    #ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    , isSemantic(true)
#endif
    {
    }

    bool isEmptyAction() {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        if (isSemantic) {
#endif
        SemanticActionType type = (SemanticActionType) typeStored;
        return type == SemanticActionType::GO_AROUND || type == SemanticActionType::WAIT_FOR_TOKENS || type == SemanticActionType::WAIT_FOR_ASYNC
                || type == SemanticActionType::STOPPED;
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        } else {
            NotificationActionType type = (NotificationActionType) typeStored;
            return type == NotificationActionType::WAIT_FOR_NOTIFICATION || type == NotificationActionType::WAIT_FOR_ASYNC;
        }
#endif
    }

    void performAction(AbstractOutStream<ZP> *out) {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        if (isSemantic) {
#endif
        performActionSemantic(out);
        ((SemanticParser<ZP>*) source)->actionPerformed((SemanticActionType) typeStored);
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
        } else {
            performActionNotification(out);
            ((ZscriptNotificationSource<ZP>*) source)->actionPerformed((NotificationActionType) typeStored);
        }
#endif
    }
    void performEmptyAction() {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            if (isSemantic) {
#endif
        ((SemanticParser<ZP>*) source)->emptyActionPerformed();
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            } else {
                ((ZscriptNotificationSource<ZP>*) source)->actionPerformed((NotificationActionType) typeStored);
            }
#endif
    }

    bool canLock() {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            if (isSemantic) {
#endif
        return ((SemanticParser<ZP>*) source)->canLock();
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            } else {
                return ((ZscriptNotificationSource<ZP>*) source)->canLock();
            }
#endif
    }

    bool lock() {
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            if (isSemantic) {
#endif
        return ((SemanticParser<ZP>*) source)->lock();
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
            } else {
                return ((ZscriptNotificationSource<ZP>*) source)->lock();
            }
#endif
    }

    uint8_t getType() {
        return typeStored;
    }
}
;
template<class ZP>
ZscriptAction<ZP> ZscriptNotificationSource<ZP>::getAction() {
    while (currentAction == NotificationActionType::INVALID) {
        currentAction = getActionType();
    }
    return ZscriptAction<ZP>(this, currentAction);
}
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_ */

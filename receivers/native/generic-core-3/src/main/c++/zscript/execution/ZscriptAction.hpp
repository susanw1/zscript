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

    void performActionSemantic(Zscript<ZP> *z, AbstractOutStream<ZP> *out) {
        SemanticParser<ZP> *parseState = ((SemanticParser<ZP>*) source);
        ZscriptCommandContext<ZP> cmdCtx;
        ZscriptAddressingContext<ZP> addrCtx;
        SemanticActionType type = (SemanticActionType) typeStored;
        switch (type) {
        case SemanticActionType::ERROR:
            startResponseSem(out, (uint8_t) 0x10);
            out->writeField('S', parseState->getErrorStatus());
            out->endSequence();
            out->close();
            parseState->unlock(z);
            break;
        case SemanticActionType::INVOKE_ADDRESSING:
            addrCtx = ZscriptAddressingContext<ZP>(parseState);
            if (addrCtx.verify()) {
                ModuleRegistry<ZP>::execute(addrCtx);
            }
            break;
        case SemanticActionType::ADDRESSING_MOVEALONG:
            addrCtx = ZscriptAddressingContext<ZP>(parseState);
            ModuleRegistry<ZP>::moveAlong(addrCtx);
            break;
        case SemanticActionType::RUN_FIRST_COMMAND:
            startResponseSem(out, (uint8_t) 0);
            // falls through
        case SemanticActionType::RUN_COMMAND:
            if (typeStored == (uint8_t) SemanticActionType::RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            cmdCtx = ZscriptCommandContext<ZP>(z, parseState, out);
            if (cmdCtx.verify()) {
                if (!parseState->isEmpty()) {
                    ModuleRegistry<ZP>::execute(cmdCtx);
                    if (cmdCtx.isCommandComplete()) {
                        cmdCtx.status(ResponseStatus::SUCCESS);
                    }
                }
            }
            break;
        case SemanticActionType::COMMAND_MOVEALONG:
            cmdCtx = ZscriptCommandContext<ZP>(z, parseState, out);
            ModuleRegistry<ZP>::moveAlong(cmdCtx);
            if (cmdCtx.isCommandComplete() && !parseState->hasSentStatus()) {
                cmdCtx.status(ResponseStatus::SUCCESS);
            }
            break;
        case SemanticActionType::END_SEQUENCE:
            if (out->isOpen()) {
                out->endSequence();
                out->close();
            }
            parseState->unlock(z);
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

    void performActionNotification(Zscript<ZP> *z, AbstractOutStream<ZP> *out) {
        ZscriptNotificationSource<ZP> *notifSrc = ((ZscriptNotificationSource<ZP>*) source);
        NotificationActionType type = (NotificationActionType) typeStored;
        switch (type) {
        case NotificationActionType::NOTIFICATION_BEGIN:
            startResponseNotif(out);
            ModuleRegistry<ZP>::notification(ZscriptNotificationContext<ZP>(notifSrc, z), false);
            break;
        case NotificationActionType::NOTIFICATION_END:
            out->endSequence();
            out->close();
            notifSrc->unlock(z);
            break;
        case NotificationActionType::NOTIFICATION_MOVE_ALONG:
            ModuleRegistry<ZP>::notification(ZscriptNotificationContext<ZP>(notifSrc, z), true);
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

    void performAction(Zscript<ZP> *z, AbstractOutStream<ZP> *out) {
        if (isSemantic) {
            performActionSemantic(z, out);
            ((SemanticParser<ZP>*) source)->actionPerformed((SemanticActionType) typeStored);
        } else {
            performActionNotification(z, out);
            ((ZscriptNotificationSource<ZP>*) source)->actionPerformed((NotificationActionType) typeStored);
        }
    }

    bool canLock(Zscript<ZP> *z) {
        if (isSemantic) {
            return ((SemanticParser<ZP>*) source)->canLock(z);
        } else {
            return ((ZscriptNotificationSource<ZP>*) source)->canLock(z);
        }
    }

    bool lock(Zscript<ZP> *z) {
        if (isSemantic) {
            return ((SemanticParser<ZP>*) source)->lock(z);
        } else {
            return ((ZscriptNotificationSource<ZP>*) source)->lock(z);
        }
    }

    uint8_t getType() {
        return typeStored;
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_ */

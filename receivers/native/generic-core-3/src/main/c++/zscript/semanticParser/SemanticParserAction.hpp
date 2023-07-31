/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_
#define SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_
#include "../ZscriptIncludes.hpp"
#include "../ZscriptAbstractOutStream.hpp"
#include "../TokenRingBuffer.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class SemanticParserAction {
private:
    SemanticParser<ZP> *parseState;
    ActionType type;

    void performActionImpl(Zscript<ZP> *z, AbstractOutStream<ZP> *out) {
        switch (type) {
        case ERROR:
            startResponse(out, (uint8_t) 0x10);
            out->asCommandOutStream().writeField('S', parseState->getErrorStatus());
            out->endSequence();
            out->close();
            parseState->unlock(z);
            break;
        case INVOKE_ADDRESSING:
            ZscriptAddressingContext<ZP> addrCtx(parseState);
            if (addrCtx.verify()) {
                z->getModuleRegistry()->execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
            ZscriptAddressingContext<ZP> addrCtx(parseState);
            z->getModuleRegistry()->moveAlong(addrCtx);
            break;
        case RUN_FIRST_COMMAND:
            startResponse(out, (uint8_t) 0);
            // fall though
        case RUN_COMMAND:
            if (type == ActionType::RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            ZscriptCommandContext<ZP> cmdCtx(z, parseState, out);
            if (cmdCtx.verify()) {
                z->getModuleRegistry()->execute(cmdCtx);
            }
            if (cmdCtx.isCommandComplete() && !parseState->hasSentStatus()) {
                cmdCtx.status(ZcodeResponseStatus::SUCCESS);
            }
            break;
        case COMMAND_MOVEALONG:
            ZscriptCommandContext<ZP> cmdCtx1(z, parseState, out);
            z->getModuleRegistry()->moveAlong(cmdCtx1);
            if (cmdCtx1.isCommandComplete() && !parseState->hasSentStatus()) {
                cmdCtx1.status(ZcodeResponseStatus::SUCCESS);
            }
            break;
        case END_SEQUENCE:
            if (out->isOpen()) {
                out->endSequence();
                out->close();
            }
            parseState->unlock(z);
            break;
        case CLOSE_PAREN:
            out->writeCloseParen();
            break;
        case GO_AROUND:
            case WAIT_FOR_TOKENS:
            case WAIT_FOR_ASYNC:
            break;
        case INVALID:
            //Unreachable
        }
    }
    void startResponse(AbstractOutStream<ZP> *out, uint8_t respType) {
        if (!out->isOpen()) {
            out->open();
        }
        out->writeField('!', respType);
        if (parseState->hasEcho()) {
            out->writeField('_', parseState->getEcho());
        }
    }
    void sendNormalMarkerPrefix(AbstractOutStream<ZP> *out) {
        uint8_t marker = parseState->takeCurrentMarker();
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

public:
    SemanticParserAction(ActionType type, SemanticParser<ZP> *parseState) :
            type(type), parseState(parseState) {
    }

    bool isEmptyAction() {
        return type == ActionType::GO_AROUND || type == ActionType::WAIT_FOR_TOKENS || type == ActionType::WAIT_FOR_ASYNC;
    }

    void performAction(Zscript<ZP> *z, AbstractOutStream<ZP> *out) {
        performActionImpl(z, out);
        parseState->actionPerformed(type);
    }

    bool canLock(Zscript<ZP> *z) {
        return parseState->canLock(z);
    }

    bool lock(Zscript<ZP> *z) {
        return parseState->lock(z);
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SEMANTICPARSER_SEMANTICPARSERACTION_HPP_ */

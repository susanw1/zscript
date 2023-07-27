package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeAddressingSystem;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeAction {
    enum ActionType {
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
        CLOSE_PAREN
    }

    private final ParseState parseState;
    private final ActionType type;

    ZcodeAction(ActionType type, ParseState parser) {
        this.parseState = parser;
        this.type = type;
    }

    public boolean isEmptyAction() {
        return type == ActionType.GO_AROUND || type == ActionType.WAIT_FOR_TOKENS || type == ActionType.WAIT_FOR_ASYNC;
    }

    public void performAction(Zcode zcode, ZcodeOutStream out) {
        performActionImpl(zcode, out);
        parseState.actionPerformed(type);
    }

    // TODO: response type
    // TODO: close out stream
    protected void performActionImpl(Zcode zcode, ZcodeOutStream out) {
        switch (type) {
        case ERROR:
            startResponse(out, (byte) 0x10);
            out.writeField('S', parseState.getErrorStatus());
            out.endSequence();
            out.close();
            parseState.unlock(zcode);
            break;
        case INVOKE_ADDRESSING:
            ZcodeAddressingContext addrCtx = new ZcodeAddressingContext((ContextView) parseState);
            if (addrCtx.verify()) {
                ZcodeAddressingSystem.execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
            ZcodeAddressingSystem.moveAlong(new ZcodeAddressingContext((ContextView) parseState));
            break;
        case RUN_FIRST_COMMAND:
            startResponse(out, (byte) 0);
            // fall though
        case RUN_COMMAND:
            if (type == ActionType.RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            ZcodeCommandContext cmdCtx = new ZcodeCommandContext((ContextView) parseState, out);
            if (cmdCtx.verify()) {
                ZcodeCommandFinder.execute(cmdCtx);
            }
            if (cmdCtx.isCommandComplete() && !parseState.hasSentStatus()) {
                cmdCtx.status(ZcodeStatus.SUCCESS);
            }
            break;
        case COMMAND_MOVEALONG:
            ZcodeCommandContext cmdCtx1 = new ZcodeCommandContext((ContextView) parseState, out);
            ZcodeCommandFinder.moveAlong(cmdCtx1);
            if (cmdCtx1.isCommandComplete() && !parseState.hasSentStatus()) {
                cmdCtx1.status(ZcodeStatus.SUCCESS);
            }
            break;
        case END_SEQUENCE:
            if (out.isOpen()) {
                out.endSequence();
                out.close();
            }
            parseState.unlock(zcode);
            break;
        case CLOSE_PAREN:
            out.writeCloseParen();
            break;
        case GO_AROUND:
        case WAIT_FOR_TOKENS:
        case WAIT_FOR_ASYNC:
            break;
        case INVALID:
            throw new IllegalStateException();
        }
    }

    private void startResponse(ZcodeOutStream out, byte respType) {
        if (!out.isOpen()) {
            out.open();
        }
        out.writeField('!', respType);
        if (parseState.hasEcho()) {
            out.writeField('_', parseState.getEcho());
        }
    }

    private void sendNormalMarkerPrefix(ZcodeOutStream out) {
        byte marker = parseState.takeCurrentMarker();
        if (marker != 0) {
            if (marker == ZcodeTokenizer.CMD_END_ANDTHEN) {
                out.writeAndThen();
            } else if (marker == ZcodeTokenizer.CMD_END_ORELSE) {
                out.writeOrElse();
            } else if (marker == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                out.writeOpenParen();
            } else if (marker == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                out.writeCloseParen();
            } else if (marker == ZcodeTokenizer.NORMAL_SEQUENCE_END) {
            } else {
                throw new IllegalStateException("Unknown marker");
            }
        }
    }

    @Override
    public String toString() {
        return "ActionType(" + type + ")";
    }

    public boolean canLock(Zcode zcode) {
        return parseState.canLock(zcode);
    }

    public boolean lock(Zcode zcode) {
        return parseState.lock(zcode);
    }

    // visible for testing
    ActionType getType() {
        return type;
    }

    // visible for testing
//    byte getInfo() {
//        return info;
//    }
}

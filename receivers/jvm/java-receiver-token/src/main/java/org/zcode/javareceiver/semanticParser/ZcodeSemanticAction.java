package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeCommandOutStream;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAction;
import org.zcode.javareceiver.execution.ZcodeAddressingContext;
import org.zcode.javareceiver.execution.ZcodeCommandContext;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeSemanticAction implements ZcodeAction {
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

    ZcodeSemanticAction(ActionType type, ParseState parser) {
        this.parseState = parser;
        this.type = type;
    }

    @Override
    public boolean isEmptyAction() {
        return type == ActionType.GO_AROUND || type == ActionType.WAIT_FOR_TOKENS || type == ActionType.WAIT_FOR_ASYNC;
    }

    @Override
    public void performAction(Zcode zcode, ZcodeOutStream out) {
        performActionImpl(zcode, out);
        parseState.actionPerformed(type);
    }

    protected void performActionImpl(Zcode zcode, ZcodeOutStream out) {
        switch (type) {
        case ERROR:
            startResponse(out, (byte) 0x10); // TODO: debate
            out.asCommandOutStream().writeField('S', parseState.getErrorStatus());
            out.endSequence();
            out.close();
            parseState.unlock(zcode);
            break;
        case INVOKE_ADDRESSING:
            ZcodeAddressingContext addrCtx = new ZcodeAddressingContext((ContextView) parseState);
            if (addrCtx.verify()) {
                zcode.getModuleRegistry().execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
            zcode.getModuleRegistry().moveAlong(new ZcodeAddressingContext((ContextView) parseState));
            break;
        case RUN_FIRST_COMMAND:
            startResponse(out, (byte) 0);
            // fall though
        case RUN_COMMAND:
            if (type == ActionType.RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            ZcodeCommandContext cmdCtx = new ZcodeCommandContext(zcode, (ContextView) parseState, out);
            if (cmdCtx.verify()) {
                zcode.getModuleRegistry().execute(cmdCtx);
            }
            if (cmdCtx.isCommandComplete() && !parseState.hasSentStatus()) {
                cmdCtx.status(ZcodeStatus.SUCCESS);
            }
            break;
        case COMMAND_MOVEALONG:
            ZcodeCommandContext cmdCtx1 = new ZcodeCommandContext(zcode, (ContextView) parseState, out);
            zcode.getModuleRegistry().moveAlong(cmdCtx1);
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
        ZcodeCommandOutStream commandOutput = out.asCommandOutStream();
        commandOutput.writeField('!', respType);
        if (parseState.hasEcho()) {
            commandOutput.writeField('_', parseState.getEcho());
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

    @Override
    public boolean canLock(Zcode zcode) {
        return parseState.canLock(zcode);
    }

    @Override
    public boolean lock(Zcode zcode) {
        return parseState.lock(zcode);
    }

    // visible for testing
    ActionType getType() {
        return type;
    }
}

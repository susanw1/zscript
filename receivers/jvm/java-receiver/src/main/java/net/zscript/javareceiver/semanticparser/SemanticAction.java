package net.zscript.javareceiver.semanticparser;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.core.CommandOutStream;
import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.execution.AddressingContext;
import net.zscript.javareceiver.execution.CommandContext;
import net.zscript.javareceiver.execution.ZscriptAction;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.tokenizer.Tokenizer;

public class SemanticAction implements ZscriptAction {
    public enum ActionType {
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
    }

    private final ParseState parseState;
    private final ActionType type;

    SemanticAction(ActionType type, ParseState parser) {
        this.parseState = parser;
        this.type = type;
    }

    @Override
    public boolean isEmptyAction() {
        return type == ActionType.GO_AROUND || type == ActionType.WAIT_FOR_TOKENS || type == ActionType.WAIT_FOR_ASYNC || type == ActionType.STOPPED;
    }

    @Override
    public void performAction(Zscript zscript, SequenceOutStream out) {
        performActionImpl(zscript, out);
        parseState.actionPerformed(type);
    }

    protected void performActionImpl(Zscript zscript, SequenceOutStream out) {
        switch (type) {
        case ERROR:
            startResponse(out, (byte) 0); // TODO: debate
            out.asCommandOutStream().writeField(Zchars.Z_STATUS, parseState.getErrorStatus());
            out.endSequence();
            out.close();
            parseState.unlock(zscript);
            break;
        case INVOKE_ADDRESSING:
            AddressingContext addrCtx = new AddressingContext(zscript, (ContextView) parseState);
            if (addrCtx.verify()) {
                zscript.getModuleRegistry().execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
            zscript.getModuleRegistry().moveAlong(new AddressingContext(zscript, (ContextView) parseState));
            break;
        case RUN_FIRST_COMMAND:
            startResponse(out, (byte) 0);
            // fall though
        case RUN_COMMAND:
            if (type == ActionType.RUN_COMMAND) {
                sendNormalMarkerPrefix(out);
            }
            CommandContext cmdCtx = new CommandContext(zscript, (ContextView) parseState, out);
            if (cmdCtx.verify()) {
                if (!parseState.isEmpty()) {
                    zscript.getModuleRegistry().execute(cmdCtx);
                    if (cmdCtx.isCommandComplete()) {
                        cmdCtx.status(ZscriptStatus.SUCCESS);
                    }
                }
            }
            break;
        case COMMAND_MOVEALONG:
            CommandContext cmdCtx1 = new CommandContext(zscript, (ContextView) parseState, out);
            zscript.getModuleRegistry().moveAlong(cmdCtx1);
            if (cmdCtx1.isCommandComplete()) {
                cmdCtx1.status(ZscriptStatus.SUCCESS);
            }
            break;
        case END_SEQUENCE:
            if (out.isOpen()) {
                out.endSequence();
                out.close();
            }
            parseState.unlock(zscript);
            break;
        case CLOSE_PAREN:
            out.writeCloseParen();
            break;
        case GO_AROUND:
        case WAIT_FOR_TOKENS:
        case WAIT_FOR_ASYNC:
        case STOPPED:
            break;
        case INVALID:
            throw new IllegalStateException();
        }
    }

    private void startResponse(SequenceOutStream out, byte respType) {
        if (!out.isOpen()) {
            out.open();
        }
        CommandOutStream commandOutput = out.asCommandOutStream();
        commandOutput.writeField(Zchars.Z_RESPONSE_MARK, respType);
        if (parseState.hasEcho()) {
            commandOutput.writeField(Zchars.Z_ECHO, parseState.getEcho());
        }
    }

    private void sendNormalMarkerPrefix(SequenceOutStream out) {
        byte marker = parseState.takeCurrentMarker();
        if (marker != 0) {
            if (marker == Tokenizer.CMD_END_ANDTHEN) {
                out.writeAndThen();
            } else if (marker == Tokenizer.CMD_END_ORELSE) {
                out.writeOrElse();
            } else if (marker == Tokenizer.CMD_END_OPEN_PAREN) {
                out.writeOpenParen();
            } else if (marker == Tokenizer.CMD_END_CLOSE_PAREN) {
                out.writeCloseParen();
            } else if (marker == Tokenizer.NORMAL_SEQUENCE_END) {
                // no action required
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
    public boolean canLock(Zscript zscript) {
        return parseState.canLock(zscript);
    }

    @Override
    public boolean lock(Zscript zscript) {
        return parseState.lock(zscript);
    }

    // visible for testing
    @Nonnull
    ActionType getType() {
        return type;
    }
}

package org.zcode.javareceiver.semanticParser;

import java.util.function.Consumer;

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
        NO_ACTION,
        NEEDS_TOKENS,
        ERROR,
        ADDRESSING,
        ADDRESSING_MOVEALONG,
        FIRST_COMMAND,
        COMMAND,
        COMMAND_MOVEALONG,
        END_SEQUENCE,
        CLOSE_PAREN
    }

    private final ActionType           type;
    private final ParseState           parseState;
    private final byte                 info;
    private final Consumer<ParseState> before;
    private final Consumer<ParseState> after;

    ZcodeAction(ActionType type, ParseState parser, byte info) {
        this(type, parser, info, null, null);
    }

    ZcodeAction(ActionType type, ParseState parser, byte info, Consumer<ParseState> before, Consumer<ParseState> after) {
        this.parseState = parser;
        this.type = type;
        this.info = info;
        this.before = before;
        this.after = after;
    }

    public boolean needsPerforming() {
        return type != ActionType.NO_ACTION && type != ActionType.NEEDS_TOKENS;
    }

    protected void performAction(Zcode zcode, ZcodeOutStream out) {
        if (before != null) {
            before.accept(parseState);
        }
        performActionImpl(zcode, out);
        if (after != null) {
            after.accept(parseState);
        }
    }

    // TODO: response type
    // TODO: close out stream
    protected void performActionImpl(Zcode zcode, ZcodeOutStream out) {
        switch (type) {
        case ERROR:
            sendError(out, info, parseState.getSeqEndMarker());
            parseState.errorSent();
            break;
        case ADDRESSING:
//            parseState.setCommandStarted();
            ZcodeAddressingContext addrCtx = new ZcodeAddressingContext((ContextView) parseState);
            if (addrCtx.verify()) {
                ZcodeAddressingSystem.execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
//            parseState.clearNeedsAction();
            ZcodeAddressingSystem.moveAlong(new ZcodeAddressingContext((ContextView) parseState));
            break;
        case FIRST_COMMAND:
//            parseState.clearFirstCommand();
            startResponse(out);
            // fall though
        case COMMAND:
//            parseState.setCommandStarted(); /// moved to "before"
            sendNormalMarkerPrefix(out, info);
            ZcodeCommandContext cmdCtx = new ZcodeCommandContext((ContextView) parseState, out);
            if (cmdCtx.verify()) {
                ZcodeCommandFinder.execute(cmdCtx);
//                if (cmdCtx.isComplete()) {
//                    if (!cmdCtx.statusGiven()) {
//                        cmdCtx.setStatus(ZcodeStatus.SUCCESS);
//                    } else if (cmdCtx.statusError()) {
//                        out.endSequence();
//                        out.close();
//                    }
//                }
            }
            break;
        case COMMAND_MOVEALONG:
//            parseState.clearNeedsAction();
            ZcodeCommandContext cmdCtx1 = new ZcodeCommandContext((ContextView) parseState, out);
            ZcodeCommandFinder.moveAlong(cmdCtx1);
//            if (cmdCtx1.isComplete()) {
//                if (!cmdCtx1.statusGiven()) {
//                    cmdCtx1.setStatus(ZcodeStatus.SUCCESS);
//                } else if (cmdCtx1.statusError()) {
//                    out.endSequence();
//                    out.close();
//                }
//            }
            break;
        case END_SEQUENCE:
            out.endSequence();
            out.close();
//            parseState.unlock(zcode);
//            parseState.seqEndSent();
            break;
        case CLOSE_PAREN:
            out.writeCloseParen();
//            parseState.closeParenSent();
            break;
        case NO_ACTION:
        case NEEDS_TOKENS:
            break;
        }
    }

    private void startResponse(ZcodeOutStream out) {
        out.open();
        out.writeField('!', 0);
        if (parseState.hasEcho()) {
            out.writeField('_', parseState.getEcho());
        }
    }

    /**
     * Outputs an 'S' status field for an <i>error</i> (not for command failure). Value is based either just on 'semanticParserError', or if SemanticParser.MARKER_ERROR, then uses
     * 'tokenizerError'.
     *
     * @param out                 where to write output
     * @param semanticParserError decides what type of status code to write
     * @param tokenizerError      decides on status code when semanticParserError == SemanticParser.MARKER_ERROR
     */
    private static void sendError(ZcodeOutStream out, byte semanticParserError, byte tokenizerError) {
        if (!out.isOpen()) {
            out.open();
        }

//        errorSent();

        byte statusValue = mapErrorToStatus(semanticParserError, tokenizerError);

        out.writeField('S', statusValue);

        out.endSequence();
        out.close();
    }

    private static byte mapErrorToStatus(byte semanticParserError, byte tokenizerError) {
        byte statusValue;
        switch (semanticParserError) {
        case SemanticParser.NO_ERROR:
            throw new IllegalStateException();
        case SemanticParser.INTERNAL_ERROR:
            statusValue = ZcodeStatus.INTERNAL_ERROR;
            break;
        case SemanticParser.MARKER_ERROR:
            if (tokenizerError == ZcodeTokenizer.ERROR_BUFFER_OVERRUN) {
                statusValue = ZcodeStatus.BUFFER_OVR_ERROR;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_FIELD_TOO_LONG) {
                statusValue = ZcodeStatus.FIELD_TOO_LONG;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_ILLEGAL_TOKEN) {
                statusValue = ZcodeStatus.ILLEGAL_KEY;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH) {
                statusValue = ZcodeStatus.ODD_LENGTH;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_STRING_ESCAPING) {
                statusValue = ZcodeStatus.ESCAPING_ERROR;
            } else if (tokenizerError == ZcodeTokenizer.ERROR_CODE_STRING_NOT_TERMINATED) {
                statusValue = ZcodeStatus.UNTERMINATED_STRING;
            } else {
                statusValue = (ZcodeStatus.INTERNAL_ERROR);
            }
            break;
        case SemanticParser.COMMENT_WITH_SEQ_FIELDS_ERROR:
            statusValue = ZcodeStatus.INVALID_COMMENT;
            break;
        case SemanticParser.LOCKS_ERROR:
            statusValue = ZcodeStatus.INVALID_LOCKS;
            break;
        case SemanticParser.MULTIPLE_ECHO_ERROR:
            statusValue = ZcodeStatus.INVALID_ECHO;
            break;
        default:
            statusValue = ZcodeStatus.INTERNAL_ERROR;
            break;
        }
        return statusValue;
    }

    private static void sendNormalMarkerPrefix(ZcodeOutStream out, byte info) {
        if (info != 0) {
            if (info == ZcodeTokenizer.CMD_END_ANDTHEN) {
                out.writeAndThen();
            } else if (info == ZcodeTokenizer.CMD_END_ORELSE) {
                out.writeOrElse();
            } else if (info == ZcodeTokenizer.CMD_END_OPEN_PAREN) {
                out.writeOpenParen();
            } else if (info == ZcodeTokenizer.CMD_END_CLOSE_PAREN) {
                out.writeCloseParen();
            } else if (info == ZcodeTokenizer.NORMAL_SEQUENCE_END) {
            } else {
                throw new IllegalStateException("Unknown marker");
            }
        }
    }

    @Override
    public String toString() {
        return "ActionType(" + type + ", info=" + Byte.toUnsignedInt(info) + ")";
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
    byte getInfo() {
        return info;
    }
}

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

    private final ParseState parser;
    private final ActionType type;
    private final byte       info;

    public static ZcodeAction noAction(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NO_ACTION, semanticParser, (byte) 0);
    }

    public static ZcodeAction needsTokens(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NEEDS_TOKENS, semanticParser, (byte) 0);
    }

    public static ZcodeAction error(SemanticParser semanticParser, byte error) {
        return new ZcodeAction(ActionType.ERROR, semanticParser, error);
    }

    public static ZcodeAction addressing(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING, semanticParser, (byte) 0);
    }

    public static ZcodeAction addressingMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, semanticParser, (byte) 0);
    }

    public static ZcodeAction commandMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, semanticParser, (byte) 0);
    }

    public static ZcodeAction firstCommand(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.FIRST_COMMAND, semanticParser, (byte) 0);
    }

    public static ZcodeAction runCommand(SemanticParser semanticParser, byte info) {
        return new ZcodeAction(ActionType.COMMAND, semanticParser, info);
    }

    public static ZcodeAction endSequence(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.END_SEQUENCE, semanticParser, (byte) 0);
    }

    public static ZcodeAction closeParen(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.CLOSE_PAREN, semanticParser, (byte) 0);
    }

    private ZcodeAction(ActionType type, ParseState parser, byte info) {
        this.parser = parser;
        this.type = type;
        this.info = info;
    }

    public boolean needsPerforming() {
        return type != ActionType.NO_ACTION && type != ActionType.NEEDS_TOKENS;
    }

    // TODO: response type
    // TODO: close out stream
    public void performAction(Zcode zcode, ZcodeOutStream out) {
        switch (type) {
        case ERROR:
            sendError(out);
            parser.errorSent();
            break;
        case ADDRESSING:
            parser.setStarted();
            ZcodeAddressingContext addrCtx = new ZcodeAddressingContext(parser, out);
            if (addrCtx.verify()) {
                ZcodeAddressingSystem.execute(addrCtx);
            }
            break;
        case ADDRESSING_MOVEALONG:
            parser.clearNeedsAction();
            ZcodeAddressingSystem.moveAlong(new ZcodeAddressingContext(parser, out));
            break;
        case FIRST_COMMAND:
            out.open();
            out.writeField('!', 0);
            if (parser.hasEcho()) {
                out.writeField('_', parser.getEcho());
            }
            parser.clearFirstCommand();
            // fall though
        case COMMAND:
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
            parser.setStarted();
            ZcodeCommandContext cmdCtx = new ZcodeCommandContext(parser, out);
            if (cmdCtx.verify()) {
                ZcodeCommandFinder.execute(cmdCtx);
                if (cmdCtx.isComplete()) {
                    if (!cmdCtx.statusGiven()) {
                        cmdCtx.status(ZcodeStatus.SUCCESS);
                    } else if (cmdCtx.statusError()) {
                        out.endSequence();
                        out.close();
                    }
                }
            }
            break;
        case COMMAND_MOVEALONG:
            parser.clearNeedsAction();
            ZcodeCommandContext cmdCtx1 = new ZcodeCommandContext(parser, out);
            ZcodeCommandFinder.moveAlong(cmdCtx1);
            if (cmdCtx1.isComplete()) {
                if (!cmdCtx1.statusGiven()) {
                    cmdCtx1.status(ZcodeStatus.SUCCESS);
                } else if (cmdCtx1.statusError()) {
                    out.endSequence();
                    out.close();
                }
            }
            break;
        case END_SEQUENCE:
            parser.seqEndSent();
            out.endSequence();
            out.close();
            zcode.unlock(parser.getLocks());
            parser.setLocked(false);
            break;
        case CLOSE_PAREN:
            parser.closeParenSent();
            out.writeCloseParen();
            break;
        case NO_ACTION:
        case NEEDS_TOKENS:
            break;
        }
    }

    private void sendError(ZcodeOutStream out) {
        if (!out.isOpen()) {
            out.open();
        }

        parser.errorSent();

        switch (info) {
        case SemanticParser.NO_ERROR:
            throw new IllegalStateException();
        case SemanticParser.INTERNAL_ERROR:
            out.writeField('S', ZcodeStatus.INTERNAL_ERROR);
            break;
        case SemanticParser.MARKER_ERROR:
            if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_BUFFER_OVERRUN) {
                out.writeField('S', ZcodeStatus.BUFFER_OVR_ERROR);
            } else if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_CODE_FIELD_TOO_LONG) {
                out.writeField('S', ZcodeStatus.FIELD_TOO_LONG);
            } else if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_CODE_ILLEGAL_TOKEN) {
                out.writeField('S', ZcodeStatus.ILLEGAL_KEY);
            } else if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_CODE_ODD_BIGFIELD_LENGTH) {
                out.writeField('S', ZcodeStatus.ODD_LENGTH);
            } else if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_CODE_STRING_ESCAPING) {
                out.writeField('S', ZcodeStatus.ESCAPING_ERROR);
            } else if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_CODE_STRING_NOT_TERMINATED) {
                out.writeField('S', ZcodeStatus.UNTERMINATED_STRING);
            } else {
                out.writeField('S', ZcodeStatus.INTERNAL_ERROR);
            }
            break;
        case SemanticParser.COMMENT_WITH_STUFF_ERROR:
            out.writeField('S', ZcodeStatus.INVALID_COMMENT);
            break;
        case SemanticParser.LOCKS_ERROR:
            out.writeField('S', ZcodeStatus.INVALID_LOCKS);
            break;
        case SemanticParser.MULTIPLE_ECHO_ERROR:
            out.writeField('S', ZcodeStatus.INVALID_ECHO);
            break;
        default:
            out.writeField('S', ZcodeStatus.INTERNAL_ERROR);
            break;
        }

        out.endSequence();
        out.close();
    }

    @Override
    public String toString() {
        return "ActionType(" + type + ", info=" + Byte.toUnsignedInt(info) + ")";
    }

    public boolean canLock(Zcode zcode) {
        return parser.isLocked() || zcode.canLock(parser.getLocks());
    }

    public boolean isLocked() {
        return parser.isLocked();
    }

    public boolean lock(Zcode zcode) {
        if (parser.isLocked()) {
            return true;
        }
        boolean l = zcode.lock(parser.getLocks());
        parser.setLocked(l);
        return l;
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

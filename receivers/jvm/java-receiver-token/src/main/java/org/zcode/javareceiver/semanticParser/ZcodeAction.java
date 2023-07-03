package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.addressing.ZcodeAddressingSystem;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingView;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.modules.ZcodeCommandFinder;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeAction {
    private enum ActionType {
        NO_ACTION,
        ERROR,
        ADDRESSING,
        ADDRESSING_MOVEALONG,
        FIRST_COMMAND,
        COMMAND,
        COMMAND_MOVEALONG,
        END_SEQUENCE
    }

    private final SemanticParser parser;
    private final ActionType     type;
    private final int            info;

    public static ZcodeAction noAction(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NO_ACTION, semanticParser, 0);
    }

    public static ZcodeAction error(SemanticParser semanticParser, int error) {
        return new ZcodeAction(ActionType.ERROR, semanticParser, error);
    }

    public static ZcodeAction addressing(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING, semanticParser, 0);
    }

    public static ZcodeAction addressingMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, semanticParser, 0);
    }

    public static ZcodeAction commandMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, semanticParser, 0);
    }

    public static ZcodeAction firstCommand(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.FIRST_COMMAND, semanticParser, 0);
    }

    public static ZcodeAction runCommand(SemanticParser semanticParser, byte info) {
        return new ZcodeAction(ActionType.COMMAND, semanticParser, info);
    }

    public static ZcodeAction endSequence(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.END_SEQUENCE, semanticParser, 0);
    }

    private ZcodeAction(ActionType type, SemanticParser parser, int info) {
        this.parser = parser;
        this.type = type;
        this.info = info;
    }

    public boolean needsPerforming() {
        return type != ActionType.NO_ACTION;
    }

    public ZcodeLockSet getLocks() {
        return parser.getLocks();
    }

    public void performAction(ZcodeOutStream out) {
        switch (type) {
        case ERROR:
            sendError(out);
            break;
        case ADDRESSING:
            parser.setStarted();
            ZcodeAddressingView view = new ZcodeAddressingView(parser, out);
            if (view.verify()) {
                ZcodeAddressingSystem.execute(view);
            }
            break;
        case ADDRESSING_MOVEALONG:
            ZcodeAddressingSystem.moveAlong(new ZcodeAddressingView(parser, out));
            break;
        case FIRST_COMMAND:
            out.open();
            if (parser.hasEcho()) {
                out.writeField('_', parser.getEcho());
            }
            parser.clearFirstCommand();
        case COMMAND:
            if (info != 0) {
                if (info == ZcodeTokenizer.CMD_END_ANDTHEN) {
                    out.writeAndThen();
                } else if (info == ZcodeTokenizer.CMD_END_ORELSE) {
                    out.writeOrElse();
                } else if (info == ZcodeTokenizer.NORMAL_SEQUENCE_END) {
                } else {
                    throw new IllegalStateException();
                }
            }
            parser.setStarted();
            ZcodeCommandView cmdview = new ZcodeCommandView(parser, out);
            if (cmdview.verify()) {
                ZcodeCommandFinder.execute(cmdview);
            }
            break;
        case COMMAND_MOVEALONG:
            ZcodeCommandFinder.moveAlong(new ZcodeCommandView(parser, out));
            break;
        case END_SEQUENCE:
            parser.seqEndSent();
            out.endSequence();
        case NO_ACTION:
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
            } else {
                out.writeField('S', ZcodeStatus.SYNTAX_ERROR);
                out.writeField('T', parser.getSeqEndMarker() & 0xf);
            }
            break;
        default:
            out.writeField('S', ZcodeStatus.SEQUENCE_ERROR);
            break;
        }
        out.endSequence();
        out.close();
    }

    @Override
    public String toString() {
        return type.name();
    }
}

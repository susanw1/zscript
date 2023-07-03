package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.core.ZcodeStatus;
import org.zcode.javareceiver.execution.ZcodeAddressingView;
import org.zcode.javareceiver.execution.ZcodeCommandView;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeAction {
    private enum ActionType {
        NO_ACTION,
        ERROR,
        ADDRESSING,
        ADDRESSING_MOVEALONG,
        FIRST_COMMAND,
        COMMAND,
        COMMAND_MOVEALONG
    }

    private final SemanticParser parser;
    private final ActionType     type;
    private final int            errorType;

    public static ZcodeAction noAction(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NO_ACTION, semanticParser, SemanticParser.NO_ERROR);
    }

    public static ZcodeAction error(SemanticParser semanticParser, int error) {
        return new ZcodeAction(ActionType.ERROR, semanticParser, error);
    }

    public static ZcodeAction addressing(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING, semanticParser, SemanticParser.NO_ERROR);
    }

    public static ZcodeAction addressingMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, semanticParser, SemanticParser.NO_ERROR);
    }

    public static ZcodeAction commandMoveAlong(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, semanticParser, SemanticParser.NO_ERROR);
    }

    public static ZcodeAction firstCommand(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.FIRST_COMMAND, semanticParser, SemanticParser.NO_ERROR);
    }

    public static ZcodeAction runCommand(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.COMMAND, semanticParser, SemanticParser.NO_ERROR);
    }

    private ZcodeAction(ActionType type, SemanticParser parser, int errorType) {
        this.parser = parser;
        this.type = type;
        this.errorType = errorType;
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
            ZcodeAddressingView view = new ZcodeAddressingView(parser, out);
            if (view.verify()) {
                view.execute();
            }
            break;
        case ADDRESSING_MOVEALONG:
            ZcodeAddressingView view2 = new ZcodeAddressingView(parser, out);
            view2.moveAlong();
            break;
        case FIRST_COMMAND:
            out.open();
            if (parser.hasEcho()) {
                out.writeField('_', parser.getEcho());
            }
        case COMMAND:
            ZcodeCommandView cmdview = new ZcodeCommandView(parser, out);
            if (cmdview.verify()) {
                cmdview.execute();
            }
            break;
        case COMMAND_MOVEALONG:
            ZcodeCommandView cmdview2 = new ZcodeCommandView(parser, out);
            cmdview2.moveAlong();
            break;
        case NO_ACTION:
            break;
        }
    }

    private void sendError(ZcodeOutStream out) {
        if (!out.isOpen()) {
            out.open();
        }
        switch (errorType) {
        case SemanticParser.NO_ERROR:
            throw new IllegalStateException();
        case SemanticParser.INTERNAL_ERROR:
            out.writeStatus(ZcodeStatus.INTERNAL_ERROR);
            break;
        case SemanticParser.MARKER_ERROR:
            if (parser.getSeqEndMarker() == ZcodeTokenizer.ERROR_BUFFER_OVERRUN) {
                out.writeStatus(ZcodeStatus.BUFFER_OVR_ERROR);
            } else {
                out.writeStatus(ZcodeStatus.SYNTAX_ERROR);
                out.writeField('T', parser.getSeqEndMarker() & 0xf);
            }
            break;
        default:
            out.writeStatus(ZcodeStatus.SEQUENCE_ERROR);
            break;
        }
        out.endSequence();
        out.close();
    }

}

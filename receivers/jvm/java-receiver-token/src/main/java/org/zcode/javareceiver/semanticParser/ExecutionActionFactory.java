package org.zcode.javareceiver.semanticParser;

import java.util.function.Consumer;

import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

public class ExecutionActionFactory implements ZcodeActionFactory {
    @Override
    public ZcodeAction noAction(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NO_ACTION, semanticParser, (byte) 0);
    }

    @Override
    public ZcodeAction needsTokens(SemanticParser semanticParser) {
        return new ZcodeAction(ActionType.NEEDS_TOKENS, semanticParser, (byte) 0);
    }

    @Override
    public ZcodeAction error(SemanticParser semanticParser, byte error) {
        return new ZcodeAction(ActionType.ERROR, semanticParser, error);
    }

    @Override
    public ZcodeAction addressing(SemanticParser semanticParser, Consumer<ParseState> before, Consumer<ParseState> after) {
        return new ZcodeAction(ActionType.ADDRESSING, semanticParser, (byte) 0, before, after);
    }

    @Override
    public ZcodeAction addressingMoveAlong(SemanticParser semanticParser, Consumer<ParseState> before) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, semanticParser, (byte) 0, before, null);
    }

    @Override
    public ZcodeAction commandMoveAlong(SemanticParser semanticParser, Consumer<ParseState> before) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, semanticParser, (byte) 0, before, null);
    }

    @Override
    public ZcodeAction firstCommand(SemanticParser semanticParser, Consumer<ParseState> before) {
        return new ZcodeAction(ActionType.FIRST_COMMAND, semanticParser, (byte) 0, before, null);
    }

    @Override
    public ZcodeAction runCommand(SemanticParser semanticParser, byte prefixMarker, Consumer<ParseState> before, Consumer<ParseState> after) {
        return new ZcodeAction(ActionType.COMMAND, semanticParser, prefixMarker, before, after);
    }

    @Override
    public ZcodeAction endSequence(SemanticParser semanticParser, Consumer<ParseState> after) {
        return new ZcodeAction(ActionType.END_SEQUENCE, semanticParser, (byte) 0, null, after);
    }

    @Override
    public ZcodeAction closeParen(SemanticParser semanticParser, Consumer<ParseState> after) {
        return new ZcodeAction(ActionType.CLOSE_PAREN, semanticParser, (byte) 0, null, after);
    }

}

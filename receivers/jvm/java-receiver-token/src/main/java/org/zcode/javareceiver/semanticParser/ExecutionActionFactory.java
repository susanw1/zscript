package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

public class ExecutionActionFactory implements ZcodeActionFactory {
    @Override
    public ZcodeAction goAround(ParseState parseState) {
        return new ZcodeAction(ActionType.GO_AROUND, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction waitForAsync(ParseState parseState) {
        return new ZcodeAction(ActionType.WAIT_FOR_ASYNC, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction waitForTokens(ParseState parseState) {
        return new ZcodeAction(ActionType.WAIT_FOR_TOKENS, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction error(ParseState parseState, byte error) {
        return new ZcodeAction(ActionType.ERROR, parseState, error);
    }

    @Override
    public ZcodeAction addressing(ParseState parseState) {
        return new ZcodeAction(ActionType.INVOKE_ADDRESSING, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction addressingMoveAlong(ParseState parseState) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction commandMoveAlong(ParseState parseState) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction firstCommand(ParseState parseState) {
        return new ZcodeAction(ActionType.RUN_FIRST_COMMAND, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction runCommand(ParseState parseState, byte prefixMarker) {
        return new ZcodeAction(ActionType.RUN_COMMAND, parseState, prefixMarker);
    }

    @Override
    public ZcodeAction endSequence(ParseState parseState) {
        return new ZcodeAction(ActionType.END_SEQUENCE, parseState, (byte) 0);
    }

    @Override
    public ZcodeAction closeParen(ParseState parseState) {
        return new ZcodeAction(ActionType.CLOSE_PAREN, parseState, (byte) 0);
    }

}

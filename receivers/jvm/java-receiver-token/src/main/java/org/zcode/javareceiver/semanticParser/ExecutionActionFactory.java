package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

public class ExecutionActionFactory implements ZcodeActionFactory {
    @Override
    public ZcodeAction goAround(ParseState parseState) {
        return new ZcodeAction(ActionType.GO_AROUND, parseState);
    }

    @Override
    public ZcodeAction waitForAsync(ParseState parseState) {
        return new ZcodeAction(ActionType.WAIT_FOR_ASYNC, parseState);
    }

    @Override
    public ZcodeAction waitForTokens(ParseState parseState) {
        return new ZcodeAction(ActionType.WAIT_FOR_TOKENS, parseState);
    }

    @Override
    public ZcodeAction error(ParseState parseState) {
        return new ZcodeAction(ActionType.ERROR, parseState);
    }

    @Override
    public ZcodeAction addressing(ParseState parseState) {
        return new ZcodeAction(ActionType.INVOKE_ADDRESSING, parseState);
    }

    @Override
    public ZcodeAction addressingMoveAlong(ParseState parseState) {
        return new ZcodeAction(ActionType.ADDRESSING_MOVEALONG, parseState);
    }

    @Override
    public ZcodeAction commandMoveAlong(ParseState parseState) {
        return new ZcodeAction(ActionType.COMMAND_MOVEALONG, parseState);
    }

    @Override
    public ZcodeAction firstCommand(ParseState parseState) {
        return new ZcodeAction(ActionType.RUN_FIRST_COMMAND, parseState);
    }

    @Override
    public ZcodeAction runCommand(ParseState parseState) {
        return new ZcodeAction(ActionType.RUN_COMMAND, parseState);
    }

    @Override
    public ZcodeAction endSequence(ParseState parseState) {
        return new ZcodeAction(ActionType.END_SEQUENCE, parseState);
    }

    @Override
    public ZcodeAction closeParen(ParseState parseState) {
        return new ZcodeAction(ActionType.CLOSE_PAREN, parseState);
    }

    @Override
    public ZcodeAction ofType(ParseState parseState, ActionType type) {
        return new ZcodeAction(type, parseState);
    }

}

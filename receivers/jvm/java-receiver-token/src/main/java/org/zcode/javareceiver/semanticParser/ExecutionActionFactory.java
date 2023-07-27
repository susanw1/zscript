package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

public class ExecutionActionFactory implements ZcodeActionFactory {

    @Override
    public ZcodeAction waitForTokens(ParseState parseState) {
        return new ZcodeAction(ActionType.WAIT_FOR_TOKENS, parseState);
    }

    @Override
    public ZcodeAction ofType(ParseState parseState, ActionType type) {
        return new ZcodeAction(type, parseState);
    }

}

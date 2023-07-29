package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

public class ExecutionActionFactory implements ZcodeActionFactory {

    @Override
    public ZcodeSemanticAction waitForTokens(ParseState parseState) {
        return new ZcodeSemanticAction(ActionType.WAIT_FOR_TOKENS, parseState);
    }

    @Override
    public ZcodeSemanticAction ofType(ParseState parseState, ActionType type) {
        return new ZcodeSemanticAction(type, parseState);
    }

}

package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

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

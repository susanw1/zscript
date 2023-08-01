package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;

public class ExecutionActionFactory implements ActionFactory {

    @Override
    public SemanticAction waitForTokens(ParseState parseState) {
        return new SemanticAction(ActionType.WAIT_FOR_TOKENS, parseState);
    }

    @Override
    public SemanticAction ofType(ParseState parseState, ActionType type) {
        return new SemanticAction(type, parseState);
    }
}

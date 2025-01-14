package net.zscript.javareceiver.semanticparser;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.semanticparser.SemanticAction.ActionType;

public class ExecutionActionFactory implements ActionFactory {

    @Nonnull
    @Override
    public SemanticAction waitForTokens(ParseState parseState) {
        return new SemanticAction(ActionType.WAIT_FOR_TOKENS, parseState);
    }

    @Nonnull
    @Override
    public SemanticAction ofType(ParseState parseState, ActionType type) {
        return new SemanticAction(type, parseState);
    }
}

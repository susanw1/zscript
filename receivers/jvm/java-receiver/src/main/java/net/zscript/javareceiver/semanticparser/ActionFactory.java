package net.zscript.javareceiver.semanticparser;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.semanticparser.SemanticAction.ActionType;

public interface ActionFactory {
    @Nonnull
    SemanticAction waitForTokens(ParseState parseState);

    @Nonnull
    SemanticAction ofType(ParseState parseState, ActionType type);
}

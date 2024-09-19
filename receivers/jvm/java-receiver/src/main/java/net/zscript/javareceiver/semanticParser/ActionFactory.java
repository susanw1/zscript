package net.zscript.javareceiver.semanticParser;

import javax.annotation.Nonnull;

import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;

public interface ActionFactory {
    @Nonnull
    SemanticAction waitForTokens(ParseState parseState);

    @Nonnull
    SemanticAction ofType(ParseState parseState, ActionType type);
}

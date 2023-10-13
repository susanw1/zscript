package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.semanticParser.SemanticAction.ActionType;

public interface ActionFactory {

    SemanticAction waitForTokens(ParseState parseState);

    SemanticAction ofType(ParseState parseState, ActionType type);

}
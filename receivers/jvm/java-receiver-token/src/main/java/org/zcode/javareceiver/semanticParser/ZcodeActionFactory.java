package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

public interface ZcodeActionFactory {

    ZcodeSemanticAction waitForTokens(ParseState parseState);

    ZcodeSemanticAction ofType(ParseState parseState, ActionType type);

}
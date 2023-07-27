package org.zcode.javareceiver.semanticParser;

import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;

public interface ZcodeActionFactory {

    ZcodeAction waitForTokens(ParseState parseState);

    ZcodeAction ofType(ParseState parseState, ActionType type);

}
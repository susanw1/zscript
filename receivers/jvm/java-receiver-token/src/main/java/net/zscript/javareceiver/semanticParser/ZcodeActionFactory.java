package net.zscript.javareceiver.semanticParser;

import net.zscript.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;

public interface ZcodeActionFactory {

    ZcodeSemanticAction waitForTokens(ParseState parseState);

    ZcodeSemanticAction ofType(ParseState parseState, ActionType type);

}
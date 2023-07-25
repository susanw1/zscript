package org.zcode.javareceiver.semanticParser;

public interface ZcodeActionFactory {
    ZcodeAction noAction(ParseState parseState);

    ZcodeAction needsTokens(ParseState parseState);

    ZcodeAction error(ParseState parseState, byte error);

    ZcodeAction addressing(ParseState parseState);

    ZcodeAction addressingMoveAlong(ParseState parseState);

    ZcodeAction firstCommand(ParseState parseState);

    ZcodeAction runCommand(ParseState parseState, byte prefixMarker);

    ZcodeAction commandMoveAlong(ParseState parseState);

    ZcodeAction endSequence(ParseState parseState);

    ZcodeAction closeParen(ParseState parseState);

}
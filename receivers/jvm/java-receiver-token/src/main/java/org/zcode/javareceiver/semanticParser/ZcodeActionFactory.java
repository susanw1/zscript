package org.zcode.javareceiver.semanticParser;

public interface ZcodeActionFactory {
    ZcodeAction goAround(ParseState parseState);

    ZcodeAction waitForAsync(ParseState parseState);

    ZcodeAction waitForTokens(ParseState parseState);

    ZcodeAction error(ParseState parseState);

    ZcodeAction addressing(ParseState parseState);

    ZcodeAction addressingMoveAlong(ParseState parseState);

    ZcodeAction firstCommand(ParseState parseState);

    ZcodeAction runCommand(ParseState parseState, byte prefixMarker);

    ZcodeAction commandMoveAlong(ParseState parseState);

    ZcodeAction endSequence(ParseState parseState);

    ZcodeAction closeParen(ParseState parseState);

}
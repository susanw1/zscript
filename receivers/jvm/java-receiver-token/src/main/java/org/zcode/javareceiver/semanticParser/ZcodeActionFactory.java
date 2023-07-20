package org.zcode.javareceiver.semanticParser;

import java.util.function.Consumer;

public interface ZcodeActionFactory {
    ZcodeAction noAction(SemanticParser semanticParser);

    ZcodeAction needsTokens(SemanticParser semanticParser);

    ZcodeAction error(SemanticParser semanticParser, byte error);

    ZcodeAction addressing(SemanticParser semanticParser, Consumer<ParseState> before, Consumer<ParseState> after);

    ZcodeAction addressingMoveAlong(SemanticParser semanticParser, Consumer<ParseState> before);

    ZcodeAction firstCommand(SemanticParser semanticParser, Consumer<ParseState> before);

    ZcodeAction runCommand(SemanticParser semanticParser, byte prefixMarker, Consumer<ParseState> before, Consumer<ParseState> after);

    ZcodeAction commandMoveAlong(SemanticParser semanticParser, Consumer<ParseState> before);

    ZcodeAction endSequence(SemanticParser semanticParser, Consumer<ParseState> after);

    ZcodeAction closeParen(SemanticParser semanticParser, Consumer<ParseState> after);

}
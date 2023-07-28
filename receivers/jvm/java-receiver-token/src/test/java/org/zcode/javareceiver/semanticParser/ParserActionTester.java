package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ZcodeAction;
import org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

/**
 * Testing utility methods.
 */
class ParserActionTester {
    private final Zcode            zcode;
    private final ZcodeTokenBuffer buffer;
    private final ZcodeTokenizer   tokenizer;
    private final SemanticParser   parser;
    private final ZcodeOutStream   outStream;

    public ParserActionTester(Zcode zcode, ZcodeTokenBuffer buffer, ZcodeTokenizer tokenizer, SemanticParser parser, ZcodeOutStream outStream) {
        this.zcode = zcode;
        this.buffer = buffer;
        this.tokenizer = tokenizer;
        this.parser = parser;
        this.outStream = outStream;
    }

    void parseSnippet(final String text, final List<ActionType> actionTypes) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType=" + t);
            ZcodeAction a1 = parser.getAction();

            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            assertThat(((ZcodeSemanticAction) a1).getType()).isEqualTo(t);
            a1.performAction(zcode, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
    }
}
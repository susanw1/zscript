package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.List;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.semanticParser.SemanticParser.State;
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
            ZcodeSemanticAction action = parser.getAction();

            System.out.println("  Received action: actionType=" + action + "; state=" + parser.getState());
            assertThat(action.getType()).isEqualTo(t);
            action.performAction(zcode, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
    }

    void parseSnippet(final String text, String output) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        ZcodeSemanticAction a1;
        while ((a1 = parser.getAction()).getType() != ActionType.WAIT_FOR_TOKENS) {
            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            a1.performAction(zcode, outStream);
            System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        }
        assertThat(outStream.toString()).isEqualTo(output);
    }

    void parseSnippet(final String text, ActionType expectedActionType, State endState, String output) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        ZcodeSemanticAction action = parser.getAction();
        System.out.println("  Received action: actionType=" + action + " (expected=" + expectedActionType + "); state=" + parser.getState());
        if (expectedActionType != null) {
            assertThat(action.getType()).isEqualTo(expectedActionType);
        }

        action.performAction(zcode, outStream);
        System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());

        if (endState != null) {
            assertThat(parser.getState()).isEqualTo(endState);
        }
        assertThat(outStream.toString()).isEqualTo(output);
    }
}
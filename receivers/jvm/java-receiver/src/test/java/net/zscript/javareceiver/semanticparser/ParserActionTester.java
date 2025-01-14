package net.zscript.javareceiver.semanticparser;

import java.util.List;

import static org.assertj.core.api.Assertions.assertThat;

import net.zscript.javareceiver.core.SequenceOutStream;
import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.semanticparser.SemanticAction.ActionType;
import net.zscript.javareceiver.semanticparser.SemanticParser.State;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;

/**
 * Testing utility methods.
 */
class ParserActionTester {
    private final Zscript           zscript;
    private final TokenBuffer       buffer;
    private final Tokenizer         tokenizer;
    private final SemanticParser    parser;
    private final SequenceOutStream outStream;

    public ParserActionTester(Zscript zscript, TokenBuffer buffer, Tokenizer tokenizer, SemanticParser parser, SequenceOutStream outStream) {
        this.zscript = zscript;
        this.buffer = buffer;
        this.tokenizer = tokenizer;
        this.parser = parser;
        this.outStream = outStream;
    }

    void parseSnippet(final String text, final List<ActionType> actionTypes) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().tokenIterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType=" + t);
            SemanticAction action = parser.getAction();

            System.out.println("  Received action: actionType=" + action + "; state=" + parser.getState());
            assertThat(action.getType()).isEqualTo(t);
            action.performAction(zscript, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
    }

    void parseSnippet(final String text, String output) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().tokenIterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        SemanticAction a1;
        while ((a1 = parser.getAction()).getType() != ActionType.WAIT_FOR_TOKENS) {
            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            a1.performAction(zscript, outStream);
            System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        }
        assertThat(outStream.toString()).isEqualTo(output);
    }

    void parseSnippet(final String text, ActionType expectedActionType, State endState, String output) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().tokenIterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        SemanticAction action = parser.getAction();
        System.out.println("  Received action: actionType=" + action + " (expected=" + expectedActionType + "); state=" + parser.getState());
        if (expectedActionType != null) {
            assertThat(action.getType()).isEqualTo(expectedActionType);
        }

        action.performAction(zscript, outStream);
        System.out.println("   - After execute action: outStream=" + outStream.toString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());

        if (endState != null) {
            assertThat(parser.getState()).isEqualTo(endState);
        }
        assertThat(outStream.toString()).isEqualTo(output);
    }
}

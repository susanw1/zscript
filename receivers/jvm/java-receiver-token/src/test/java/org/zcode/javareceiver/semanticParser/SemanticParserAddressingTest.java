package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.ERROR;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.INVOKE_ADDRESSING;
import static org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType.WAIT_FOR_TOKENS;

import java.util.List;

import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.core.StringBuilderOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class SemanticParserAddressingTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = new Zcode();

    private final StringBuilderOutStream outStream = new StringBuilderOutStream();

    void shouldProduceActions(final String text, final List<ActionType> actionTypes, final String finalOutput) {
        // feed all chars into tokens/buffer
        text.chars().forEachOrdered(c -> tokenizer.accept((byte) c));

        buffer.getTokenReader().iterator().forEach(t -> System.out.print(t + " "));
        System.out.println();

        actionTypes.forEach(t -> {
            System.out.println("Expecting actionType=" + t);
            ZcodeAction a1 = parser.getAction();

            System.out.println("  Received action: actionType=" + a1 + "; state=" + parser.getState());
            assertThat(a1.getType()).isEqualTo(t);
            a1.performAction(zcode, outStream);

            System.out.println("   - After execute action: outStream=" + outStream.getString().replaceAll("\\n", "\\\\n") + "; state=" + parser.getState());
        });
        assertThat(outStream.getString()).isEqualTo(finalOutput);
    }

    @Test
    void shouldProduceActionForBasicAddressing() {
        shouldProduceActions("@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS), "");
    }

    @Test
    void shouldAcceptAddressingWithEchoAndLocks() {
        shouldProduceActions("_2%233@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS), "");
    }

    @Test
    void shouldFailInvalidAddressing() {
        shouldProduceActions("@99999Z1\n", List.of(ERROR, WAIT_FOR_TOKENS), "!10S20\n");
    }

    @Test
    void shouldAcceptMultPartAddressing() {
        shouldProduceActions("@1.2.3.4.5.6.7.8.9.0.1.2.3.4.5.6.7.8.9.0Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS), "");
    }

}

package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.RUN_FIRST_COMMAND;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.WAIT_FOR_TOKENS;

import java.io.IOException;
import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.zcode.javareceiver.core.StringWriterOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

class SemanticParserMulticommandTest {
    private final ZcodeTokenBuffer buffer    = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final ZcodeTokenizer   tokenizer = new ZcodeTokenizer(buffer.getTokenWriter(), 2);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zcode zcode = new Zcode();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;

    @BeforeEach
    void setup() throws IOException {
        zcode.addModule(new ZcodeCoreModule());
        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zcode, buffer, tokenizer, parser, outStream);
    }

    @Test
    public void shouldHandleTwoEmptyCommands() {
        parserActionTester.parseSnippet("\n \n", List.of(RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!\n!\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldHandleSeveralEmptyCommands() {
        parserActionTester.parseSnippet("\n\n \n\n",
                List.of(RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!\n!\n!\n!\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    public void shouldHandleSeveralBasicCommands() {
        parserActionTester.parseSnippet("Z1A\nZ1B\n Z1C\nZ1D\n",
                List.of(RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, RUN_FIRST_COMMAND, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!AS\n!BS\n!CS\n!DS\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceMultipleResponses(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceMultipleResponses() {
        return Stream.of(
                of("Z1 A2 +1234\n Z1 A3B3 +5678\n", "!A2+1234S\n!A3B3+5678S\n"),
                of("Z1A S1 \n Z1B\n", "!AS1\n!BS\n"),
                of("Z1A S1 | Z1B S1 \n Z1C\n", "!AS1|BS1\n!CS\n"),
                of("Z1A S13 \n Z1B\n", "!AS13\n!BS\n"),
                of("Z1A S13 | Z1B S1 \n Z1C\n", "!AS13\n!CS\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceMultipleResponsesWithComments(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceMultipleResponsesWithComments() {
        return Stream.of(
                of("Z1 A \n#hi\n", "!AS\n"),
                of("Z1 A \n#hi\n#abc\nZ1B\n", "!AS\n!BS\n"));
    }

    @ParameterizedTest
    @MethodSource
    public void shouldProduceMultipleResponsesWithAddressing(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldProduceMultipleResponsesWithAddressing() {
        // Addressing actions are passed on downstream, not acknowledged - they should not appear in the response.
        return Stream.of(
                of("Z1 A \n@1 Z1B\n", "!AS\n"),
                of("Z1 A \n@1.2 Z1B\n@2.2 Z1C\n Z1D\n", "!AS\n!DS\n"));
    }
}

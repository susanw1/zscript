package net.zscript.javareceiver.semanticparser;

import java.io.IOException;
import java.util.stream.Stream;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.params.provider.Arguments.of;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.StringWriterOutStream;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

class EchoParserTest {
    private final TokenBuffer buffer    = TokenRingBuffer.createBufferWithCapacity(256);
    private final Tokenizer   tokenizer = new Tokenizer(buffer.getTokenWriter(), false);

    private final SemanticParser parser = new SemanticParser(buffer.getTokenReader(), new ExecutionActionFactory());

    private final Zscript zscript = new Zscript();

    private StringWriterOutStream outStream;
    private ParserActionTester    parserActionTester;

    @BeforeEach
    void setup() throws IOException {
        zscript.addModule(new ZscriptCoreModule());
        outStream = new StringWriterOutStream();
        parserActionTester = new ParserActionTester(zscript, buffer, tokenizer, parser, outStream);
    }

    @Test
    public void shouldEchoSequenceLevelValue() {
        parserActionTester.parseSnippet(">1234Z1\n", "!>1234S\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @ParameterizedTest
    @MethodSource
    public void shouldEchoSequenceLevelLineNumber(final String text, String finalOutput) {
        parserActionTester.parseSnippet(text, finalOutput);
        assertThat(outStream.isOpen()).isFalse();
    }

    private static Stream<Arguments> shouldEchoSequenceLevelLineNumber() {
        return Stream.of(
                of(">1234Z1\n", "!>1234S\n"),
                of(">\n", "!>\n"),
                of(">1\n>2\n", "!>1\n!>2\n"),
                of(">>\n", "!>S31\n"),
                of(">12345\n", "!S20\n"));
    }

}

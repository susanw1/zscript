package net.zscript.javareceiver.semanticparser;

import java.io.IOException;
import java.util.List;

import static net.zscript.javareceiver.semanticparser.SemanticAction.ActionType.END_SEQUENCE;
import static net.zscript.javareceiver.semanticparser.SemanticAction.ActionType.ERROR;
import static net.zscript.javareceiver.semanticparser.SemanticAction.ActionType.INVOKE_ADDRESSING;
import static net.zscript.javareceiver.semanticparser.SemanticAction.ActionType.WAIT_FOR_TOKENS;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.modules.core.ZscriptCoreModule;
import net.zscript.javareceiver.testing.StringWriterOutStream;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenRingBuffer;
import net.zscript.tokenizer.Tokenizer;

public class SemanticParserAddressingTest {
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
    void shouldProduceActionForBasicAddressing() {
        parserActionTester.parseSnippet("@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldAcceptAddressingWithEchoAndLocks() {
        parserActionTester.parseSnippet(">2%233@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldFailInvalidAddressing() {
        parserActionTester.parseSnippet("@99999Z1\n", List.of(ERROR, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!S20\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldFailNonExistentAddressing() {
        zscript.setNotificationOutStream(outStream);
        parserActionTester.parseSnippet("@9999Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!S12\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldAcceptMultPartAddressing() {
        parserActionTester.parseSnippet("@1.2.3.4.5.6.7.8.9.0.1.2.3.4.5.6.7.8.9.0Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

}

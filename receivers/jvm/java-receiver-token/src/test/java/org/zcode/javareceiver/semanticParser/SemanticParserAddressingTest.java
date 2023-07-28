package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.END_SEQUENCE;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.ERROR;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.INVOKE_ADDRESSING;
import static org.zcode.javareceiver.semanticParser.ZcodeSemanticAction.ActionType.WAIT_FOR_TOKENS;

import java.io.IOException;
import java.util.List;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.core.StringWriterOutStream;
import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.modules.core.ZcodeCoreModule;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class SemanticParserAddressingTest {
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
    void shouldProduceActionForBasicAddressing() {
        parserActionTester.parseSnippet("@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldAcceptAddressingWithEchoAndLocks() {
        parserActionTester.parseSnippet("_2%233@2Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldFailInvalidAddressing() {
        parserActionTester.parseSnippet("@99999Z1\n", List.of(ERROR, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("!10S20\n");
        assertThat(outStream.isOpen()).isFalse();
    }

    @Test
    void shouldAcceptMultPartAddressing() {
        parserActionTester.parseSnippet("@1.2.3.4.5.6.7.8.9.0.1.2.3.4.5.6.7.8.9.0Z1\n", List.of(INVOKE_ADDRESSING, END_SEQUENCE, WAIT_FOR_TOKENS));
        assertThat(outStream.getString()).isEqualTo("");
        assertThat(outStream.isOpen()).isFalse();
    }

}

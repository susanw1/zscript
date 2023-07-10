package org.zcode.javareceiver.semanticParser;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.zcode.javareceiver.semanticParser.ZcodeAction.ActionType;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenWriter;
import org.zcode.javareceiver.tokenizer.ZcodeTokenRingBuffer;

class SemanticParserTest {
    private final ZcodeTokenBuffer buffer      = ZcodeTokenRingBuffer.createBufferWithCapacity(256);
    private final TokenWriter      tokenWriter = buffer.getTokenWriter();
    private final TokenReader      tokenReader = buffer.getTokenReader();

    private final SemanticParser parser = new SemanticParser(tokenReader);

    @BeforeEach
    void setUp() throws Exception {
    }

    @Test
    void shouldStartNoActionWithEmptyTokens() {
        final ZcodeAction a = parser.getAction();
        assertThat(a.getType()).isEqualTo(ActionType.NO_ACTION);
    }

}

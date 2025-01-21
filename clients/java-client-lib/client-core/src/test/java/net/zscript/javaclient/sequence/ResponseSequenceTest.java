package net.zscript.javaclient.sequence;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ZscriptParseException;

class ResponseSequenceTest {
    @Test
    public void shouldParseEmptySequence() {
        final ResponseSequence respSeq = parse("!");
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(0);
        assertThat(respSeq.getEchoValue()).isEqualTo(-1);
        assertThat(respSeq.asStringUtf8()).isEqualTo("!");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("");

    }

    @Test
    public void shouldParseResponseAndEchoValue() {
        final ResponseSequence respSeq = parse("!3 _a AS");
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(3);
        assertThat(respSeq.getEchoValue()).isEqualTo(0x0a);
        assertThat(respSeq.getEchoValue()).isEqualTo(0x0a);
        assertThat(respSeq.asStringUtf8()).isEqualTo("!3_aAS");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("AS");
    }

    @Test
    public void shouldFailWithoutResponseMarker() {
        assertThatThrownBy(() -> parse(""))
                .isInstanceOf(ZscriptParseException.class);
    }

    @Test
    public void shouldImplementToString() {
        assertThat(parse("!1 _2 A")).hasToString("ResponseSequence:{'!1_2A'}");
    }

    private static ResponseSequence parse(String s) {
        return ResponseSequence.parse(tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken());
    }
}

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
        assertThat(respSeq.isTimedOut()).isFalse();
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(0);
        assertThat(respSeq.hasEchoValue()).isFalse();
        assertThat(respSeq.getEchoValue()).isEqualTo(-1);
        assertThat(respSeq.asStringUtf8()).isEqualTo("!");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("");

    }

    @Test
    public void shouldParseResponseAndEchoValue() {
        final ResponseSequence respSeq = parse("!3 =a AS");
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(3);
        assertThat(respSeq.hasEchoValue()).isTrue();
        assertThat(respSeq.getEchoValue()).isEqualTo(0x0a);
        assertThat(respSeq.asStringUtf8()).isEqualTo("!3=aAS");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("AS");
    }

    @Test
    public void shouldFailWithoutResponseMarker() {
        assertThatThrownBy(() -> parse(""))
                .isInstanceOf(ZscriptParseException.class);
    }

    @Test
    public void shouldCreateEmptySequence() {
        final ResponseSequence respSeq = ResponseSequence.empty();
        assertThat(respSeq.isTimedOut()).isFalse();
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(-1);
        assertThat(respSeq.hasEchoValue()).isFalse();
        assertThat(respSeq.getEchoValue()).isEqualTo(-1);
        assertThat(respSeq.asStringUtf8()).isEqualTo("");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("");
    }

    @Test
    public void shouldCreateTimedOutSequence() {
        final ResponseSequence respSeq = ResponseSequence.timedOut();
        assertThat(respSeq.isTimedOut()).isTrue();
        assertThat(respSeq.getResponseFieldValue()).isEqualTo(-1);
        assertThat(respSeq.hasEchoValue()).isFalse();
        assertThat(respSeq.getEchoValue()).isEqualTo(-1);
        assertThat(respSeq.asStringUtf8()).isEqualTo("");
        assertThat(respSeq.getExecutionPath().asStringUtf8()).isEqualTo("");
    }

    @Test
    public void shouldImplementToString() {
        assertThat(parse("!1 =2 A")).hasToString("ResponseSequence:{'!1=2A'}");
    }

    private static ResponseSequence parse(String s) {
        return ResponseSequence.parse(tokenize(byteStringUtf8(s), true).getTokenReader().getFirstReadToken());
    }
}

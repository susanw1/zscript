package net.zscript.javaclient.tokens;

import static net.zscript.javaclient.tokens.ExtendingTokenBuffer.tokenize;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.javaclient.addressing.AddressedResponse;
import net.zscript.javaclient.addressing.CompleteAddressedResponse;
import net.zscript.javaclient.commandPaths.ZscriptFieldSet;
import net.zscript.javaclient.sequence.CommandSequence;
import net.zscript.model.ZscriptModel;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.Tokenizer;

public class ExtendingTokenBufferTest {
    @Test
    public void shouldTokenizeCommand() {
        final TokenBuffer     buf        = tokenize(byteStringUtf8("Z5 A3\n"));
        final CommandSequence commandSeq = CommandSequence.parse(ZscriptModel.rawModel(), buf.getTokenReader().getFirstReadToken(), false);

        final ZscriptFieldSet fields = commandSeq.getExecutionPath().getFirstCommand().getFields();
        assertThat(fields.getFieldCount()).isEqualTo(2);
        assertThat(fields.getField('Z')).isPresent().hasValue(5);
        assertThat(fields.getField('A')).isPresent().hasValue(3);
    }

    @Test
    public void tokenizerShouldRejectBadCommand() {
        assertThatThrownBy(() -> tokenize(byteStringUtf8("Z54321 \n")))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Syntax error");
    }

    @Test
    public void tokenizerShouldRejectOverlongCommand() {
        assertThatThrownBy(() -> tokenize(byteStringUtf8("Z1 \n X")))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Extra characters");
    }

    @Test
    public void shouldTokenizeResponse() {
        final TokenBuffer       buf      = tokenize(byteStringUtf8("!S5"));
        final AddressedResponse response = CompleteAddressedResponse.parse(buf.getTokenReader()).asResponse();
        assertThat(response.hasAddress()).isFalse();
        final ZscriptFieldSet fields = response.getContent().getExecutionPath().getFirstResponse().getFields();
        assertThat(fields.getFieldCount()).isEqualTo(1);
        assertThat(fields.getField('S')).isPresent().hasValue(5);
    }

    @Test
    public void tokenizerShouldRejectBadResponse() {
        final TokenBuffer buf = tokenize(byteStringUtf8("S1 \n")); // missing '!'
        assertThatThrownBy(() -> CompleteAddressedResponse.parse(buf.getTokenReader()))
                .isInstanceOf(ZscriptParseException.class).hasMessageContaining("Invalid response sequence");
    }

    @Test
    public void shouldTokenizeIncompleteCommandWithoutNewline() {
        long tokenCount = tokenize(byteStringUtf8("S1"), false).getTokenReader().tokenIterator().stream().count();
        assertThat(tokenCount).isEqualTo(1);
    }

    @Test
    public void shouldTokenizeIncompleteCommandWithNewline() {
        long tokenCount = tokenize(byteStringUtf8("S1"), true).getTokenReader().tokenIterator().stream().count();
        assertThat(tokenCount).isEqualTo(2);
    }

    @Test
    public void shouldExpandBuffer() {
        final ExtendingTokenBuffer buf = new ExtendingTokenBuffer();
        final Tokenizer            tok = new Tokenizer(buf.getTokenWriter());

        assertThat(buf.getDataSize()).isLessThan(500);
        tok.accept((byte) '"');
        for (int i = 0; i < 1000; i++) {
            tok.accept((byte) 'x');
        }
        assertThat(buf.getDataSize()).isGreaterThan(1000);
    }

    @Test
    public void shouldTokenizeText() {
        final ExtendingTokenBuffer buf = tokenize(byteStringUtf8("!S5\n"));
        assertThat(buf.getTokenBytes()).isEqualTo(byteString(new byte[] { '!', 0, 'S', 1, 5, Tokenizer.NORMAL_SEQUENCE_END }));
    }

    @Test
    public void shouldCreateTokenBufferFromBytes() {
        ExtendingTokenBuffer buf = ExtendingTokenBuffer.fromTokenBytes(byteString(new byte[] { '!', 0, 'S', 1, 5, Tokenizer.NORMAL_SEQUENCE_END }));
        assertThat(buf.getDataSize()).isEqualTo(6);
    }

    @Test
    public void shouldRejectTokenBufferFromIncorrectBytes() {
        assertThatThrownBy(() -> ExtendingTokenBuffer.fromTokenBytes(byteString(new byte[] { '!', 0, 'S', 0, 5, Tokenizer.NORMAL_SEQUENCE_END })))
                .isInstanceOf(IllegalArgumentException.class).hasMessageContaining("Not a valid token string");
    }
}

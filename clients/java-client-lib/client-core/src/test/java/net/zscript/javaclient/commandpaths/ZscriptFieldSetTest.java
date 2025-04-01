package net.zscript.javaclient.commandpaths;

import java.util.List;

import static java.util.Collections.emptyList;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOfBytes;
import static net.zscript.util.ByteString.ByteAppender.ISOLATIN1_APPENDER;
import static net.zscript.util.ByteString.byteString;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class ZscriptFieldSetTest {
    ZscriptFieldSet fieldSet = ZscriptFieldSet.fromList(
            List.of(new BigField(new byte[] { 'a', 'b' }, false), new BigField(new byte[] { 'c', 'd', 'e' }, true)),
            List.of(fieldOf((byte) 'A', 12), fieldOf((byte) 'Z', 7), fieldOf((byte) 'C', 0), fieldOf((byte) 'D', 0x123)));

    @Test
    public void shouldInitializeFromMap() {
        assertThat(fieldSet.getFieldCount()).isEqualTo(4);
        assertThat(fieldSet.getField('A')).hasValue(12);
        assertThat(fieldSet.getField('Z')).hasValue(7);
        assertThat(fieldSet.hasField('Z')).isTrue();
        assertThat(fieldSet.getFieldVal((byte) 'Z')).isEqualTo(fieldOfBytes((byte) 'Z', byteString((byte) 7)));

        assertThat(fieldSet.getZscriptField('Z').orElseThrow()).isEqualTo(fieldOf((byte) 'Z', 7));
        assertThat(fieldSet.getField('B')).isNotPresent();
        assertThat(fieldSet.hasField('B')).isFalse();
        assertThat(fieldSet.getZscriptField('B')).isNotPresent();
        assertThat(fieldSet.getFieldVal((byte) 'B')).isNull();

        assertThat(fieldSet.hasBigField()).isTrue();
        assertThat(fieldSet.getBigFieldSize()).isEqualTo(5);
        assertThat(fieldSet.getBigFieldAsByteString().asString()).isEqualTo("abcde");
    }

    @Test
    public void shouldAppendToByteString() {
        assertThat(byteString(fieldSet).asString()).isEqualTo("Z7AcCD123+6162\"cde\"");
    }

    @Test
    public void shouldDetermineIfEmpty() {
        assertThat(fieldSet.isEmpty()).isFalse();
        assertThat(ZscriptFieldSet.blank().isEmpty()).isTrue();
        assertThat(ZscriptFieldSet.blank().hasBigField()).isFalse();
        assertThat(ZscriptFieldSet.fromList(emptyList(), List.of(fieldOf((byte) 'Z', 4))).isEmpty()).isFalse();
    }

    @Test
    @Disabled("for now - fix this")
    public void shouldDetermineBufferLength() {
        assertThat(fieldSet.getBufferLength()).isEqualTo(21);
        assertThat(ZscriptFieldSet.blank().getBufferLength()).isEqualTo(0);
    }

    @Test
    public void shouldCreateFromTokens() {
        final ExtendingTokenBuffer buf = ExtendingTokenBuffer.tokenize(byteString("Z7Ac B616263 C\"cde\"", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs = ZscriptFieldSet.fromTokens(buf.getTokenReader().getFirstReadToken());
        assertThat(byteString(zfs).asString()).isEqualTo("Z7AcB616263C636465");
    }

    @Test
    public void shouldMatchExpressionDescriptions() {
        final ExtendingTokenBuffer buf1 = ExtendingTokenBuffer.tokenize(byteString("Z7 Ac B616263 C\"cde\"", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs1 = ZscriptFieldSet.fromTokens(buf1.getTokenReader().getFirstReadToken());
        final ExtendingTokenBuffer buf2 = ExtendingTokenBuffer.tokenize(byteString("Z7 Ab B2 C636465", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs2 = ZscriptFieldSet.fromTokens(buf2.getTokenReader().getFirstReadToken());

        assertThat(zfs1.matchDescription(zfs1)).isEmpty();
        assertThat(zfs1.matches(zfs1)).isTrue();
        assertThat(zfs2.matchDescription(zfs2)).isEmpty();
        assertThat(zfs1.matches(zfs2)).isFalse();

        assertThat(zfs1.matchDescription(zfs2)).isEqualTo("AcB616263 <> AbB2");
        assertThat(zfs2.matchDescription(zfs1)).isEqualTo("AbB2 <> AcB616263");

        final ExtendingTokenBuffer buf3 = ExtendingTokenBuffer.tokenize(byteString("Z7Ab", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs3 = ZscriptFieldSet.fromTokens(buf3.getTokenReader().getFirstReadToken());
        assertThat(zfs3.matchDescription(zfs3)).isEmpty();
        assertThat(zfs2.matchDescription(zfs3)).isEqualTo("B2C636465 <> ");
        assertThat(zfs2.matches(zfs3)).isFalse();
    }
}

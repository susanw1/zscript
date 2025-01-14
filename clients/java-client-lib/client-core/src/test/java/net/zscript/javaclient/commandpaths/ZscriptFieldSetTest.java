package net.zscript.javaclient.commandpaths;

import java.util.List;
import java.util.Map;

import static java.util.Collections.emptyList;
import static net.zscript.javaclient.commandpaths.NumberField.fieldOf;
import static net.zscript.util.ByteString.ByteAppender.ISOLATIN1_APPENDER;
import static net.zscript.util.ByteString.byteString;
import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.tokens.ExtendingTokenBuffer;

public class ZscriptFieldSetTest {
    ZscriptFieldSet fieldSet = ZscriptFieldSet.fromMap(
            List.of(new BigField(new byte[] { 'a', 'b' }, false), new BigField(new byte[] { 'c', 'd', 'e' }, true)),
            Map.of((byte) 'A', 12, (byte) 'Z', 7, (byte) 'C', 0, (byte) 'D', 0x123));

    @Test
    public void shouldInitializeFromMap() {
        assertThat(fieldSet.getFieldCount()).isEqualTo(4);
        assertThat(fieldSet.getField('A')).hasValue(12);
        assertThat(fieldSet.getField('Z')).hasValue(7);
        assertThat(fieldSet.hasField('Z')).isTrue();
        assertThat(fieldSet.getFieldVal((byte) 'Z')).isEqualTo(7);

        assertThat(fieldSet.getZscriptField('Z').orElseThrow()).isEqualTo(fieldOf((byte) 'Z', 7));
        assertThat(fieldSet.getField('B')).isNotPresent();
        assertThat(fieldSet.hasField('B')).isFalse();
        assertThat(fieldSet.getZscriptField('B')).isNotPresent();
        assertThat(fieldSet.getFieldVal((byte) 'B')).isEqualTo(-1);

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
        assertThat(ZscriptFieldSet.fromMap(emptyList(), Map.of((byte) 'Z', 4)).isEmpty()).isFalse();
    }

    @Test
    public void shouldDetermineBufferLength() {
        assertThat(fieldSet.getBufferLength()).isEqualTo(21);
        assertThat(ZscriptFieldSet.blank().getBufferLength()).isEqualTo(0);
    }

    @Test
    public void shouldCreateFromTokens() {
        final ExtendingTokenBuffer buf = ExtendingTokenBuffer.tokenize(byteString("Z7Ac+6162\"cde\"", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs = ZscriptFieldSet.fromTokens(buf.getTokenReader().getFirstReadToken());
        assertThat(byteString(zfs).asString()).isEqualTo("Z7Ac+6162\"cde\"");
    }

    @Test
    public void shouldMatchExpressionDescriptions() {
        final ExtendingTokenBuffer buf1 = ExtendingTokenBuffer.tokenize(byteString("Z7 Ac +6162 \"cde\"", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs1 = ZscriptFieldSet.fromTokens(buf1.getTokenReader().getFirstReadToken());
        final ExtendingTokenBuffer buf2 = ExtendingTokenBuffer.tokenize(byteString("Z7 Ab B2 \"abc\"", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs2 = ZscriptFieldSet.fromTokens(buf2.getTokenReader().getFirstReadToken());

        assertThat(zfs1.matchDescription(zfs1)).isEmpty();
        assertThat(zfs1.matches(zfs1)).isTrue();
        assertThat(zfs2.matchDescription(zfs2)).isEmpty();
        assertThat(zfs1.matches(zfs2)).isFalse();

        assertThat(zfs1.matchDescription(zfs2)).isEqualTo("Ac <> B2Ab + abcde <> abc");
        assertThat(zfs2.matchDescription(zfs1)).isEqualTo("B2Ab <> Ac + abc <> abcde");

        final ExtendingTokenBuffer buf3 = ExtendingTokenBuffer.tokenize(byteString("Z7Ac", ISOLATIN1_APPENDER));
        final ZscriptFieldSet      zfs3 = ZscriptFieldSet.fromTokens(buf3.getTokenReader().getFirstReadToken());
        assertThat(zfs3.matchDescription(zfs3)).isEmpty();
        assertThat(zfs1.matchDescription(zfs3)).isEqualTo(" + abcde <> ");
        assertThat(zfs1.matches(zfs3)).isFalse();
    }
}

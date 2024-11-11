package net.zscript.javaclient.commandPaths;

import java.util.List;
import java.util.Map;

import static java.util.Collections.emptyList;
import static java.util.Collections.emptyMap;
import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;
import static net.zscript.util.ByteString.ByteAppender.ISOLATIN1_APPENDER;
import static net.zscript.util.ByteString.byteString;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

import net.zscript.javaclient.tokens.ExtendingTokenBuffer;
import net.zscript.tokenizer.Tokenizer;

public class ZscriptFieldSetTest {
    ZscriptFieldSet fieldSet = ZscriptFieldSet.fromMap(List.of(new byte[] { 'a', 'b' }, new byte[] { 'c', 'd', 'e' }), List.of(false, true),
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
    public void shouldFailToInitializeFromUnequalLists() {
        assertThatThrownBy(() -> ZscriptFieldSet.fromMap(List.of(new byte[] { 'b' }), List.of(false, true), emptyMap())).isInstanceOf(IllegalArgumentException.class);
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
        assertThat(ZscriptFieldSet.fromMap(emptyList(), emptyList(), Map.of((byte) 'Z', 4)).isEmpty()).isFalse();
    }

    @Test
    public void shouldDetermineBufferLength() {
        assertThat(fieldSet.getBufferLength()).isEqualTo(21);
        assertThat(ZscriptFieldSet.blank().getBufferLength()).isEqualTo(0);
    }

    @Test
    public void shouldCreateFromTokens() {
        ExtendingTokenBuffer buf = new ExtendingTokenBuffer();
        Tokenizer            tok = new Tokenizer(buf.getTokenWriter(), 2);
        byteString("Z7Ac+6162\"cde\"", ISOLATIN1_APPENDER).forEach(tok::accept);
        ZscriptFieldSet zfs = ZscriptFieldSet.fromTokens(buf.getTokenReader().getFirstReadToken());
        assertThat(byteString(zfs).asString()).isEqualTo("Z7Ac+6162\"cde\"");
    }
}

package net.zscript.javaclient.commandbuilder;

import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

import net.zscript.javaclient.ZscriptByteString;
import net.zscript.javaclient.ZscriptByteString.ZscriptByteStringBuilder;
import net.zscript.model.components.Zchars;

class ZscriptByteStringTest {
    @Test
    public void shouldAppendText() {
        assertThat(ZscriptByteString.builder().appendBigfieldText("Hello").toByteArray())
                .containsExactly('"', 'H', 'e', 'l', 'l', 'o', '"');
    }

    @Test
    public void shouldAppendTextWithEscapes() {
        assertThat(ZscriptByteString.builder().appendBigfieldText("He=\no").toByteArray())
                .containsExactly('"', 'H', 'e', '=', '3', 'd', '=', '0', 'a', 'o', '"');
    }

    @Test
    public void shouldAppendBytes() {
        assertThat(ZscriptByteString.builder().appendBigfieldBytes("ab").toByteArray())
                .containsExactly('+', '6', '1', '6', '2');
    }

    @Test
    public void shouldWriteHex() {
        ZscriptByteStringBuilder out = ZscriptByteString.builder();
        assertThat(out.appendHexPair(0).toByteArray()).containsExactly('0', '0');
        assertThat(out.appendHexPair((byte) 0x3d).toByteArray()).containsExactly('0', '0', '3', 'd');
        assertThat(out.appendHexPair((byte) 0xff).toByteArray()).containsExactly('0', '0', '3', 'd', 'f', 'f');
    }

    @Test
    public void shouldAppendField() {
        assertThat(ZscriptByteString.builder().appendField(Zchars.Z_CMD, 0x4a5b).toByteArray())
                .containsExactly('Z', '4', 'a', '5', 'b');
        assertThat(ZscriptByteString.builder().appendField(Zchars.Z_CMD, 0).toByteArray())
                .containsExactly('Z');
    }

    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ZscriptByteString.class).verify();
    }

}

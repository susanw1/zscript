package net.zscript.javaclient.commandbuilder;

import static org.assertj.core.api.Assertions.assertThat;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;

import net.zscript.model.components.Zchars;

class ZscriptByteStringTest {
    @Test
    public void shouldAppendText() {
        assertThat(ZscriptByteString.builder().appendText("Hello").toByteArray())
                .containsExactly('"', 'H', 'e', 'l', 'l', 'o', '"');
    }

    @Test
    public void shouldAppendTextWithEscapes() {
        assertThat(ZscriptByteString.builder().appendText("He=\no").toByteArray())
                .containsExactly('"', 'H', 'e', '=', '3', 'd', '=', '0', 'a', 'o', '"');
    }

    @Test
    public void shouldAppendBytes() {
        assertThat(ZscriptByteString.builder().appendBytes("ab").toByteArray())
                .containsExactly('+', '6', '1', '6', '2');
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

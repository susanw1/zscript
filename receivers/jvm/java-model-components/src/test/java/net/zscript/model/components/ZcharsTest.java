package net.zscript.model.components;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZcharsTest {
    @Test
    void shouldHexConvert() {
        for (char i = 0; i < 257; i++) {
            final int  ch       = Character.digit(i, 16);
            final byte expected = (byte) (ch == -1 || Character.isUpperCase(i) ? 0x10 : ch);
            assertThat(Zchars.parseHex((byte) i)).as("char " + i + " (asc " + (int) i + ")").isEqualTo(expected);
        }
    }

    @Test
    public void shouldCheckZchars() {
        assertThat(Zchars.mustStringEscape((byte) '=')).isTrue();
        assertThat(Zchars.mustStringEscape((byte) '\n')).isTrue();
        assertThat(Zchars.mustStringEscape((byte) '\0')).isTrue();
        assertThat(Zchars.mustStringEscape((byte) '"')).isTrue();
        assertThat(Zchars.mustStringEscape((byte) 'a')).isFalse();
        assertThat(Zchars.mustStringEscape((byte) ' ')).isFalse();
    }
}

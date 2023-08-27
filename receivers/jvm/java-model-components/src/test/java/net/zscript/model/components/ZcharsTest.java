package net.zscript.model.components;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZcharsTest {
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

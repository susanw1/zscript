package net.zscript.javareceiver.tokenizer;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

class ZcharsTest {

    @Test
    void shouldHexConvert() {
        for (char i = 0; i < 257; i++) {
            final int  ch       = Character.digit(i, 16);
            final byte expected = (byte) (ch == -1 || Character.isUpperCase(i) ? 0x10 : ch);
            assertThat(Zchars.parseHex((byte) i)).as("char " + i + " (asc " + (int) i + ")").isEqualTo(expected);
        }
    }

}

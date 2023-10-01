package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import org.junit.jupiter.api.Test;

class ByteStringTest {
    @Test
    public void shouldAppendBytes() throws IOException {
        ByteString.ByteStringBuilder strBuilder = ByteString.builder().appendByte(0x61).appendByte('b');
        ByteString                   str        = strBuilder.build();
        assertThat(str.toByteArray()).containsExactly('a', 'b');

        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
        assertThat(str.writeTo(byteArrayOutputStream)).isSameAs(str);

        ByteArrayOutputStream byteArrayOutputStream2 = new ByteArrayOutputStream();
        strBuilder.writeTo(byteArrayOutputStream2);
        assertThat(strBuilder.toByteArray()).containsExactly('a', 'b');
        assertThat(byteArrayOutputStream2.toByteArray()).containsExactly('a', 'b');
    }

    @Test
    public void shouldWriteToByteArrayNumbers() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0x1a2b).toByteArray())
                .containsExactly('Z', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldWriteNumberZero() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0).toByteArray())
                .containsExactly('Z');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumerValue() {
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric(123456).toByteArray())
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldWriteHex() {
        ByteString.ByteStringBuilder builder = ByteString.builder();
        assertThat(builder.appendHexValue(0).toByteArray()).containsExactly('0', '0');
        assertThat(builder.appendHexValue(0x3d).toByteArray()).containsExactly('0', '0', '3', 'd');
        assertThat(builder.appendHexValue(0xff).toByteArray()).containsExactly('0', '0', '3', 'd', 'f', 'f');
    }
}

package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import net.zscript.util.ByteString.ImmutableByteString;

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

        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendByte(257).toByteArray())
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldInitializeNewBuilderWithExistingContent() {
        ByteString.ByteStringBuilder b = ByteString.builder().appendByte('Z').appendNumeric(0x01a2);
        assertThat(ByteString.builder(b).appendByte('X').appendNumeric(0x1a2b).toByteArray())
                .containsExactly('Z', '1', 'a', '2', 'X', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldAppendExistingContent() {
        ByteString.ByteStringBuilder b = ByteString.builder().appendByte('Z').appendNumeric(0xa2);
        assertThat(ByteString.builder().appendByte('X').append(b).appendByte('Y').toByteArray())
                .containsExactly('X', 'Z', 'a', '2', 'Y');
        assertThat(ByteString.builder().appendByte('X').append(b.build()).appendByte('Y').toByteArray())
                .containsExactly('X', 'Z', 'a', '2', 'Y');
    }

    @Test
    public void shouldWriteToByteArrayNumbers() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0x1a2b).toByteArray())
                .containsExactly('Z', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldOmitNumberZero() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0).toByteArray())
                .containsExactly('Z');
    }

    @Test
    public void shouldWriteNumberZero() {
        assertThat(ByteString.builder().appendByte('Z').appendNumericKeepZero(0).toByteArray())
                .containsExactly('Z', '0');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumericValue() {
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric(123456).toByteArray())
                .isInstanceOf(IllegalArgumentException.class);
    }

    @ParameterizedTest
    @CsvSource({ "0xf1e2d3c4,Zf1e2d3c4", "0xe2d3c4,Ze2d3c4", "0x3c4,Z3c4", "0x10000,Z10000", "0xffff,Zffff", "0,Z" })
    public void shouldWriteToByteArrayNumbers32(long value, String expected) {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32(value).toByteArray())
                .containsExactly(expected.getBytes(StandardCharsets.ISO_8859_1));
    }

    @Test
    public void shouldOmitNumberZero32() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32(0).toByteArray())
                .containsExactly('Z');
    }

    @Test
    public void shouldWriteNumberZero32() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32KeepZero(0).toByteArray())
                .containsExactly('Z', '0');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumericValue32() {
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric32(0x123456789L).toByteArray())
                .isInstanceOf(IllegalArgumentException.class);
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric32(-1).toByteArray())
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldWriteHex() {
        ByteString.ByteStringBuilder builder = ByteString.builder();
        assertThat(builder.appendHexPair(0).toByteArray()).containsExactly('0', '0');
        assertThat(builder.appendHexPair(0x3d).toByteArray()).containsExactly('0', '0', '3', 'd');
        assertThat(builder.appendHexPair(0xff).toByteArray()).containsExactly('0', '0', '3', 'd', 'f', 'f');

        assertThatThrownBy(() -> builder.appendHexPair(123456))
                .isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ImmutableByteString.class).verify();
    }

    @Test
    public void toStringContract() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0x12).build().toString())
                .isEqualTo("ImmutableByteString[Z12]");
    }

    @Test
    public void builderToStringContract() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric(0x12).toString())
                .isEqualTo("ByteStringBuilder[Z12]");
    }
}

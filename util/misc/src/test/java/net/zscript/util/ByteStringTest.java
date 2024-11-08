package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.NoSuchElementException;

import static java.nio.charset.StandardCharsets.ISO_8859_1;
import static net.zscript.util.ByteString.ByteAppender.ISOLATIN1_APPENDER;
import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatIndexOutOfBoundsException;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

import nl.jqno.equalsverifier.EqualsVerifier;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import net.zscript.util.ByteString.ByteAppendable;

class ByteStringTest {
    @Test
    public void shouldAppendBytes() throws IOException {
        var strBuilder = ByteString.builder().appendByte(0x61).appendByte('b');
        var str        = strBuilder.build();
        assertThat(str.toByteArray()).containsExactly('a', 'b');
        assertThat(str.size()).isEqualTo(2);

        var byteArrayOutputStream = new ByteArrayOutputStream();
        assertThat(str.writeTo(byteArrayOutputStream)).isSameAs(str);

        var byteArrayOutputStream2 = new ByteArrayOutputStream();
        strBuilder.writeTo(byteArrayOutputStream2);
        assertThat(strBuilder.toByteArray()).containsExactly('a', 'b');
        assertThat(byteArrayOutputStream2.toByteArray()).containsExactly('a', 'b');

        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendByte(257).toByteArray()).isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldAppendRaw() {
        var str = ByteString.builder()
                .appendRaw(new byte[] { 'a', 'b' })
                .appendRaw(new byte[] { 'p', 'q', 'r', 's', 't' }, 1, 3)
                .build();

        assertThat(str.asString()).isEqualTo("abqrs");
    }

    @Test
    public void shouldAccessBytes() {
        var strBuilder = ByteString.builder().appendByte(0x61).appendByte('b');
        assertThat(strBuilder.size()).isEqualTo(2);
        final ByteString str = strBuilder.build();
        assertThat(str.get(0)).isEqualTo((byte) 97);
        assertThat(str.get(1)).isEqualTo((byte) 98);
        assertThatIndexOutOfBoundsException().isThrownBy(() -> str.get(2));
    }

    @Test
    public void shouldIterateBytes() {
        var str  = ByteString.builder().appendByte(0x61).appendByte('b').build();
        var iter = str.iterator();
        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.next()).isEqualTo((byte) 97);
        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.next()).isEqualTo((byte) 98);
        assertThat(iter.hasNext()).isFalse();
        assertThatThrownBy(iter::next).isInstanceOf(NoSuchElementException.class);
    }

    @Test
    public void shouldIterateBlockBytes() {
        var str  = ByteString.builder().appendUtf8("hello, London calling, £5 is a fiver").build();
        var iter = str.iterator(6);

        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.next()).isEqualTo((byte) 'h');
        assertThat(iter.nextContiguous(8)).isEqualTo("ello, Lo".getBytes(ISO_8859_1));
        assertThat(iter.nextContiguous(11)).isEqualTo("ndon callin".getBytes(ISO_8859_1));
        assertThat(iter.nextContiguous()).containsExactly('g', ',', ' ', 0xc2, 0xa3, '5');
        assertThat(iter.hasNext()).isTrue();
        assertThat(iter.nextContiguous()).isEqualTo(" is a ".getBytes(ISO_8859_1));
        assertThat(iter.nextContiguous()).isEqualTo("fiver".getBytes(ISO_8859_1));
        assertThat(iter.hasNext()).isFalse();
    }

    @Test
    public void shouldInitializeNewBuilderWithExistingContent() {
        var b = ByteString.builder().appendByte('Z').appendNumeric16(0x01a2);
        assertThat(ByteString.builder(b.asAppendable()).appendByte('X').appendNumeric16(0x1a2b).toByteArray()).containsExactly('Z', '1', 'a', '2', 'X', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldInitializeNewBuilderWithMultipleExistingContent() {
        var b1 = ByteString.builder().appendByte('Z').appendNumeric16(0x02);
        var b2 = ByteString.builder().appendByte('Y').appendNumeric16(0x03);
        assertThat(ByteString.builder(b1.asAppendable(), b2.asAppendable()).appendByte('X').appendNumeric16(0x1a2b).asString()).isEqualTo("Z2Y3X1a2b");
    }

    @Test
    public void shouldAppendExistingContent() {
        var b = ByteString.builder().appendByte('Z').appendNumeric16(0xa2);
        assertThat(ByteString.builder().appendByte('X').append(b.asAppendable()).appendByte('Y').toByteArray()).containsExactly('X', 'Z', 'a', '2', 'Y');
        assertThat(ByteString.builder().appendByte('X').append(b.build()).appendByte('Y').toByteArray()).containsExactly('X', 'Z', 'a', '2', 'Y');
    }

    @Test
    public void shouldAppendAppendables() {
        class TestAppendable implements ByteAppendable {
            final int x;

            TestAppendable(int x) {
                this.x = x;
            }

            @Override
            public void appendTo(ByteString.ByteStringBuilder builder) {
                builder.appendUtf8("x=").appendHexPair(x);
            }
        }

        assertThat(ByteString.builder(new TestAppendable(1)).build().asString()).isEqualTo("x=01");
        assertThat(ByteString.from(new TestAppendable(1)).asString()).isEqualTo("x=01");

        assertThat(ByteString.builder(new TestAppendable(1), new TestAppendable(2)).build().asString()).isEqualTo("x=01x=02");
        assertThat(ByteString.concat(new TestAppendable(1), new TestAppendable(2)).asString()).isEqualTo("x=01x=02");
        assertThat(ByteString.builder().append(new TestAppendable(1), new TestAppendable(2)).asString()).isEqualTo("x=01x=02");

        assertThat(ByteString.builder(List.of(new TestAppendable(3), new TestAppendable(4))).build().asString()).isEqualTo("x=03x=04");
        assertThat(ByteString.concat(List.of(new TestAppendable(3), new TestAppendable(4))).asString()).isEqualTo("x=03x=04");
        assertThat(ByteString.builder().append(List.of(new TestAppendable(1), new TestAppendable(2))).asString()).isEqualTo("x=01x=02");

        assertThat(new TestAppendable(6).toByteString().asString()).isEqualTo("x=06");
    }

    @Test
    public void shouldCreateFromTypes() {
        assertThat(ByteString.from((byte) 'a').toByteArray()).containsExactly('a');
        assertThat(ByteString.from(new byte[] { 'a', 'b' }).toByteArray()).containsExactly('a', 'b');

        assertThat(ByteString.concat(ISOLATIN1_APPENDER, "hello", " ", "world").asString()).isEqualTo("hello world");
        assertThat(ByteString.concat(ISOLATIN1_APPENDER, List.of("hello", " ", "world")).asString()).isEqualTo("hello world");

        assertThat(ISOLATIN1_APPENDER.toByteString("Hello")).isEqualTo(ByteString.builder().appendUtf8("Hello").build());
    }

    @Test
    public void shouldWriteToByteArrayNumbers() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric16(0x1a2b).toByteArray()).containsExactly('Z', '1', 'a', '2', 'b');
    }

    @Test
    public void shouldOmitNumberZero() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric16(0).toByteArray()).containsExactly('Z');
    }

    @Test
    public void shouldWriteNumberZero() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric16KeepZero(0).toByteArray()).containsExactly('Z', '0');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumericValue() {
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric16(123456).toByteArray()).isInstanceOf(IllegalArgumentException.class);
    }

    @ParameterizedTest
    @CsvSource({ "0xf1e2d3c4,Zf1e2d3c4", "0xe2d3c4,Ze2d3c4", "0x3c4,Z3c4", "0x10000,Z10000", "0xffff,Zffff", "0,Z" })
    public void shouldWriteToByteArrayNumbers32(long value, String expected) {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32(value).toByteArray()).containsExactly(expected.getBytes(ISO_8859_1));
    }

    @Test
    public void shouldOmitNumberZero32() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32(0).toByteArray()).containsExactly('Z');
    }

    @Test
    public void shouldWriteNumberZero32() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric32KeepZero(0).toByteArray()).containsExactly('Z', '0');
    }

    @Test
    public void shouldThrowOnOutOfRangeNumericValue32() {
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric32(0x123456789L).toByteArray()).isInstanceOf(IllegalArgumentException.class);
        assertThatThrownBy(() -> ByteString.builder().appendByte('Z').appendNumeric32(-1).toByteArray()).isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldWriteHex() {
        var builder = ByteString.builder();
        assertThat(builder.appendHexPair(0).toByteArray()).containsExactly('0', '0');
        assertThat(builder.appendHexPair(0x3d).toByteArray()).containsExactly('0', '0', '3', 'd');
        assertThat(builder.appendHexPair(0xff).toByteArray()).containsExactly('0', '0', '3', 'd', 'f', 'f');

        assertThatThrownBy(() -> builder.appendHexPair(123456)).isInstanceOf(IllegalArgumentException.class);
    }

    @Test
    public void shouldResetBuilder() {
        var builder = ByteString.builder();
        assertThat(builder.appendHexPair(0).toByteArray()).containsExactly('0', '0');
        builder.reset();
        assertThat(builder.size()).isZero();
        assertThat(builder.toByteArray()).isEmpty();
        assertThat(builder.appendHexPair(0x21).toByteArray()).containsExactly('2', '1');
    }

    @Test
    public void equalsContract() {
        EqualsVerifier.forClass(ByteString.class).verify();
    }

    @Test
    public void toStringContract() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric16(0x12).build().toString()).isEqualTo("ByteString[\"Z12\"]");
    }

    @Test
    public void builderToStringContract() {
        assertThat(ByteString.builder().appendByte('Z').appendNumeric16(0x12).toString()).isEqualTo("ByteStringBuilder[\"Z12\"]");
    }
}

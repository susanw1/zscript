package net.zscript.javareceiver.core;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import net.zscript.javareceiver.testing.StringWriterOutStream;

public class AbstractOutStreamTest {
    StringWriterOutStream stOut;

    @BeforeEach
    void setup() throws IOException {
        stOut = new StringWriterOutStream();
    }

    private char toHexChar(byte v) {
        if (v < 10) {
            return (char) ('0' + v);
        }
        return (char) ('a' + v - 10);
    }

    @Test
    void shouldWriteCloseParen() {
        stOut.writeCloseParen();
        assertThat(stOut.getString()).isEqualTo(")");
    }

    @Test
    void shouldWriteOpenParen() {
        stOut.writeOpenParen();
        assertThat(stOut.getString()).isEqualTo("(");
    }

    @Test
    void shouldWriteAnd() {
        stOut.writeAndThen();
        assertThat(stOut.getString()).isEqualTo("&");
    }

    @Test
    void shouldWriteOr() {
        stOut.writeOrElse();
        assertThat(stOut.getString()).isEqualTo("|");
    }

    @Test
    void shouldWriteEndOfSeq() {
        stOut.endSequence();
        assertThat(stOut.getString()).isEqualTo("\n");
    }

    @Test
    void shouldWriteAnyLengthField() {
        stOut.writeField('A', 0);
        stOut.writeField('B', 0x1);
        stOut.writeField('C', 0x10);
        stOut.writeField('D', 0x100);
        stOut.writeField('E', 0x1000);
        assertThat(stOut.getString()).isEqualTo("AB1C10D100E1000");
    }

    @Test
    void shouldWriteStringField() {
        byte[]        data     = new byte[256];
        StringBuilder expected = new StringBuilder();
        expected.append('X');
        for (byte i = 0; i < 16; i++) {
            for (byte j = 0; j < 16; j++) {
                data[i * 16 + j] = (byte) (i * 16 + j);
                expected.append(toHexChar(i));
                expected.append(toHexChar(j));
            }
        }
        stOut.writeFieldHex('X', data);
        assertThat(stOut.getString()).isEqualTo(expected.toString());
    }

    @Test
    void shouldWriteStringFieldWithBytes() {
        byte[]        data     = new byte[256];
        StringBuilder expected = new StringBuilder();
        expected.append("X\"");
        for (int i = 0; i < 256; i++) {
            if (i == '\0') {
                expected.append("=00");
            } else if (i == '\n') {
                expected.append("=0a");
            } else if (i == '"') {
                expected.append("=22");
            } else if (i == '=') {
                expected.append("=3d");
            } else {
                expected.append((char) Byte.toUnsignedInt((byte) i));
            }
            data[i] = (byte) i;
        }
        expected.append('"');
        stOut.writeFieldQuoted('X', data);
        assertThat(stOut.getString()).isEqualTo(expected.toString());
    }

    @Test
    void shouldWriteStringFieldWithHiByte() {
        // 0xb5 is the 'µ' symbol, but we expect literally 0xb5, not utf-8's 0xc2b5
        stOut.writeFieldQuoted('X', new byte[] { (byte) 0xb5 });
        // note: ISO_8859_1 handily takes bottom byte of each char. We're not really using it.
        assertThat(stOut.getString().getBytes(StandardCharsets.ISO_8859_1)).hasSize(4).containsExactly('X', '"', 0xb5, '"');
    }

    @Test
    void shouldWriteStringFieldWithStringContainingHiByte() {
        // 0xb5 is the 'µ' symbol, and writing strings should convert to UTF-8's 0xc2b5
        stOut.writeFieldQuoted('X', "µ");
        // note: ISO_8859_1 handily takes bottom byte of each char. We're not really using it.
        assertThat(stOut.getString().getBytes(StandardCharsets.ISO_8859_1)).hasSize(5).containsExactly('X', '"', 0xc2, 0xb5, '"');
    }

    @Test
    void shouldWriteStringFieldWithString() {
        stOut.writeFieldQuoted('X', "Hello World");
        assertThat(stOut.getString()).isEqualTo("X\"Hello World\"");
    }

    @Test
    void shouldWriteZscriptField() {
        // This is really hard/annoying to test, and it's only used in the echo command. But we want to test it - we just had a stupid bug in it. Let's just write comprehensive
        // parser tests around this.
    }

}

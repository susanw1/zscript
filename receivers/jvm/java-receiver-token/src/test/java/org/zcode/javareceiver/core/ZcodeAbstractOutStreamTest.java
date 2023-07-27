package org.zcode.javareceiver.core;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZcodeAbstractOutStreamTest {
    private char toHexChar(byte v) {
        if (v < 10) {
            return (char) ('0' + v);
        }
        return (char) ('a' + v - 10);
    }

    @Test
    void shouldWriteCloseParen() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeCloseParen();
        assertThat(stOut.getString()).isEqualTo(")");
    }

    @Test
    void shouldWriteOpenParen() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeOpenParen();
        assertThat(stOut.getString()).isEqualTo("(");
    }

    @Test
    void shouldWriteAnd() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeAndThen();
        assertThat(stOut.getString()).isEqualTo("&");
    }

    @Test
    void shouldWriteOr() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeOrElse();
        assertThat(stOut.getString()).isEqualTo("|");
    }

    @Test
    void shouldWriteEndOfSeq() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.endSequence();
        assertThat(stOut.getString()).isEqualTo("\n");
    }

    @Test
    void shouldWriteAnyLengthField() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeField('A', 0);
        stOut.writeField('B', 0x1);
        stOut.writeField('C', 0x10);
        stOut.writeField('D', 0x100);
        stOut.writeField('E', 0x1000);
        assertThat(stOut.getString()).isEqualTo("AB1C10D100E1000");
    }

    @Test
    void shouldWriteBigField() {
        StringBuilderOutStream stOut    = new StringBuilderOutStream();
        byte[]         data     = new byte[256];
        StringBuilder  expected = new StringBuilder();
        expected.append('+');
        for (byte i = 0; i < 16; i++) {
            for (byte j = 0; j < 16; j++) {
                data[i * 16 + j] = (byte) (i * 16 + j);
                expected.append(toHexChar(i));
                expected.append(toHexChar(j));
            }
        }
        stOut.writeBig(data);
        assertThat(stOut.getString()).isEqualTo(expected.toString());
    }

    @Test
    void shouldWriteStringField() {
        StringBuilderOutStream stOut    = new StringBuilderOutStream();
        byte[]         data     = new byte[256];
        StringBuilder  expected = new StringBuilder();
        expected.append('"');
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
                expected.append((char) (byte) i);
            }
            data[i] = (byte) i;
        }
        expected.append('"');
        stOut.writeString(data);
        assertThat(stOut.getString()).isEqualTo(expected.toString());
    }

    @Test
    void shouldWriteStringFieldAsString() {
        StringBuilderOutStream stOut = new StringBuilderOutStream();
        stOut.writeString("Hello World");
        assertThat(stOut.getString()).isEqualTo("\"Hello World\"");
    }
}

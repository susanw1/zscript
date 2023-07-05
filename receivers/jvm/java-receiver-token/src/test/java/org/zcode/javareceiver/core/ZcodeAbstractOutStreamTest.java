package org.zcode.javareceiver.core;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.Test;

public class ZcodeAbstractOutStreamTest {
    private class ZcodeStringOut extends ZcodeAbstractOutStream {
        StringBuilder str = new StringBuilder();

        @Override
        public void open() {
        }

        @Override
        public void close() {
        }

        @Override
        public boolean isOpen() {
            return false;
        }

        @Override
        public void writeByte(byte b) {
            str.append((char) b);
        }

        public String getString() {
            return str.toString();
        }
    }

    private char toHexChar(byte v) {
        if (v < 10) {
            return (char) ('0' + v);
        } else {
            return (char) ('a' + v - 10);
        }
    }

    @Test
    void shouldWriteCloseParen() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeCloseParen();
        assertThat(stOut.getString()).isEqualTo(")");
    }

    @Test
    void shouldWriteOpenParen() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeOpenParen();
        assertThat(stOut.getString()).isEqualTo("(");
    }

    @Test
    void shouldWriteAnd() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeAndThen();
        assertThat(stOut.getString()).isEqualTo("&");
    }

    @Test
    void shouldWriteOr() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeOrElse();
        assertThat(stOut.getString()).isEqualTo("|");
    }

    @Test
    void shouldWriteEndOfSeq() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.endSequence();
        assertThat(stOut.getString()).isEqualTo("\n");
    }

    @Test
    void shouldWriteAnyLengthField() {
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeField('A', 0);
        stOut.writeField('B', 0x1);
        stOut.writeField('C', 0x10);
        stOut.writeField('D', 0x100);
        stOut.writeField('E', 0x1000);
        assertThat(stOut.getString()).isEqualTo("AB1C10D100E1000");
    }

    @Test
    void shouldWriteBigField() {
        ZcodeStringOut stOut    = new ZcodeStringOut();
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
        ZcodeStringOut stOut    = new ZcodeStringOut();
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
        ZcodeStringOut stOut = new ZcodeStringOut();
        stOut.writeString("Hello World");
        assertThat(stOut.getString()).isEqualTo("\"Hello World\"");
    }
}

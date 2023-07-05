package org.zcode.javareceiver.core;

import java.nio.charset.StandardCharsets;

import org.zcode.javareceiver.execution.ZcodeField;
import org.zcode.javareceiver.tokenizer.Zchars;

public abstract class ZcodeAbstractOutStream implements ZcodeOutStream {

    private byte toHex(byte n) {
        int nibble = Byte.toUnsignedInt(n);
        if (nibble < 10) {
            return (byte) (nibble + '0');
        } else {
            return (byte) (nibble + 'a' - 10);
        }
    }

    private void writeHex(byte b) {
        writeByte(toHex((byte) ((b >> 4) & 0xF)));
        writeByte(toHex((byte) (b & 0xF)));
    }

    private boolean writeHexTrim(byte b) {
        if ((b & 0xF0) != 0) {
            writeByte(toHex((byte) ((b >> 4) & 0xF)));
            writeByte(toHex((byte) (b & 0xF)));
            return true;
        } else if (b != 0) {
            writeByte(toHex((byte) (b & 0xF)));
            return true;
        } else {
            return false;
        }
    }

    private void writeStringByte(byte b) {
        if (b == '\n') {
            writeByte((byte) '=');
            writeByte((byte) '0');
            writeByte((byte) 'a');
        } else if (b == '"') {
            writeByte((byte) '=');
            writeByte((byte) '2');
            writeByte((byte) '2');
        } else if (b == '=') {
            writeByte((byte) '=');
            writeByte((byte) '3');
            writeByte((byte) 'd');
        } else if (b == '\000') {
            writeByte((byte) '=');
            writeByte((byte) '0');
            writeByte((byte) '0');
        } else {
            writeByte(b);
        }
    }

    @Override
    public void endSequence() {
        writeByte((byte) '\n');
    }

    @Override
    public void writeField(byte field, int value) {
        writeByte(field);
        if (writeHexTrim((byte) (value >> 8))) {
            writeHex((byte) (value & 0xFF));
        } else {
            writeHexTrim((byte) (value & 0xFF));
        }
    }

    @Override
    public void writeField(char field, int value) {
        writeField((byte) field, value);
    }

    @Override
    public void writeField(ZcodeField field) {
        if (field.isBigField()) {
            if (field.getKey() == Zchars.Z_BIGFIELD_QUOTED) {
                writeByte((byte) '"');
                for (byte b : field) {
                    writeStringByte(b);
                }
                writeByte((byte) '"');
            } else {
                writeByte((byte) '+');
                for (byte b : field) {
                    writeHex(b);
                }
            }
        } else {
            writeField(field.getKey(), field.getValue());
        }
    }

    @Override
    public void writeString(String string) {
        writeString(string.getBytes(StandardCharsets.UTF_8));
    }

    @Override
    public void writeString(byte[] data) {
        writeByte((byte) '"');
        for (byte b : data) {
            writeStringByte(b);
        }
        writeByte((byte) '"');
    }

    @Override
    public void writeBig(byte[] data) {
        writeByte((byte) '+');
        for (byte b : data) {
            writeHex(b);
        }
    }

    @Override
    public void writeOrElse() {
        writeByte((byte) '|');
    }

    @Override
    public void writeAndThen() {
        writeByte((byte) '&');
    }

    @Override
    public void writeOpenParen() {
        writeByte((byte) '(');
    }

    @Override
    public void writeCloseParen() {
        writeByte((byte) ')');
    }
}

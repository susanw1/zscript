package org.zcode.javareceiver.core;

import java.nio.charset.StandardCharsets;

import org.zcode.javareceiver.execution.ZcodeField;
import org.zcode.javareceiver.tokenizer.Zchars;

public abstract class ZcodeAbstractOutStream implements ZcodeOutStream {

    @Override
    public abstract void writeByte(byte b);

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

    @Override
    public void endSequence() {
        writeByte((byte) '\n');
    }

    @Override
    public void writeField(byte field, int value) {
        writeByte(field);
        if (value >= (1 << 12)) {
            writeHex((byte) (value >> 8));
            writeHex((byte) (value & 0xFF));
        } else if (value >= (1 << 8)) {
            writeByte(toHex((byte) ((value >> 8) & 0xF)));
            writeHex((byte) (value & 0xFF));
        } else if (value >= (1 << 4)) {
            writeHex((byte) (value & 0xFF));
        } else if (value > 0) {
            writeByte(toHex((byte) (value & 0xF)));
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
                    writeByte(b);
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
    public void silentSucceed() {
    }

    @Override
    public void writeString(String string) {
        writeByte((byte) '"');
        for (byte b : string.getBytes(StandardCharsets.UTF_8)) {
            writeByte(b);
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
    public void startField(byte key) {
        writeByte(key);
    }

    @Override
    public void continueField(byte next) {
        writeHex(next);
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

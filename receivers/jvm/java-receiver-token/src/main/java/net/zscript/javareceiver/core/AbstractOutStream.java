package net.zscript.javareceiver.core;

import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.execution.ZscriptField;
import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.Zchars;

public abstract class AbstractOutStream implements OutStream, ZscriptCommandOutStream {
    /**
     * @param bytes   the bytes to write
     * @param count   how many of the bytes to write
     * @param hexMode if false, write the bytes as they are (eg byte 0x6d is written as 'm'); if true, write them as hex-pairs, eg byte 0x6d is written as '6','d'
     */
    protected abstract void writeBytes(byte[] bytes, int count, boolean hexMode);

    private void writeCharAsByte(int c) {
        writeBytes(new byte[] { (byte) c }, 1, false);
    }

    /**
     * Finds the ascii char representation of the bottom nibble of {@code b}. Watch out - this returns a byte, so it must be cast to a char to use it in certain contexts, such as
     * StringBuilders or String '+' operations.
     *
     * @param b the byte to convert (ignores top nibble)
     * @return a byte in range '0'-'9' or 'a'-'f'
     */
    protected static final byte toHexChar(int b) {
        int lowNibble = b & 0xf;
        return (byte) (lowNibble < 10 ? lowNibble + '0' : lowNibble + ('a' - 10));
    }

    private void appendHex(byte b, byte[] buffer, int index) {
        buffer[index] = toHexChar(b >>> 4);
        buffer[index + 1] = toHexChar(b & 0xf);
    }

    private int appendHexTrim(byte b, byte[] buffer, int index) {
        if ((b & 0xF0) != 0) {
            buffer[index] = toHexChar(b >>> 4);
            buffer[index + 1] = toHexChar(b & 0xf);
            return 2;
        }
        if (b != 0) {
            buffer[index] = toHexChar(b & 0xf);
            return 1;
        }
        return 0;
    }

    /**
     * Convenience method that write the bytes stored in the supplied string.
     */
    private void writeEscaped(byte b) {
        writeBytes(new byte[] { '=', toHexChar(b >>> 4), toHexChar(b & 0xf) }, 3, false);
    }

    private void writeStringByte(byte b) {
        switch (b) {
        case '\n':
        case '=':
        case '"':
        case '\0':
            writeEscaped(b);
            break;
        default:
            writeCharAsByte(b);
        }
    }

    @Override
    public void endSequence() {
        writeCharAsByte('\n');
    }

    @Override
    public void writeBytes(byte[] bytes) {
        writeBytes(bytes, bytes.length, false);
    }

    @Override
    public void writeField(byte field, int value) {
        byte[] bytes = new byte[5];
        bytes[0] = field;

        int offset = appendHexTrim((byte) (value >>> 8), bytes, 1);
        if (offset > 0) {
            appendHex((byte) (value & 0xFF), bytes, offset + 1);
            offset += 2;
        } else {
            offset += appendHexTrim((byte) (value & 0xFF), bytes, offset + 1);
        }
        writeBytes(bytes, offset + 1, false);
    }

    @Override
    public void writeField(ZscriptField field) {
        if (field.isBigField()) {
            if (field.getKey() == Zchars.Z_BIGFIELD_QUOTED) {
                writeCharAsByte('"');
                // note, could use BlockIterator, but need to split and extract escaped chars
                for (byte b : field) {
                    writeStringByte(b);
                }
                writeCharAsByte('"');
            } else {
                writeCharAsByte('+');
                for (BlockIterator iterator = field.iterator(); iterator.hasNext();) {
                    byte[] bytes = iterator.nextContiguous();
                    writeBytes(bytes, bytes.length, true);
                }
            }
        } else {
            writeField(field.getKey(), field.getValue());
        }
    }

    @Override
    public void writeQuotedString(String string) {
        writeQuotedString(string.getBytes(StandardCharsets.UTF_8));
    }

    @Override
    public void writeQuotedString(byte[] data) {
        writeCharAsByte('"');
        for (byte b : data) {
            writeStringByte(b);
        }
        writeCharAsByte('"');
    }

    @Override
    public void writeBig(byte[] data) {
        writeCharAsByte('+');
        writeBytes(data, data.length, true);
    }

    @Override
    public void writeOrElse() {
        writeCharAsByte('|');
    }

    @Override
    public void writeAndThen() {
        writeCharAsByte('&');
    }

    @Override
    public void writeOpenParen() {
        writeCharAsByte('(');
    }

    @Override
    public void writeCloseParen() {
        writeCharAsByte(')');
    }
}

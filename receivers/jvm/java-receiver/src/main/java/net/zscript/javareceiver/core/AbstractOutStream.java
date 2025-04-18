package net.zscript.javareceiver.core;

import java.nio.charset.StandardCharsets;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.BlockIterator;

public abstract class AbstractOutStream implements OutStream {
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
     * StringBuilders or String concat operations.
     *
     * @param b the byte to convert (ignores top nibble)
     * @return a byte in range '0'-'9' or 'a'-'f'
     */
    protected static byte toHexChar(int b) {
        int lowNibble = b & 0xf;
        return (byte) (lowNibble < 10 ? lowNibble + '0' : lowNibble + ('a' - 10));
    }

    private void appendHex(byte b, byte[] buffer, int index) {
        buffer[index] = toHexChar(b >>> 4);
        buffer[index + 1] = toHexChar(b & 0xf);
    }

    /**
     * Utility that writes the supplied byte to the supplied buffer as 0, 1, or 2 hex digits.
     *
     * @param b          the byte to write
     * @param buffer     the buffer to write to
     * @param startIndex the start index within the buffer to begin writing
     * @return the number of digits written, to facilitate advancing the index
     */
    private int appendHexTrim(byte b, byte[] buffer, int startIndex) {
        if ((b & 0xF0) != 0) {
            buffer[startIndex] = toHexChar(b >>> 4);
            buffer[startIndex + 1] = toHexChar(b & 0xf);
            return 2;
        }
        if (b != 0) {
            buffer[startIndex] = toHexChar(b & 0xf);
            return 1;
        }
        return 0;
    }

    /**
     * Convenience method that write the bytes stored in the supplied string.
     */
    private void writeEscaped(byte b) {
        writeBytes(new byte[] { Zchars.Z_STRING_ESCAPE, toHexChar(b >>> 4), toHexChar(b & 0xf) }, 3, false);
    }

    private void writeStringByte(byte b) {
        if (Zchars.mustStringEscape(b)) {
            writeEscaped(b);
        } else {
            writeCharAsByte(b);
        }
    }

    @Override
    public void endSequence() {
        writeCharAsByte(Zchars.Z_EOL_SYMBOL);
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
        if (field.isNumeric()) {
            writeField(field.getKey(), field.getValue());
        } else {
            writeFieldQuoted(field.getKey(), field.iterator());
        }
    }

    @Override
    public void writeFieldQuoted(byte key, String string) {
        writeFieldQuoted(key, string.getBytes(StandardCharsets.UTF_8));
    }

    @Override
    public void writeFieldQuoted(byte key, byte[] data) {
        writeCharAsByte(key);
        writeCharAsByte(Zchars.Z_STRING_TYPE_QUOTED);
        for (byte b : data) {
            writeStringByte(b);
        }
        writeCharAsByte(Zchars.Z_STRING_TYPE_QUOTED);
    }

    @Override
    public void writeFieldQuoted(byte key, BlockIterator dataIterator) {
        writeCharAsByte(key);
        writeCharAsByte(Zchars.Z_STRING_TYPE_QUOTED);

        while (dataIterator.hasNext()) {
            writeStringByte(dataIterator.next());
        }
        writeCharAsByte(Zchars.Z_STRING_TYPE_QUOTED);
    }

    @Override
    public void writeFieldHex(byte key, byte[] data) {
        writeCharAsByte(key);
        writeBytes(data, data.length, true);
    }

    @Override
    public void writeFieldHex(byte key, BlockIterator dataIterator) {
        writeCharAsByte(key);
        while (dataIterator.hasNext()) {
            byte[] data = dataIterator.nextContiguous();
            writeBytes(data, data.length, true);
        }
    }

    @Override
    public void writeOrElse() {
        writeCharAsByte(Zchars.Z_ORELSE);
    }

    @Override
    public void writeAndThen() {
        writeCharAsByte(Zchars.Z_ANDTHEN);
    }

    @Override
    public void writeOpenParen() {
        writeCharAsByte(Zchars.Z_OPENPAREN);
    }

    @Override
    public void writeCloseParen() {
        writeCharAsByte(Zchars.Z_CLOSEPAREN);
    }
}

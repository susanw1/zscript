package org.zcode.javareceiver;

import java.nio.charset.StandardCharsets;

public abstract class AbstractZcodeOutStream extends ZcodeOutStream {

    public abstract void writeByte(byte value);

    @Override
    public ZcodeOutStream markNotification() {
        writeByte((byte) '!');
        return this;
    }

    @Override
    public ZcodeOutStream markBroadcast() {
        writeByte((byte) '*');
        return this;
    }

    @Override
    public ZcodeOutStream writeStatus(final ZcodeResponseStatus st) {
        writeByte((byte) 'S');
        if (st.getValue() != 0) {
            final byte[] arr = Integer.toHexString(st.getValue()).getBytes(StandardCharsets.US_ASCII);
            return writeBytes(arr, arr.length);
        } else {
            return this;
        }
    }

    @Override
    public ZcodeOutStream writeField(final char f, final byte v) {
        writeByte((byte) f);
        if (v != 0) {
            final byte[] arr = Integer.toHexString(Byte.toUnsignedInt(v)).getBytes(StandardCharsets.US_ASCII);
            return writeBytes(arr, arr.length);
        } else {
            return this;
        }
    }

    @Override
    public ZcodeOutStream continueField(final byte v) {
        final byte[] arr = Integer.toHexString(Byte.toUnsignedInt(v)).getBytes(StandardCharsets.US_ASCII);
        if (arr.length == 1) {
            writeByte((byte) '0');
            writeByte(arr[0]);
        } else {
            return writeBytes(arr, arr.length);
        }
        return this;
    }

    @Override
    public ZcodeOutStream writeBigHexField(final byte[] value, final int length) {
        writeByte((byte) '+');
        for (int i = 0; i < length; i++) {
            writeByte(toHexDigit((value[i] >>> 4) & 0x0F));
            writeByte(toHexDigit(value[i] & 0x0F));
        }
        return this;
    }

    @Override
    public ZcodeOutStream writeBigStringField(final byte[] value, final int length) {
        writeByte((byte) '"');
        for (int i = 0; i < length; i++) {
            if (value[i] == '\n') {
                writeByte((byte) '\\');
                writeByte((byte) 'n');
            } else if (value[i] == '\\' || value[i] == '"') {
                writeByte((byte) '\\');
                writeByte(value[i]);
            } else {
                writeByte(value[i]);
            }
        }
        writeByte((byte) '"');
        return this;
    }

    @Override
    public ZcodeOutStream writeBigStringField(final String s) {
        writeByte((byte) '"');
        final byte[] arr = s.replace("\\", "\\\\").replace("\n", "\\n").replace("\"", "\\\"").getBytes(StandardCharsets.UTF_8);
        writeBytes(arr, arr.length);
        writeByte((byte) '"');
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSeparator() {
        writeByte((byte) '&');
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSequenceErrorHandler() {
        writeByte((byte) '|');
        return this;
    }

    @Override
    public ZcodeOutStream writeCommandSequenceSeparator() {
        writeByte((byte) '\n');
        return this;
    }

    private byte toHexDigit(final int i) {
        return (byte) (i > 9 ? 'a' + i - 10 : '0' + i);
    }

}

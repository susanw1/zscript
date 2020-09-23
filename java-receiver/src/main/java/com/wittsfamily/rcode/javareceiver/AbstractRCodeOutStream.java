package com.wittsfamily.rcode.javareceiver;

import java.nio.charset.StandardCharsets;

public abstract class AbstractRCodeOutStream extends RCodeOutStream {

    public abstract void writeByte(byte value);

    @Override
    public RCodeOutStream markNotification() {
        writeByte((byte) '!');
        return this;
    }

    @Override
    public RCodeOutStream markBroadcast() {
        writeByte((byte) '*');
        return this;
    }

    @Override
    public RCodeOutStream writeStatus(RCodeResponseStatus st) {
        writeByte((byte) 'S');
        if (st.getValue() != 0) {
            byte[] arr = Integer.toHexString(st.getValue()).getBytes(StandardCharsets.US_ASCII);
            return writeBytes(arr, arr.length);
        } else {
            return this;
        }
    }

    @Override
    public RCodeOutStream writeField(char f, byte v) {
        writeByte((byte) f);
        if (v != 0) {
            byte[] arr = Integer.toHexString(Byte.toUnsignedInt(v)).getBytes(StandardCharsets.US_ASCII);
            return writeBytes(arr, arr.length);
        } else {
            return this;
        }
    }

    @Override
    public RCodeOutStream continueField(byte v) {
        byte[] arr = Integer.toHexString(Byte.toUnsignedInt(v)).getBytes(StandardCharsets.US_ASCII);
        if (arr.length == 1) {
            writeByte((byte) '0');
            writeByte(arr[0]);
        } else {
            return writeBytes(arr, arr.length);
        }
        return this;
    }

    @Override
    public RCodeOutStream writeBigHexField(byte[] value, int length) {
        writeByte((byte) '+');
        for (int i = 0; i < length; i++) {
            writeByte(toHexDigit((value[i] >>> 4) & 0x0F));
            writeByte(toHexDigit(value[i] & 0x0F));
        }
        return this;
    }

    @Override
    public RCodeOutStream writeBigStringField(byte[] value, int length) {
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
    public RCodeOutStream writeBigStringField(String s) {
        writeByte((byte) '"');
        byte[] arr = s.replace("\\", "\\\\").replace("\n", "\\n").replace("\"", "\\\"").getBytes(StandardCharsets.UTF_8);
        writeBytes(arr, arr.length);
        writeByte((byte) '"');
        return this;
    }

    @Override
    public RCodeOutStream writeCommandSeperator() {
        writeByte((byte) '&');
        return this;
    }

    @Override
    public RCodeOutStream writeCommandSequenceErrorHandler() {
        writeByte((byte) '|');
        return this;
    }

    @Override
    public RCodeOutStream writeCommandSequenceSeperator() {
        writeByte((byte) '\n');
        return this;
    }

    private byte toHexDigit(int i) {
        return (byte) (i > 9 ? 'a' + i - 10 : '0' + i);
    }

}

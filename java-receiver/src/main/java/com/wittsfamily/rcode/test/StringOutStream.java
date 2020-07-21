package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.AbstractRCodeOutStream;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

public class StringOutStream extends AbstractRCodeOutStream {
    private final StringBuilder builder = new StringBuilder();

    @Override
    public void writeByte(byte value) {
        builder.append((char) value);
    }

    @Override
    public RCodeOutStream writeBytes(byte[] value, int length) {
        for (int i = 0; i < length; i++) {
            writeByte(value[i]);
        }
        return this;
    }

    @Override
    public void openResponse(RCodeCommandChannel target) {
    }

    @Override
    public void openNotification() {
    }

    @Override
    public boolean isOpen() {
        return true;
    }

    @Override
    public void close() {
    }

    public String getString() {
        return builder.toString();
    }
}

package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class StringOutStream extends AbstractRCodeOutStream {
    private final StringBuilder builder = new StringBuilder();
    private boolean open = false;

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
        open = true;
    }

    @Override
    public void openNotification(RCodeCommandChannel target) {
        open = true;
    }

    @Override
    public void openDebug(RCodeCommandChannel target) {
        open = true;
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    public void close() {
        open = false;
    }

    public String getString() {
        return builder.toString();
    }
}

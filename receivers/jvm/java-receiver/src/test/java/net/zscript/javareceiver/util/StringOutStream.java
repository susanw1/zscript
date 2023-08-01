package net.zscript.javareceiver.util;

import net.zscript.javareceiver.AbstractZcodeOutStream;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.parsing.ZcodeCommandChannel;

public class StringOutStream extends AbstractZcodeOutStream {
    private final StringBuilder builder = new StringBuilder();
    private boolean             open    = false;

    @Override
    public void writeByte(final byte value) {
        builder.append((char) value);
    }

    @Override
    public ZcodeOutStream writeBytes(final byte[] value, final int length) {
        for (int i = 0; i < length; i++) {
            writeByte(value[i]);
        }
        return this;
    }

    @Override
    public void openResponse(final ZcodeCommandChannel target) {
        open = true;
    }

    @Override
    public void openNotification(final ZcodeCommandChannel target) {
        open = true;
    }

    @Override
    public void openDebug(final ZcodeCommandChannel target) {
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

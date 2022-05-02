package org.zcode.javareceiver.util;

import org.zcode.javareceiver.AbstractZcodeOutStream;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;

public class PrintingOutStream extends AbstractZcodeOutStream {

    @Override
    public void writeByte(final byte value) {
        System.out.print((char) value);
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
    }

    @Override
    public void openNotification(final ZcodeCommandChannel target) {
    }

    @Override
    public void openDebug(final ZcodeCommandChannel target) {
    }

    @Override
    public boolean isOpen() {
        return true;
    }

    @Override
    public void close() {
    }

}

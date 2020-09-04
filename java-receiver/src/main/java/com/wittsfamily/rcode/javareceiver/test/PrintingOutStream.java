package com.wittsfamily.rcode.javareceiver.test;

import com.wittsfamily.rcode.javareceiver.AbstractRCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;

public class PrintingOutStream extends AbstractRCodeOutStream {

    @Override
    public void writeByte(byte value) {
        System.out.print((char) value);
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
    public void openNotification(RCodeCommandChannel target) {
    }

    @Override
    public void openDebug(RCodeCommandChannel target) {
    }

    @Override
    public boolean isOpen() {
        return true;
    }

    @Override
    public void close() {
    }

}

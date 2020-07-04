package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.AbstractRCodeOutStream;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;

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
    public void openNotification() {
    }

    @Override
    public boolean isOpen() {
        return true;
    }

    @Override
    public void close() {
    }

}

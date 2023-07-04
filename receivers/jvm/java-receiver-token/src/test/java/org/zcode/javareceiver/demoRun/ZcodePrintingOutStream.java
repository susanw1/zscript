package org.zcode.javareceiver.demoRun;

import org.zcode.javareceiver.core.ZcodeAbstractOutStream;

public class ZcodePrintingOutStream extends ZcodeAbstractOutStream {
    private boolean open = false;

    @Override
    public void open() {
        open = true;
    }

    @Override
    public void close() {
        open = false;
    }

    @Override
    public boolean isOpen() {
        return open;
    }

    @Override
    public void writeByte(byte b) {
        System.out.print((char) b);
    }

}
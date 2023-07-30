package org.zcode.javareceiver.demoRun;

import java.io.IOException;
import java.io.PrintStream;

import org.zcode.javareceiver.core.OutputStreamOutStream;

public class ZcodePrintingOutStream extends OutputStreamOutStream<PrintStream> {
    public ZcodePrintingOutStream() throws IOException {
        super(System.out);
    }

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
}

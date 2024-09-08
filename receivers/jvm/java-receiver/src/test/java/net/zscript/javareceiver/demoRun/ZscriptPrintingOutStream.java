package net.zscript.javareceiver.demoRun;

import java.io.IOException;
import java.io.PrintStream;

import net.zscript.javareceiver.core.OutputStreamOutStream;

public class ZscriptPrintingOutStream extends OutputStreamOutStream<PrintStream> {
    public ZscriptPrintingOutStream() throws IOException {
        super(System.out);
    }
}

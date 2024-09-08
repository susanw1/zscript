package net.zscript.javareceiver.demoRun;

import java.io.PrintStream;

import net.zscript.javareceiver.core.OutputStreamOutStream;

public class ZscriptPrintingOutStream extends OutputStreamOutStream<PrintStream> {
    public ZscriptPrintingOutStream() {
        super(System.out);
    }
}

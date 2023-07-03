package org.zcode.javareceiver.core;

import org.zcode.javareceiver.semanticParser.SemanticParser;

public class ZcodeChannel {
    private final SemanticParser p;
    private final ZcodeOutStream out;

    public ZcodeChannel(SemanticParser p, ZcodeOutStream out) {
        this.p = p;
        this.out = out;
    }

    public SemanticParser getParser() {
        return p;
    }

    public ZcodeOutStream getOutStream() {
        return out;
    }
}

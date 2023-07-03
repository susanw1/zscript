package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.semanticParser.SemanticParser;

public class ZcodeAddressingView {
    private final SemanticParser parser;
    private final ZcodeOutStream out;

    public ZcodeAddressingView(SemanticParser parser, ZcodeOutStream out) {
        this.parser = parser;
        this.out = out;
    }

    public void execute() {
        // TODO Auto-generated method stub

    }

    public void moveAlong() {
        // TODO Auto-generated method stub

    }

    public boolean verify() {
        // TODO Auto-generated method stub
        return false;
    }
}

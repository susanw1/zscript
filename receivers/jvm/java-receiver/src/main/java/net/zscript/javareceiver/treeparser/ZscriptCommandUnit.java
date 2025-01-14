package net.zscript.javareceiver.treeparser;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class ZscriptCommandUnit implements ZscriptSequenceUnit {
    private final ReadToken start;

    public ZscriptCommandUnit(ReadToken start) {
        this.start = start;
    }

    @Override
    public boolean parse() {
        return false;
    }

    @Override
    public String toString() {
        return "Command:{" + start + "}";
    }
}

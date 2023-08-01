package net.zscript.javareceiver.treeParser;

import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeCommandUnit implements ZcodeSequenceUnit {
    private final ReadToken start;

    public ZcodeCommandUnit(ReadToken start) {
        this.start = start;
    }

    @Override
    public boolean parse() {
        return false;
    }

    @Override
    public String toString() {
        return "Command";
    }
}

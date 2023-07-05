package org.zcode.javareceiver.treeParser;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;
import org.zcode.javareceiver.tokenizer.ZcodeTokenizer;

public class ZcodeOrSeperatedUnit implements ZcodeSequenceUnit {
    private final ZcodeSequenceUnitPlace before;
    private final ZcodeSequenceUnitPlace after;

    public ZcodeOrSeperatedUnit(ReadToken start, int startParenLevel, ReadToken nextStart, byte endMarker) {
        this.before = new ZcodeSequenceUnitPlace(start, ZcodeTokenizer.CMD_END_ORELSE, startParenLevel);
        this.after = new ZcodeSequenceUnitPlace(nextStart, endMarker, 0);
    }

    @Override
    public boolean parse() {
        return before.parse() | after.parse();
    }

    @Override
    public String toString() {
        return "(" + before.toString() + "|" + after.toString() + ")";
    }

}

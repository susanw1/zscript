package net.zscript.javareceiver.treeParser;

import net.zscript.javareceiver.tokenizer.Tokenizer;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class ZscriptOrSeparatedUnit implements ZscriptSequenceUnit {
    private final ZscriptSequenceUnitPlace before;
    private final ZscriptSequenceUnitPlace after;

    public ZscriptOrSeparatedUnit(ReadToken start, int startParenLevel, ReadToken nextStart, byte endMarker) {
        this.before = new ZscriptSequenceUnitPlace(start, Tokenizer.CMD_END_ORELSE, startParenLevel);
        this.after = new ZscriptSequenceUnitPlace(nextStart, endMarker, 0);
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

package net.zscript.javareceiver.treeParser;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class ZscriptSequenceUnitPlace {
    private ZscriptSequenceUnit unit;

    public ZscriptSequenceUnitPlace(ReadToken start, byte endMarker, int startParenLevel) {
        unit = new ZscriptUnknownCommandUnit(this, start, endMarker, startParenLevel);
    }

    public void replace(ZscriptSequenceUnit unit) {
        this.unit = unit;
    }

    public boolean parse() {
        return unit.parse();
    }

    @Override
    public String toString() {
        return unit.toString();
    }
}

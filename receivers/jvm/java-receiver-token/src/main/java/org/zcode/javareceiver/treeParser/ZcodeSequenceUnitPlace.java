package org.zcode.javareceiver.treeParser;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeSequenceUnitPlace {
    private ZcodeSequenceUnit unit;

    public ZcodeSequenceUnitPlace(ReadToken start, byte endMarker, int startParenLevel) {
        unit = new ZcodeUnknownCommandUnit(this, start, endMarker, startParenLevel);
    }

    public void replace(ZcodeSequenceUnit unit) {
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

package net.zscript.javareceiver.treeParser;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class ZscriptUnknownCommandUnit implements ZscriptSequenceUnit {
    private final ZscriptSequenceUnitPlace place;
    private final ReadToken                start;
    private final byte                     endMarker;
    private final int                      startParenLevel;

    public ZscriptUnknownCommandUnit(ZscriptSequenceUnitPlace place, ReadToken start, byte endMarker, int startParenLevel) {
        this.place = place;
        this.start = start;
        this.endMarker = endMarker;
        this.startParenLevel = startParenLevel;
    }

    @Override
    public boolean parse() {
        place.replace(ZscriptTreeParser.parse(start, endMarker, startParenLevel));
        return true;
    }

}

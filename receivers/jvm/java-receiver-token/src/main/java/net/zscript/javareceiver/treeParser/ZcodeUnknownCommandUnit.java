package net.zscript.javareceiver.treeParser;

import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeUnknownCommandUnit implements ZcodeSequenceUnit {
    private final ZcodeSequenceUnitPlace place;
    private final ReadToken              start;
    private final byte                   endMarker;
    private final int                    startParenLevel;

    public ZcodeUnknownCommandUnit(ZcodeSequenceUnitPlace place, ReadToken start, byte endMarker, int startParenLevel) {
        this.place = place;
        this.start = start;
        this.endMarker = endMarker;
        this.startParenLevel = startParenLevel;
    }

    @Override
    public boolean parse() {
        place.replace(ZcodeTreeParser.parse(start, endMarker, startParenLevel));
        return true;
    }

}

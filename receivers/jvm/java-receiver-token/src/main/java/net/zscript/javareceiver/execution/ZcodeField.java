package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeField implements Iterable<Byte> {
    private final ReadToken token;

    public ZcodeField(ReadToken token) {
        this.token = token;
    }

    public byte getKey() {
        return token.getKey();
    }

    public int getValue() {
        return token.getData16();
    }

    public boolean isBigField() {
        return Zchars.isBigField(token.getKey());
    }

    @Override
    public BlockIterator iterator() {
        return token.blockIterator();
    }

}

package net.zscript.javareceiver.execution;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.BlockIterator;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

public class ZscriptTokenField implements Iterable<Byte> {
    private final ReadToken token;

    public ZscriptTokenField(ReadToken token) {
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

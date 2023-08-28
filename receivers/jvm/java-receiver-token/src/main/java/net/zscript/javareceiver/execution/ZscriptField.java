package net.zscript.javareceiver.execution;

import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.model.components.Zchars;

public class ZscriptField implements Iterable<Byte> {
    private final ReadToken token;

    public ZscriptField(ReadToken token) {
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

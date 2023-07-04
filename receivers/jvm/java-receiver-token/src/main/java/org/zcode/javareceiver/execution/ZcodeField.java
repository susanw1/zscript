package org.zcode.javareceiver.execution;

import org.zcode.javareceiver.tokenizer.BlockIterator;
import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

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

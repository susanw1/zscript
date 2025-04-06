package net.zscript.tokenizer;

import javax.annotation.Nonnull;

import static java.util.Objects.requireNonNull;

import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
import net.zscript.util.BlockIterator;

/**
 * Given a ReadToken, this provides a "field view" of the token.
 */
public class ZscriptTokenField implements ZscriptField {
    private final ReadToken token;

    public ZscriptTokenField(ReadToken token) {
        this.token = requireNonNull(token);
    }

    public byte getKey() {
        return token.getKey();
    }

    public int getValue() {
        return token.getData16();
    }

    @Override
    public boolean isNumeric() {
        return token.hasNumeric(2);
    }

    @Nonnull
    @Override
    public BlockIterator iterator() {
        return token.dataIterator();
    }

    @Override
    public String toString() {
        return token.toString();
    }
}

package net.zscript.javaclient.commandPaths;

import net.zscript.tokenizer.BlockIterator;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

public class NumberField implements ZscriptField, ByteString.ByteAppendable {
    private final byte key;
    private final int  value;

    public static NumberField fieldOf(byte key, int value) {
        return new NumberField(key, value);
    }

    public NumberField(byte key, int value) {
        this.key = key;
        this.value = value;
    }

    @Override
    public byte getKey() {
        return key;
    }

    @Override
    public int getValue() {
        return value;
    }

    @Override
    public boolean isBigField() {
        return false;
    }

    @Override
    public BlockIterator iterator() {
        return BlockIterator.EMPTY;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.appendByte(key).appendNumeric16(value);
    }

    @Override
    public String toString() {
        return "NumberField[" + ByteString.from(this) + "]";
    }
}

package net.zscript.javaclient.commandPaths;

import net.zscript.tokenizer.BlockIterator;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

public class NumberField implements ZscriptField, ByteString.ByteAppendable {
    private byte key;
    private int  value;

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
        append(key, value, builder);
    }

    public static void append(byte key, int value, ByteStringBuilder builder) {
        builder.appendByte(key).appendNumeric16(value);
    }
}

package net.zscript.javaclient.commandPaths;

import javax.annotation.Nonnull;
import java.util.Objects;

import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

public final class NumberField implements ZscriptField, ByteString.ByteAppendable {
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

    @Nonnull
    @Override
    public BlockIterator iterator() {
        return BlockIterator.EMPTY;
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.appendByte(key).appendNumeric16(value);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (!(o instanceof ZscriptField))
            return false;
        ZscriptField other = (ZscriptField) o;
        return !other.isBigField() && key == other.getKey() && value == other.getValue();
    }

    @Override
    public int hashCode() {
        return Objects.hash(key, value);
    }

    @Override
    public String toString() {
        return "NumberField[" + ByteString.from(this).asString() + "]";
    }
}

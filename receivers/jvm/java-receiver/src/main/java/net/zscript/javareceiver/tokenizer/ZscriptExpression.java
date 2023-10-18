package net.zscript.javareceiver.tokenizer;

import java.util.Iterator;
import java.util.OptionalInt;

import net.zscript.model.components.Zchars;

public interface ZscriptExpression {
    OptionalInt getField(byte f);

    default OptionalInt getField(char key) {
        return getField((byte) key);
    }

    default boolean hasField(final byte key) {
        return getField(key).isPresent();
    }

    default boolean hasField(final char key) {
        return hasField((byte) key);
    }

    default int getField(final byte key, final int def) {
        return getField(key).orElse(def);
    }

    default int getField(final char key, final int def) {
        return getField((byte) key, def);
    }

    int getFieldCount();

    boolean hasBigField();

    int getBigFieldSize();

    byte[] getBigFieldData();

}

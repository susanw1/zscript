package net.zscript.javareceiver.tokenizer;

import java.util.Iterator;
import java.util.OptionalInt;

public interface ZscriptExpression {
    OptionalInt getField(byte f);

    default boolean hasField(final byte key) {
        return getField(key).isPresent();
    }

    default int getField(final byte key, final int def) {
        return getField(key).orElse(def);
    }

    default OptionalInt getField(char key) {
        return getField((byte) key);
    }

    int getFieldCount();

    BlockIterator getBigField();

    boolean hasBigField();

    int getBigFieldSize();

    default byte[] getBigFieldData() {
        byte[] data = new byte[getBigFieldSize()];
        int    i    = 0;
        for (Iterator<Byte> iterator = getBigField(); iterator.hasNext();) {
            data[i++] = iterator.next();
        }
        return data;
    }

    default boolean isValid(final byte[] hasRequiredKeys) {
        for (byte b : hasRequiredKeys) {
            if (Zchars.isNumericKey(b) && !hasField(b) || Zchars.isBigField(b) && !hasBigField()) {
                return false;
            }
        }
        return true;
    }
}

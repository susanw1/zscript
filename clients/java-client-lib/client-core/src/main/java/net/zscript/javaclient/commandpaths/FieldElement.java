package net.zscript.javaclient.commandpaths;

import javax.annotation.Nonnull;
import java.util.Objects;

import static net.zscript.util.ByteString.builder;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.concat;

import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteStringBuilder;

public final class FieldElement implements ZscriptField, ByteString.ByteAppendable {
    private final byte       key;
    private final ByteString data;
    private final boolean    preferString;

    public static FieldElement fieldOf(byte key, int value) {
        return new FieldElement(key, value);
    }

    public static FieldElement fieldOf(byte key, long value) {
        if (value > 0xffff_ffffL) {
            throw new IllegalArgumentException("value too large for 32-bit field");
        }
        return new FieldElement(key, (int) value);
    }

    public static FieldElement fieldOfText(byte key, ByteString string) {
        return new FieldElement(key, string, true);
    }

    public static FieldElement fieldOfBytes(byte key, ByteString string) {
        return new FieldElement(key, string, false);
    }

    public static FieldElement fieldAsSmallest(byte key, ByteString string) {
        int len      = string.size();
        int escCount = 0;
        for (byte b : string.toByteArray()) {
            if (Zchars.mustStringEscape(b)) {
                escCount++;
            }
        }
        return new FieldElement(key, string, len * 2 > len + escCount * 2);
    }

    public static FieldElement fieldOf(ZscriptField f) {
        return f instanceof FieldElement ? (FieldElement) f : new FieldElement(f.getKey(), concat(f.iterator()), false);
    }

    /**
     * Creates the FieldElement from the supplied key and value. Note that the value can be a full 32-bit number and is treated as though unsigned.
     *
     * @param key   the key for the field
     * @param value the value to encode (as up to 4 bytes)
     */
    private FieldElement(byte key, int value) {
        this.key = key;
        final ByteStringBuilder b = builder();

        for (int offset = 24; offset >= 0; offset -= 8) {
            final int ch = value >>> offset;
            if (ch != 0) {
                b.appendByte(ch & 0xff);
            }
        }
        this.data = b.build();
        this.preferString = false;
    }

    private FieldElement(byte key, ByteString data, boolean preferString) {
        this.key = key;
        this.data = data;
        this.preferString = preferString;
    }

    @Override
    public byte getKey() {
        return key;
    }

    /**
     * Utility method that reads a 2-byte number from the end of the 'data' array,  working back from the supplied 'end' value. Specifically, the two bytes are taken from index
     * {@code end-2} and {@code end-1}.
     *
     * @param end the end index, exclusively (ie, value is read from the previous <= 2 bytes)
     * @return the value read (in range 0-0xffff)
     */
    private int getValueImpl(int end) {
        int t = 0;
        if (end >= 2) {
            t = (data.get(end - 2) & 0xff) << 8;
        }
        if (end >= 1) {
            t |= data.get(end - 1) & 0xff;
        }
        return t;
    }

    @Override
    public int getValue() {
        return getValueImpl(data.size());
    }

    @Override
    public long getValue32() {
        final int sz = data.size();
        int       t  = getValueImpl(sz);
        if (sz > 2) {
            t |= getValueImpl(sz - 2) << 16;
        }
        return t & 0xffff_ffffL;
    }

    @Override
    public boolean isNumeric() {
        return data.size() <= 2 && !preferString;
    }

    /**
     * Returns a copy of the field's data.
     *
     * @return copy of the field's data
     */
    @Nonnull
    public byte[] getData() {
        return data.toByteArray();
    }

    /**
     * Returns the field data as a ByteString.
     *
     * @return the field data
     */
    @Nonnull
    public ByteString getDataAsByteString() {
        return data;
    }

    /**
     * Determines the length of the data, in bytes.
     *
     * @return number of data bytes
     */
    public int getDataLength() {
        return data.size();
    }

    @Nonnull
    @Override
    public BlockIterator iterator() {
        return data.iterator();
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        builder.appendByte(key);
        if (isNumeric()) {
            builder.appendNumeric16(getValue());
        } else if (preferString) {
            builder.appendByte(Zchars.Z_STRING_TYPE_QUOTED);
            for (byte b : data) {
                if (Zchars.mustStringEscape(b)) {
                    builder.appendByte(Zchars.Z_STRING_ESCAPE).appendHexPair(b);
                } else {
                    builder.appendByte(b);
                }
            }
            builder.appendByte(Zchars.Z_STRING_TYPE_QUOTED);
        } else {
            for (byte b : data) {
                builder.appendHexPair(b);
            }
        }
    }

    public boolean matches(ZscriptField f) {
        if (key != f.getKey()) {
            return false;
        }
        if (f instanceof FieldElement) {
            final FieldElement otherFE = (FieldElement) f;
            return isNumeric() && otherFE.isNumeric() ?
                    getValue() == otherFE.getValue() : data.equals(otherFE.getDataAsByteString());
        } else {
            return data.equals(byteString(f.iterator()));
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        final FieldElement other = (FieldElement) o;
        if (key != other.getKey())
            return false;
        return data.equals(other.data) && preferString == other.preferString;
    }

    @Override
    public int hashCode() {
        return Objects.hash(key, data, preferString);
    }

    @Override
    public String toString() {
        return toStringImpl();
    }
}

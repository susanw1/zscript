package net.zscript.tokenizer;

import javax.annotation.Nonnull;

import net.zscript.util.BlockIterator;

/**
 * Represents a Zscript field, which may be either numeric or a big-field.
 */
public interface ZscriptField extends Iterable<Byte> {
    /**
     * Determines the key for this field (eg u/c letter for a numeric field, or big-field key)
     *
     * @return the key
     */
    byte getKey();

    /**
     * Numeric value of the first 2 bytes of data, as a 16-bit unsigned value.
     *
     * @return the value of this field's data
     */
    int getValue();

    /**
     * Indicates whether this field has a meaningful numeric form, which implies {@link #getValue()} gives a reasonable value.
     *
     * @return true if numeric, false if string only
     */
    boolean isNumeric();

    /**
     * @return true if this is a big-field (quoted or string), false otherwise. Note, true big-fields are obsolete, as any A-Z field can be string or numeric
     */
    @Deprecated
    boolean isBigField();

    /**
     * Returns a data iterator for the data in this field, including extension data.
     *
     * @return an iterator over the data in this field
     */
    @Nonnull
    @Override
    BlockIterator iterator();
}

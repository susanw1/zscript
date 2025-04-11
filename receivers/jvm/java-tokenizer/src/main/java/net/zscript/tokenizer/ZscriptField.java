package net.zscript.tokenizer;

import javax.annotation.Nonnull;

import net.zscript.util.BlockIterator;

/**
 * Represents a Zscript field, which may be either numeric or string.
 */
public interface ZscriptField extends Iterable<Byte> {
    /**
     * Determines the key for this field (eg u/c letter for a normal expression field)
     *
     * @return the key
     */
    byte getKey();

    /**
     * Numeric value of the field data, as a 16-bit unsigned value stored in an int to avoid signedness issues. If the data contains more than 16 bits, then the lowest 2 bytes are
     * returned (ie the last two in the buffer, ignoring the rest.
     *
     * @return the value of this field's data as a 16-bit unsigned value
     */
    int getValue();

    /**
     * Numeric value of the field data, as a 32-bit unsigned value stored in a long to avoid signedness issues. If the data contains more than 32 bits, then the lowest 4 bytes are
     * returned (ie the last four in the buffer, ignoring the rest.
     *
     * @return the value of this field's data as a 32-bit unsigned value
     */
    long getValue32();

    /**
     * Indicates whether this field fits into a 16-bit numeric form, which implies {@link #getValue()} gives a reasonable value.
     *
     * @return true if numeric, false if string only
     */
    boolean isNumeric();

    /**
     * Returns a data iterator for the data in this field, including extension data.
     *
     * @return an iterator over the data in this field
     */
    @Nonnull
    @Override
    BlockIterator iterator();
}

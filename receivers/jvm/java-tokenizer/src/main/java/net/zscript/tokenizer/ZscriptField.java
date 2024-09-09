package net.zscript.tokenizer;

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
     * If this is a numeric field, then this is the numeric value of the 16-bit field. Otherwise zero.
     *
     * @return the value of this field, or zero
     */
    int getValue();

    /**
     * @return true if this is a big-field (quoted or string), false otherwise
     */
    boolean isBigField();

    /**
     * Returns a data iterator for the data in this field. This might be empty for a numeric field, so check that {@link #isBigField()} is true to avoid uncertainty.
     *
     * @return an iterator over the data in this field (including extension data)
     */
    @Override
    BlockIterator iterator();
}

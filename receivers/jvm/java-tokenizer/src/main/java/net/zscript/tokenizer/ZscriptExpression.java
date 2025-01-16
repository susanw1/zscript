package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.stream.Stream;

import net.zscript.util.ByteString;

/**
 * Defines the information available from a Zscript expression, that is, a single Zscript command or response with fields and big-fields; an element of a command/response
 * sequence.
 */
public interface ZscriptExpression {
    /**
     * Accesses all the numeric fields in indeterminate order (not big-fields - use {@link #getBigFieldAsByteString()}).
     *
     * @return stream of numeric fields
     */
    @Nonnull
    Stream<ZscriptField> fields();

    /**
     * Gets the ZscriptField representing the specified numeric field (key, value), if it exists.
     *
     * @param key the key of the required field
     * @return the field, or empty if the field isn't defined
     */
    @Nonnull
    Optional<? extends ZscriptField> getZscriptField(byte key);

    /**
     * Gets the ZscriptField representing the specified numeric field (key, value), if it exists.
     *
     * @param key the key of the required field
     * @return the field, or empty if the field isn't defined
     */
    @Nonnull
    default Optional<? extends ZscriptField> getZscriptField(char key) {
        return getZscriptField((byte) key);
    }

    /**
     * Gets the value of the specified field.
     *
     * @param key the key of the required field
     * @return the value of that field, or empty if the field isn't defined
     */
    @Nonnull
    default OptionalInt getField(byte key) {
        // I thought there would be a super-cool way to do this that wasn't basically an if-statement...OptionalInt makes it messy.
        return getZscriptField(key)
                .map(f -> OptionalInt.of(f.getValue()))
                .orElseGet(OptionalInt::empty);
    }

    /**
     * Gets the value of the specified field.
     *
     * @param key the key of the required field
     * @return the value of that field, or empty if the field isn't defined
     */
    @Nonnull
    default OptionalInt getField(char key) {
        return getField((byte) key);
    }

    /**
     * Determines whether the specified field is defined.
     *
     * @param key the key of the required field
     * @return true if it exists, false otherwise
     */
    default boolean hasField(final byte key) {
        return getField(key).isPresent();
    }

    /**
     * Determines whether the specified field is defined.
     *
     * @param key the key of the required field
     * @return true if it exists, false otherwise
     */
    default boolean hasField(final char key) {
        return hasField((byte) key);
    }

    /**
     * Gets the value of the specified field, or a default value if the field isn't defined.
     *
     * @param key the key of the required field
     * @param def the default value
     * @return the value of that field, or empty if the field isn't defined
     */
    default int getField(final byte key, final int def) {
        return getField(key).orElse(def);
    }

    /**
     * Gets the value of the specified field, or a default value if the field isn't defined.
     *
     * @param key the key of the required field
     * @param def the default value
     * @return the value of that field, or empty if the field isn't defined
     */
    default int getField(final char key, final int def) {
        return getField((byte) key, def);
    }

    /**
     * Determine how many fields are defined in this expression (except big-field).
     *
     * @return the number of defined fields
     */
    int getFieldCount();

    /**
     * Determines whether there is a big-field defined.
     *
     * @return true if there is a big-field, false otherwise
     */
    boolean hasBigField();

    /**
     * Determines the total number of bytes in the big-field (or fields, concatenated, if there are several).
     *
     * @return the big-field size, in bytes (zero if no big-field is found)
     */
    int getBigFieldSize();

    /**
     * The data associated with the big-field. If multiple, then concatenated, in order. If no big field, then return an empty array.
     *
     * @return a new array containing all big-field data, or empty array if none defined
     */
    @Nonnull
    default byte[] getBigFieldData() {
        return getBigFieldAsByteString().toByteArray();
    }

    /**
     * The data associated with the big-field. If multiple, then concatenated, in order. If no big field, then return an empty array.
     *
     * @return a ByteString containing all big-field data, or empty ByteString if none defined
     */
    @Nonnull
    ByteString getBigFieldAsByteString();
}

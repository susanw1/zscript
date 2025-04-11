package net.zscript.tokenizer;

import javax.annotation.Nonnull;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.OptionalLong;
import java.util.stream.Stream;

import net.zscript.util.BlockIterator;
import net.zscript.util.ByteString;

/**
 * Defines the information available from a Zscript expression, that is, a single Zscript command or response with numeric and string fields; an element of a command/response
 * sequence.
 */
public interface ZscriptExpression {
    /**
     * Accesses all the fields in indeterminate order.
     *
     * @return stream of fields
     */
    @Nonnull
    Stream<? extends ZscriptField> fields();

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
     * Gets the 16-bit value of the specified field.
     *
     * @param key the key of the required field
     * @return the 16-bit value of that field, or empty if the field isn't defined
     */
    @Nonnull
    default OptionalInt getFieldValue(byte key) {
        // I thought there would be a super-cool way to do this that wasn't basically an if-statement...OptionalInt makes it messy.
        return getZscriptField(key)
                .map(f -> OptionalInt.of(f.getValue()))
                .orElseGet(OptionalInt::empty);
    }

    /**
     * Gets the 16-bit value of the specified field.
     *
     * @param key the key of the required field
     * @return the 16-bit value of that field, or empty if the field isn't defined
     */
    @Nonnull
    default OptionalInt getFieldValue(char key) {
        return getFieldValue((byte) key);
    }

    /**
     * Gets the 32-bit value of the specified field.
     *
     * @param key the key of the required field
     * @return the 32-bit value of that field, or empty if the field isn't defined
     */
    default OptionalLong getFieldValue32(byte key) {
        return getZscriptField(key)
                .map(f -> OptionalLong.of(f.getValue32()))
                .orElseGet(OptionalLong::empty);
    }

    /**
     * Gets the 32-bit value of the specified field.
     *
     * @param key the key of the required field
     * @return the 32-bit value of that field, or empty if the field isn't defined
     */
    @Nonnull
    default OptionalLong getFieldValue32(char key) {
        return getFieldValue32((byte) key);
    }

    /**
     * Gets the value of the specified field, or a default value if the field isn't defined.
     *
     * @param key the key of the required field
     * @param def the default value
     * @return the value of that field, or empty if the field isn't defined
     */
    default int getFieldValueOrDefault(final byte key, final int def) {
        return getFieldValue(key).orElse(def);
    }

    /**
     * Gets the value of the specified field, or a default value if the field isn't defined.
     *
     * @param key the key of the required field
     * @param def the default value
     * @return the value of that field, or empty if the field isn't defined
     */
    default int getFieldValueOrDefault(final char key, final int def) {
        return getFieldValueOrDefault((byte) key, def);
    }

    /**
     * Accesses the data associated with the specified field.
     *
     * @param key the key of the required field
     * @return the BlockIterator of data in that field, or empty if the field isn't defined
     */
    @Nonnull
    default Optional<BlockIterator> getFieldData(byte key) {
        return getZscriptField(key)
                .map(ZscriptField::iterator);
    }

    /**
     * Accesses the data associated with the specified field.
     *
     * @param key the key of the required field
     * @return the BlockIterator of data in that field, or empty if the field isn't defined
     */
    @Nonnull
    default Optional<BlockIterator> getFieldData(char key) {
        return getFieldData((byte) key);
    }

    /**
     * Determines whether the specified field is defined.
     *
     * @param key the key of the required field
     * @return true if it exists, false otherwise
     */
    default boolean hasField(final byte key) {
        return getZscriptField(key).isPresent();
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
     * Determine how many fields are defined in this expression
     *
     * @return the number of defined fields
     */
    int getFieldCount();

    /**
     * Gets the byte string content of the specified field.
     *
     * @param key the key of the required field
     * @return the ByteString of data in that field, or empty if the field isn't defined
     */
    @Nonnull
    default Optional<ByteString> getFieldAsByteString(byte key) {
        return getFieldData(key).map(ByteString::concat);
    }

    /**
     * @see #getFieldAsByteString(byte)
     */
    @Nonnull
    default Optional<ByteString> getFieldAsByteString(char key) {
        return getFieldAsByteString((byte) key);
    }

    /**
     * Gets the string length of the specified field.
     *
     * @param key the key of the required field
     * @return the number of data bytes in the specified field, or zero if the field isn't defined
     */
    default int getFieldDataLength(byte key) {
        // this implementation is a fall-back - implementations can probably be far more efficient
        return getFieldAsByteString(key).map(ByteString::size).orElse(0);
    }

    /**
     * Gets the string length of the specified field.
     *
     * @param key the key of the required field
     * @return the number of data bytes in the specified field, or zero if the field isn't defined
     */
    default int getFieldDataLength(char key) {
        return getFieldDataLength((byte) key);
    }
}

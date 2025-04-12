package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Map;

import static java.util.stream.Collectors.joining;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldAsSmallest;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOf;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOfBytes;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOfText;
import static net.zscript.util.ByteString.byteString;
import static net.zscript.util.ByteString.byteStringUtf8;

import net.zscript.javaclient.commandbuilderapi.ZscriptMissingFieldException;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandpaths.FieldElement;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

/**
 * The builder for creating a ZscriptBuiltCommand.
 *
 * @param <T> the type of response this command would expect to receive
 */
public abstract class ZscriptCommandBuilder<T extends ZscriptResponse> {
    ResponseCaptor<T> captor = null;
    final Map<Byte, FieldElement> fields = new HashMap<>();

    /** Set of 26 numeric fields, and bigfield. Bits init'd on {@link #setRequiredFields(byte[])} and cleared when fields are set. */
    private final BitSet requiredFields = new BitSet();

    public ZscriptCommandBuilder<T> setField(byte key, int value) {
        return setField(fieldOf(key, value));
    }

    public ZscriptCommandBuilder<T> setField32(byte key, long value) {
        return setField(fieldOf(key, value));
    }

    protected ZscriptCommandBuilder<T> setFieldAsBytes(byte key, ByteString data) {
        return setField(fieldOfBytes(key, data));
    }

    protected ZscriptCommandBuilder<T> setFieldAsBytes(char key, byte[] data) {
        return setField(fieldOfBytes((byte) key, byteString(data)));
    }

    protected ZscriptCommandBuilder<T> setFieldAsText(byte key, ByteString data) {
        return setField(fieldOfText(key, data));
    }

    protected ZscriptCommandBuilder<T> setFieldAsText(char key, String text) {
        return setField(fieldOfText((byte) key, byteStringUtf8(text)));
    }

    protected ZscriptCommandBuilder<T> setFieldAsSmallest(byte key, ByteString data) {
        return setField(fieldAsSmallest(key, data));
    }

    private ZscriptCommandBuilder<T> setField(FieldElement field) {
        if (!Zchars.isExpressionKey(field.getKey())) {
            throw new IllegalArgumentException("Key not a valid Zscript Command key: " + (char) field.getKey());
        }
        fields.put(field.getKey(), field);
        requiredFields.clear(field.getKey() - 'A');
        return this;
    }

    public ZscriptCommandBuilder<T> setField(char key, int value) {
        return setField((byte) key, value);
    }

    protected FieldElement getFieldOrZero(byte key) {
        if (!Zchars.isExpressionKey(key)) {
            throw new IllegalArgumentException("Key not a valid Zscript Command key: " + (char) key);
        }
        return fields.getOrDefault(key, fieldOf(key, 0));
    }

    protected FieldElement getFieldOrZero(char key) {
        return getFieldOrZero((byte) key);
    }

    protected final void setRequiredFields(byte[] keys) {
        for (byte k : keys) {
            if (Zchars.isExpressionKey(k)) {
                requiredFields.set(k - 'A');
            }
        }
    }

    @Nonnull
    public ZscriptCommandBuilder<T> capture(ResponseCaptor<T> captor) {
        this.captor = captor;
        return this;
    }

    /**
     * Determines whether this command has all of its required fields set.
     *
     * @return true if required fields are all set, false otherwise
     */
    public final boolean isValid() {
        return requiredFields.isEmpty();
    }

    /**
     * Validates that there are no invalid (missing+required) fields. To be called on {@link #build()}.
     *
     * @throws ZscriptMissingFieldException if builder doesn't yet pass validation
     */
    protected void failIfInvalid() {
        if (!isValid()) {
            String fieldList = requiredFields.stream()
                    .mapToObj(b -> "'" + String.valueOf((char) (b + 'A')) + "'")
                    .collect(joining(","));
            throw new ZscriptMissingFieldException("missingKeys=%s", fieldList);
        }
    }

    /**
     * Builds the command.
     *
     * @return the command that has been built
     * @throws ZscriptMissingFieldException if builder doesn't yet pass validation
     */
    @Nonnull
    public abstract ZscriptCommandNode<T> build();
}

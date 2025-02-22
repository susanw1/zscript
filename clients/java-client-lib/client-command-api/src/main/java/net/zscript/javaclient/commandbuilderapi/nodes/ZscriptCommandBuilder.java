package net.zscript.javaclient.commandbuilderapi.nodes;

import javax.annotation.Nonnull;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.util.stream.Collectors.joining;

import net.zscript.javaclient.commandbuilderapi.ZscriptMissingFieldException;
import net.zscript.javaclient.commandbuilderapi.ZscriptResponse;
import net.zscript.javaclient.commandpaths.BigField;
import net.zscript.model.components.Zchars;

/**
 * The builder for creating a ZscriptBuiltCommand.
 *
 * @param <T> the type of response this command would expect to receive
 */
public abstract class ZscriptCommandBuilder<T extends ZscriptResponse> {
    private static final int BIGFIELD_REQD_OFFSET = 26;

    ResponseCaptor<T> captor = null;
    final List<BigField>     bigFields = new ArrayList<>();
    final Map<Byte, Integer> fields    = new HashMap<>();

    /** Set of 26 numeric fields, and bigfield. Bits init'd on {@link #setRequiredFields(byte[])} and cleared when fields are set. */
    private final BitSet requiredFields = new BitSet();

    public ZscriptCommandBuilder<T> setField(byte key, int value) {
        if (!Zchars.isNumericKey(key)) {
            throw new IllegalArgumentException("Key not a valid Zscript Command key: " + (char) key);
        }
        fields.put(key, value);
        requiredFields.clear(key - 'A');
        return this;
    }

    public ZscriptCommandBuilder<T> setField(char key, int value) {
        return setField((byte) key, value);
    }

    protected int getField(byte key) {
        if (!Zchars.isNumericKey(key)) {
            throw new IllegalArgumentException("Key not a valid Zscript Command key: " + (char) key);
        }
        return fields.getOrDefault(key, 0);
    }

    protected int getField(char key) {
        return getField((byte) key);
    }

    protected ZscriptCommandBuilder<T> addBigField(byte[] data) {
        addBigFieldImpl(new BigField(data, false));
        return this;
    }

    protected ZscriptCommandBuilder<T> addBigField(byte[] data, boolean asString) {
        addBigFieldImpl(new BigField(data, asString));
        return this;
    }

    protected ZscriptCommandBuilder<T> addBigFieldAsSmallest(byte[] data) {
        int bigFieldPlusLen = data.length * 2 + 1;
        int bigFieldStrLen  = 2 + data.length;
        for (byte b : data) {
            if (Zchars.mustStringEscape(b)) {
                bigFieldStrLen += 2;
            }
        }
        addBigFieldImpl(new BigField(data, bigFieldStrLen < bigFieldPlusLen));
        return this;
    }

    protected ZscriptCommandBuilder<T> addBigField(String data) {
        addBigFieldImpl(new BigField(data.getBytes(StandardCharsets.UTF_8), true));
        return this;
    }

    private void addBigFieldImpl(BigField bigField) {
        bigFields.add(bigField);
        requiredFields.clear(BIGFIELD_REQD_OFFSET);
    }

    protected final void setRequiredFields(byte[] keys) {
        for (byte k : keys) {
            if (Zchars.isNumericKey(k)) {
                requiredFields.set(k - 'A');
            } else if (Zchars.isBigField(k)) {
                requiredFields.set(BIGFIELD_REQD_OFFSET);
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
                    .mapToObj(b -> "'" + (b == BIGFIELD_REQD_OFFSET ? "+" : String.valueOf((char) (b + 'A'))) + "'")
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

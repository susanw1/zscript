package net.zscript.javaclient.commandbuilder;

import static java.util.stream.Collectors.joining;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import net.zscript.javareceiver.tokenizer.Zchars;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;

public abstract class ZscriptCommandBuilder<T extends ZscriptResponse> {
    private static final int BIGFIELD_REQD_OFFSET = 26;

    private final List<ZscriptResponseListener<T>> listeners      = new ArrayList<>();
    private final List<BigField>                   bigFields      = new ArrayList<>();
    private final Map<Byte, Integer>               fields         = new HashMap<>();
    private final BitSet                           requiredFields = new BitSet();;

    public class ZscriptBuiltCommand extends ZscriptCommand {

        @Override
        public CommandSeqElement reEvaluate() {
            return this;
        }

        @Override
        public boolean canFail() {
            return commandCanFail();
        }

        @Override
        public boolean isCommand() {
            return true;
        }

        @Override
        public byte[] compile(boolean includeParens) {
            try {
                ByteArrayOutputStream out = new ByteArrayOutputStream();
                if (fields.get(Zchars.Z_CMD) != null) {
                    out.write(formatField(Zchars.Z_CMD, fields.get(Zchars.Z_CMD)));
                }
                for (Map.Entry<Byte, Integer> entry : fields.entrySet()) {
                    Byte key = entry.getKey();
                    if (key != Zchars.Z_CMD) {
                        Integer val = entry.getValue();
                        out.write(formatField(key, val));
                    }
                }
                for (BigField big : bigFields) {
                    big.write(out);
                }
                return out.toByteArray();
            } catch (IOException e) {
                throw new UncheckedIOException(e);
            }
        }

        @Override
        public void response(ZscriptExpression resp) {
            T parsed = parseResponse(resp);
            for (ZscriptResponseListener<T> listener : listeners) {
                listener.accept(parsed);
            }
        }

        @Override
        public void notExecuted() {
            for (ZscriptResponseListener<T> listener : listeners) {
                listener.notExecuted();
            }
        }
    }

    private class BigField {
        private final byte[]  data;
        private final boolean isString;

        public BigField(byte[] data, boolean isString) {
            this.data = data;
            this.isString = isString;
        }

        public void write(ByteArrayOutputStream out) {
            try {
                if (isString) {
                    out.write('"');
                    for (byte b : data) {
                        if (b == '"') {
                            out.write("=22".getBytes(StandardCharsets.UTF_8));
                        } else if (b == '=') {
                            out.write("=3d".getBytes(StandardCharsets.UTF_8));
                        } else if (b == '\n') {
                            out.write("=0a".getBytes(StandardCharsets.UTF_8));
                        } else if (b == '\0') {
                            out.write("=00".getBytes(StandardCharsets.UTF_8));
                        } else {
                            out.write(b);
                        }
                    }
                    out.write('"');
                } else {
                    out.write('+');
                    for (byte b : data) {
                        out.write(new byte[] { toHex(b >> 4), toHex(b & 0xF) });
                    }
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private static byte toHex(int nibble) {
        if (nibble < 10) {
            return (byte) (nibble + '0');
        }
        return (byte) (nibble + 'a' - 10);
    }

    public static byte[] formatField(byte f, int value) {
        if (value > 0x10000 || value < 0) {
            throw new IllegalStateException("Command fields must be uint16s");
        }
        if (value == 0) {
            return new byte[] { f };
        }
        if (value < 0x10) {
            return new byte[] { f, toHex(value) };
        }
        if (value < 0x100) {
            return new byte[] { f, toHex(value >> 4), toHex(value & 0xF) };
        }
        if (value < 0x1000) {
            return new byte[] { f, toHex(value >> 8), toHex((value >> 4) & 0xF), toHex(value & 0xF) };
        }
        return new byte[] { f, toHex(value >> 12), toHex((value >> 8) & 0xF), toHex((value >> 4) & 0xF), toHex(value & 0xF) };
    }

    public ZscriptCommandBuilder<T> setField(byte key, int value) {
        if (!Zchars.isNumericKey(key)) {
            throw new IllegalArgumentException("Key not a valid Zscript Command key: " + (char) key);
        }
        fields.put(key, value);
        requiredFields.clear(key - 'A');
        return this;
    }

    protected ZscriptCommandBuilder<T> setField(char key, int value) {
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
        addBigFieldImpl(new BigField(data, bigFieldStrLen > bigFieldPlusLen));
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

    /**
     * Checks that the supplied number can index the supplied Enum type, and throws a ZscriptFieldOutOfRangeException if it isn't.
     *
     * @param value     the value to check
     * @param enumClass the class of the enum to validate against
     * @param key       the letter key of the field being checked, for error msg purposes
     * @param isBitset  true if this enum represents a bitset, where values are really powers of 2; false if it's a normal enum
     * @return the value passed in, for nesting
     * @throws ZscriptFieldOutOfRangeException if value is out of range
     */
    protected static <E extends Enum<E>> int checkInEnumRange(int value, Class<E> enumClass, char key, boolean isBitset) {
        int max = enumClass.getEnumConstants().length;
        if (isBitset) {
            max = 1 << max;
        }
        if (value < 0 || value >= max) {
            throw new ZscriptFieldOutOfRangeException("name=%s, key='%c', value=0x%x, min=0x%x, max=0x%x", enumClass.getSimpleName(), key, value, 0, max - 1);
        }
        return value;
    }

    protected static <E extends Enum<E>> EnumSet<E> bitsetToEnumSet(int bitFields, Class<E> enumClass) {
        final E[] enumValues = enumClass.getEnumConstants();
        return BitSet.valueOf(new long[] { bitFields }).stream()
                .mapToObj(b -> enumValues[b])
                .collect(Collectors.toCollection(() -> EnumSet.noneOf(enumClass)));
    }

    protected abstract T parseResponse(ZscriptExpression resp);

    protected abstract boolean commandCanFail();

    public ZscriptCommandBuilder<T> addResponseListener(ZscriptResponseListener<T> listener) {
        listeners.add(listener);
        return this;
    }

    /**
     * Determines whether this command has all of its required fields set.
     *
     * @return true if required fields are all set, false otherwise
     */
    public boolean isValid() {
        return requiredFields.isEmpty();
    }

    /**
     * Builds the command.
     *
     * @return the command that has been built
     * @throws ZscriptMissingFieldException if builder doesn't yet pass validation
     */
    public ZscriptCommand build() {
        if (!isValid()) {
            String fieldList = requiredFields.stream()
                    .mapToObj(b -> "'" + (b == BIGFIELD_REQD_OFFSET ? "+" : String.valueOf((char) (b + 'A')) + "'"))
                    .collect(joining(","));
            throw new ZscriptMissingFieldException("missingKeys=%s", fieldList);
        }
        return new ZscriptBuiltCommand();
    }
}

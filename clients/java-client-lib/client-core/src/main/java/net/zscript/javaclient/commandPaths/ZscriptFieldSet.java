package net.zscript.javaclient.commandPaths;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.Set;
import java.util.stream.IntStream;
import java.util.stream.Stream;

import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toSet;
import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;
import static net.zscript.util.ByteString.byteString;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer;
import net.zscript.tokenizer.TokenBufferIterator;
import net.zscript.tokenizer.ZscriptExpression;
import net.zscript.tokenizer.ZscriptField;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

/**
 * Represents a set of fields making up a Zscript expression (single command or response) that can be parsed from Tokens.
 */
public final class ZscriptFieldSet implements ZscriptExpression, ByteAppendable {
    private static final Logger LOG   = LoggerFactory.getLogger(ZscriptFieldSet.class);
    private static final int    UNSET = -1;

    private final List<BigField> bigFields;
    private final int[]          fields;
    private final boolean        hasClash;

    public static ZscriptFieldSet fromTokens(TokenBuffer.TokenReader.ReadToken start) {
        final List<BigField> bigFields = new ArrayList<>();

        final int[] fields = new int[26];
        Arrays.fill(fields, UNSET);
        boolean hasClash = false;

        final TokenBufferIterator iterator = start.tokenIterator();
        for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
            final TokenBuffer.TokenReader.ReadToken token = opt.get();
            final byte                              key   = token.getKey();

            if (Zchars.isBigField(key)) {
                bigFields.add(new BigField(byteString(token.dataIterator()), key == Zchars.Z_BIGFIELD_QUOTED));
            } else {
                if (!Zchars.isNumericKey(key)) {
                    throw new ZscriptParseException("Invalid numeric key, must be 'A'-'Z' [key=0x%02x('%c')]", key, key);
                }
                if (fields[key - 'A'] != UNSET || token.getDataSize() > 2 || !Zchars.isNumericKey(key)) {
                    hasClash = true;
                } else {
                    fields[key - 'A'] = token.getData16();
                }
            }
        }
        return new ZscriptFieldSet(bigFields, fields, hasClash);
    }

    // FIXME - this can be improved, only used once
    public static ZscriptFieldSet fromMap(List<byte[]> inBigFields, List<Boolean> bigFieldStrings, Map<Byte, Integer> inFields) {
        final int[] fields = new int[26];
        Arrays.fill(fields, UNSET);
        for (final Map.Entry<Byte, Integer> e : inFields.entrySet()) {
            fields[e.getKey() - 'A'] = e.getValue();
        }
        if (inBigFields.size() != bigFieldStrings.size()) {
            throw new IllegalArgumentException();
        }
        final List<BigField> bigFields = new ArrayList<>();
        for (int i = 0; i < inBigFields.size(); i++) {
            bigFields.add(new BigField(inBigFields.get(i), bigFieldStrings.get(i)));
        }
        return new ZscriptFieldSet(bigFields, fields, false);
    }

    public static ZscriptFieldSet blank() {
        final int[] fields = new int[26];
        Arrays.fill(fields, UNSET);
        return new ZscriptFieldSet(emptyList(), fields, false);
    }

    private ZscriptFieldSet(List<BigField> bigFields, int[] fields, boolean hasClash) {
        this.bigFields = bigFields;
        this.fields = fields;
        this.hasClash = hasClash;
    }

    /**
     * @return true if this expression has no numeric fields and no big-fields; false otherwise
     */
    public boolean isEmpty() {
        if (!bigFields.isEmpty()) {
            return false;
        }
        for (final int i : fields) {
            if (i != UNSET) {
                return false;
            }
        }
        return true;
    }

    @Nonnull
    @Override
    public Stream<ZscriptField> fields() {
        return IntStream.range(0, fields.length).unordered()
                .filter(i -> fields[i] != UNSET)
                .mapToObj(i -> new NumberField((byte) (i + 'A'), fields[i]));
    }

    public int getFieldVal(byte key) {
        checkNumericKey(key);
        return fields[key - 'A'];
    }

    public void setFieldVal(byte key, int value) {
        checkNumericKey(key);
        fields[key - 'A'] = value;
    }

    private static void checkNumericKey(byte key) {
        if (!Zchars.isNumericKey(key)) {
            throw new IllegalArgumentException("numeric key must be A-Z, not " + key + "('" + (char) key + "')");
        }
    }

    /**
     * Aggregates all big-fields in this field-set and returns the concatenated result.
     *
     * @return concatenation of all associated big-field data
     */
    @Nonnull
    @Override
    public ByteString getBigFieldAsByteString() {
        return ByteString.concat((bigField, b) -> b.append(bigField.getDataAsByteString()), bigFields);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (fields['Z' - 'A'] != UNSET) {
            fieldOf(Zchars.Z_CMD, getFieldVal(Zchars.Z_CMD)).appendTo(builder);
        }
        for (byte key = 'A'; key < 'Z'; key++) {
            if (getFieldVal(key) != UNSET) {
                fieldOf(key, getFieldVal(key)).appendTo(builder);
            }
        }
        for (final BigField big : bigFields) {
            big.appendTo(builder);
        }
    }

    public int getBufferLength() {
        int length = 0;
        for (int field : fields) {
            if (field != UNSET) {
                if (field == 0) {
                    length += 2;
                } else if (field < 0x100) {
                    length += 3;
                } else {
                    length += 4;
                }
            }
        }
        for (final BigField big : bigFields) {
            length += big.getBufferLength();
        }
        return length;
    }

    @Nonnull
    @Override
    public OptionalInt getField(byte key) {
        return fields[key - 'A'] == UNSET ? OptionalInt.empty() : OptionalInt.of(fields[key - 'A']);
    }

    @Nonnull
    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        int v = getFieldVal(key);
        return Optional.ofNullable(v != UNSET ? new NumberField(key, v) : null);
    }

    @Override
    public int getFieldCount() {
        return (int) Arrays.stream(fields).filter(f -> f != UNSET).count();
    }

    @Override
    public boolean hasBigField() {
        return !bigFields.isEmpty();
    }

    @Override
    public int getBigFieldSize() {
        return bigFields.stream()
                .mapToInt(BigField::getDataLength)
                .sum();
    }

    /**
     * Builds a string description of the difference between this FieldSet and the supplied ZscriptExpression.
     *
     * @param other expression to compare with
     * @return a descriptive string if there are differences; empty otherwise
     */
    public String matchDescription(ZscriptExpression other) {
        final Set<NumberField> thisFields   = fields().map(NumberField::fieldOf).collect(toSet());
        final Set<NumberField> otherFields  = other.fields().map(NumberField::fieldOf).collect(toSet());
        final Set<NumberField> intersection = new HashSet<>(thisFields);
        intersection.retainAll(otherFields);
        thisFields.removeAll(intersection);
        otherFields.removeAll(intersection);

        ByteStringBuilder b = ByteString.builder(thisFields).appendUtf8(" <> ").append(otherFields);
        if (hasBigField() != other.hasBigField() || !getBigFieldAsByteString().equals(other.getBigFieldAsByteString())) {
            b.appendUtf8(" + ").append(getBigFieldAsByteString()).appendUtf8(" <> ").append(other.getBigFieldAsByteString());
        }
        return b.build().asString();
    }

    /**
     * Determines whether this set of fields is essentially same as those of the supplied ZscriptExpression.
     *
     * @param other expression to compare with
     * @return true is same; false otherwise
     */
    public boolean matches(ZscriptExpression other) {
        return matchDescription(other).isEmpty();
    }

    @Override
    public String toString() {
        return toStringImpl();
    }
}

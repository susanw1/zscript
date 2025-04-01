package net.zscript.javaclient.commandpaths;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.stream.Stream;

import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;
import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;
import static net.zscript.javaclient.commandpaths.FieldElement.fieldOfBytes;
import static net.zscript.util.ByteString.byteString;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.zscript.javaclient.ZscriptParseException;
import net.zscript.model.components.Zchars;
import net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;
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
    private static final Logger LOG = LoggerFactory.getLogger(ZscriptFieldSet.class);

    private final List<BigField> bigFields;
    private final FieldElement[] fields;
    private final boolean        hasClash;

    public static ZscriptFieldSet fromTokens(ReadToken start) {
        final List<BigField> bigFields = new ArrayList<>();

        final FieldElement[] fields   = new FieldElement[26];
        boolean              hasClash = false;

        final TokenBufferIterator iterator = start.tokenIterator();
        for (Optional<ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
            final ReadToken token = opt.get();
            final byte      key   = token.getKey();

            if (Zchars.isBigField(key)) {
                bigFields.add(new BigField(byteString(token.dataIterator()), key == Zchars.Z_BIGFIELD_QUOTED));
            } else {
                if (!Zchars.isExpressionKey(key)) {
                    throw new ZscriptParseException("Invalid numeric key, must be 'A'-'Z' [key=0x%02x('%c')]", key, key);
                }
                if (fields[key - 'A'] != null) {
                    hasClash = true;
                } else {
                    fields[key - 'A'] = fieldOfBytes(key, token.dataIterator().toByteString());
                }
            }
        }
        return new ZscriptFieldSet(bigFields, fields, hasClash);
    }

    public static ZscriptFieldSet fromList(List<BigField> bigFields, List<FieldElement> inFields) {
        final FieldElement[] fields = new FieldElement[26];
        for (final FieldElement e : inFields) {
            fields[e.getKey() - 'A'] = e;
        }
        return new ZscriptFieldSet(bigFields, fields, false);
    }

    public static ZscriptFieldSet blank() {
        final FieldElement[] fields = new FieldElement[26];
        return new ZscriptFieldSet(emptyList(), fields, false);
    }

    private ZscriptFieldSet(List<BigField> bigFields, FieldElement[] fields, boolean hasClash) {
        this.bigFields = List.copyOf(bigFields);
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
        for (final FieldElement i : fields) {
            if (i != null) {
                return false;
            }
        }
        return true;
    }

    @Nonnull
    @Override
    public Stream<? extends ZscriptField> fields() {
        return Arrays.stream(fields)
                .filter(Objects::nonNull);
    }

    @Nullable
    public FieldElement getFieldVal(byte key) {
        checkFieldKey(key);
        return fields[key - 'A'];
    }

    private static void checkFieldKey(byte key) {
        if (!Zchars.isExpressionKey(key)) {
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
    @Deprecated
    public ByteString getBigFieldAsByteString() {
        return ByteString.concat((bigField, b) -> b.append(bigField.getDataAsByteString()), bigFields);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        // output Z-field first, for prettiness
        if (fields['Z' - 'A'] != null) {
            fields['Z' - 'A'].appendTo(builder);
        }
        for (byte key = 'A'; key < 'Z'; key++) {
            if (getFieldVal(key) != null) {
                getFieldVal(key).appendTo(builder);
            }
        }
        for (final BigField big : bigFields) {
            big.appendTo(builder);
        }
    }

    public int getBufferLength() {
        ByteStringBuilder b = ByteString.builder();
        for (FieldElement field : fields) {
            if (field != null) {
                b.append(field);
            }
        }
        int length = b.size();
        for (final BigField big : bigFields) {
            length += big.getBufferLength();
        }
        return length;
    }

    @Nonnull
    @Override
    public OptionalInt getField(byte key) {
        return getZscriptField(key).stream().mapToInt(ZscriptField::getValue).findFirst();
    }

    @Nonnull
    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        return ofNullable(getFieldVal(key));
    }

    @Override
    public int getFieldCount() {
        return (int) Arrays.stream(fields).filter(Objects::nonNull).count();
    }

    @Override
    @Deprecated
    public boolean hasBigField() {
        return !bigFields.isEmpty();
    }

    @Override
    @Deprecated
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
        final Map<Byte, FieldElement> thisFields  = fields().map(FieldElement::fieldOf).collect(toMap(FieldElement::getKey, identity()));
        final Map<Byte, FieldElement> otherFields = other.fields().map(FieldElement::fieldOf).collect(toMap(FieldElement::getKey, identity()));

        removeMatches(thisFields, otherFields);
        removeMatches(otherFields, thisFields);

        final ByteStringBuilder b = ByteString.builder();
        if (!thisFields.isEmpty() || !otherFields.isEmpty()) {
            b.append(thisFields.values()).appendUtf8(" <> ").append(otherFields.values());
        }
        if (hasBigField() != other.hasBigField() || !getBigFieldAsByteString().equals(other.getBigFieldAsByteString())) {
            b.appendUtf8(" + ").append(getBigFieldAsByteString()).appendUtf8(" <> ").append(other.getBigFieldAsByteString());
        }
        return b.build().asString();
    }

    private static void removeMatches(Map<Byte, FieldElement> m1, Map<Byte, FieldElement> m2) {
        for (Iterator<FieldElement> it = m1.values().iterator(); it.hasNext(); ) {
            FieldElement       el = it.next();
            final FieldElement f  = m2.get(el.getKey());
            if (f != null && el.matches(f)) {
                it.remove();
                m2.remove(el.getKey());
            }
        }
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

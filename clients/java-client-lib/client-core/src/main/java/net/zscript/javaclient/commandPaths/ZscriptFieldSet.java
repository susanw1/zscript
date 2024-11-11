package net.zscript.javaclient.commandPaths;

import javax.annotation.Nonnull;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;

import static java.util.Collections.emptyList;
import static net.zscript.javaclient.commandPaths.NumberField.fieldOf;
import static net.zscript.util.ByteString.byteString;

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

    private final List<BigField> bigFields;
    private final int[]          fields;
    private final boolean        hasClash;

    public static ZscriptFieldSet fromTokens(TokenBuffer.TokenReader.ReadToken start) {
        List<BigField> bigFields = new ArrayList<>();

        int[] fields = new int[26];
        Arrays.fill(fields, -1);
        boolean hasClash = false;

        TokenBufferIterator iterator = start.tokenIterator();
        for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
            TokenBuffer.TokenReader.ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                bigFields.add(new BigField(byteString(token.dataIterator()), token.getKey() == Zchars.Z_BIGFIELD_QUOTED));
            } else {
                if (fields[token.getKey() - 'A'] != -1 || token.getDataSize() > 2 || !Zchars.isNumericKey(token.getKey())) {
                    hasClash = true;
                } else {
                    fields[token.getKey() - 'A'] = token.getData16();
                }
            }
        }
        return new ZscriptFieldSet(bigFields, fields, hasClash);
    }

    // FIXME - this can be improved, only used once
    public static ZscriptFieldSet fromMap(List<byte[]> inBigFields, List<Boolean> bigFieldStrings, Map<Byte, Integer> inFields) {
        int[] fields = new int[26];
        Arrays.fill(fields, -1);
        for (Map.Entry<Byte, Integer> e : inFields.entrySet()) {
            fields[e.getKey() - 'A'] = e.getValue();
        }
        if (inBigFields.size() != bigFieldStrings.size()) {
            throw new IllegalArgumentException();
        }
        List<BigField> bigFields = new ArrayList<>();
        for (int i = 0; i < inBigFields.size(); i++) {
            bigFields.add(new BigField(inBigFields.get(i), bigFieldStrings.get(i)));
        }
        return new ZscriptFieldSet(bigFields, fields, false);
    }

    public static ZscriptFieldSet blank() {
        int[] fields = new int[26];
        Arrays.fill(fields, -1);
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
        for (int i : fields) {
            if (i != -1) {
                return false;
            }
        }
        return true;
    }

    public int getFieldVal(byte key) {
        return fields[key - 'A'];
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
        if (fields['Z' - 'A'] != -1) {
            fieldOf(Zchars.Z_CMD, fields['Z' - 'A']).appendTo(builder);
        }
        for (int i = 0; i < fields.length; i++) {
            if (i + 'A' != 'Z' && fields[i] != -1) {
                fieldOf((byte) (i + 'A'), fields[i]).appendTo(builder);
            }
        }
        for (BigField big : bigFields) {
            big.appendTo(builder);
        }
    }

    public int getBufferLength() {
        int length = 0;
        for (int field : fields) {
            if (field != -1) {
                if (field == 0) {
                    length += 2;
                } else if (field < 0x100) {
                    length += 3;
                } else {
                    length += 4;
                }
            }
        }
        for (BigField big : bigFields) {
            length += big.getBufferLength();
        }
        return length;
    }

    @Nonnull
    @Override
    public OptionalInt getField(byte key) {
        return fields[key - 'A'] == -1 ? OptionalInt.empty() : OptionalInt.of(fields[key - 'A']);
    }

    @Nonnull
    @Override
    public Optional<? extends ZscriptField> getZscriptField(byte key) {
        int v = fields[key - 'A'];
        return Optional.ofNullable(v != -1 ? new NumberField(key, v) : null);
    }

    @Override
    public int getFieldCount() {
        return (int) Arrays.stream(fields).filter(f -> f != -1).count();
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
}

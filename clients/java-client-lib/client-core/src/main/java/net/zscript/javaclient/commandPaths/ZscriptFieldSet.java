package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class ZscriptFieldSet implements ZscriptExpression, ByteAppendable {

    private final List<BigField> bigFields;
    private final int[]          fields;
    private final boolean        hasClash;

    public static ZscriptFieldSet fromTokens(TokenBuffer.TokenReader.ReadToken start) {
        List<BigField> bigFields = new ArrayList<>();

        int[] fields = new int[26];
        Arrays.fill(fields, -1);
        boolean hasClash = false;

        TokenBufferIterator iterator = start.getNextTokens();
        for (Optional<TokenBuffer.TokenReader.ReadToken> opt = iterator.next(); opt.filter(t -> !t.isMarker()).isPresent(); opt = iterator.next()) {
            TokenBuffer.TokenReader.ReadToken token = opt.get();
            if (Zchars.isBigField(token.getKey())) {
                byte[] data = new byte[token.getDataSize()];

                int i = 0;
                for (Iterator<Byte> iter = token.blockIterator(); iter.hasNext(); ) {
                    data[i++] = iter.next();
                }
                bigFields.add(new BigField(data, token.getKey() == Zchars.Z_BIGFIELD_QUOTED));
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
        return new ZscriptFieldSet(new ArrayList<>(), fields, false);
    }

    private ZscriptFieldSet(List<BigField> bigFields, int[] fields, boolean hasClash) {
        this.bigFields = bigFields;
        this.fields = fields;
        this.hasClash = hasClash;
    }

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

    public int getFieldValue(byte key) {
        return fields[key - 'A'];
    }

    public int getFieldValue(char key) {
        return fields[key - 'A'];
    }

    /**
     * Aggregates all big-fields in this field-set and returns the concatenated result.
     *
     * @return concatenation of all associated big-field data
     */
    public byte[] getBigFieldData() {
        ByteStringBuilder out = ByteString.builder();
        for (BigField big : bigFields) {
            out.appendRaw(big.getData());
        }
        return out.toByteArray();
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (fields['Z' - 'A'] != -1) {
            builder.appendByte(Zchars.Z_CMD).appendNumeric(fields['Z' - 'A']);
        }
        for (int i = 0; i < fields.length; i++) {
            if (i != 'Z' - 'A' && fields[i] != -1) {
                builder.appendByte((byte) (i + 'A')).appendNumeric(fields[i]);
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
            length += big.getDataLength() + 2;
        }
        return length;
    }

    public OptionalInt getField(byte f) {
        return fields[f - 'A'] == -1 ? OptionalInt.empty() : OptionalInt.of(fields[f - 'A']);
    }

    public int getFieldCount() {
        int count = 0;
        for (int field : fields) {
            if (field != -1) {
                count++;
            }
        }
        return count;
    }

    public boolean hasBigField() {
        return !bigFields.isEmpty();
    }

    public int getBigFieldSize() {
        int len = 0;
        for (BigField big : bigFields) {
            len += big.getDataLength();
        }
        return len;
    }
}

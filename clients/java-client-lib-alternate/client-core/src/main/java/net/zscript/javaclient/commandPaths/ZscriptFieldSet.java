package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Optional;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public class ZscriptFieldSet {

    static class BigField {
        private final byte[]  data;
        private final boolean isString;

        BigField(byte[] data, boolean isString) {
            this.data = data;
            this.isString = isString;
        }

        public byte[] getData() {
            return data;
        }

        public boolean isString() {
            return isString;
        }

        public void output(ZscriptByteString.ZscriptByteStringBuilder out) {
            if (isString) {
                out.appendBigfieldText(data);
            } else {
                out.appendBigfieldBytes(data);
            }
        }
    }

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

    public List<BigField> getBigFields() {
        return bigFields;
    }

    public byte[] getBigFieldTotal() {
        ByteString.ByteStringBuilder out = ByteString.builder();
        for (BigField big : bigFields) {
            out.appendRaw(big.getData());
        }
        return out.toByteArray();
    }

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder out) {
        if (fields['Z' - 'A'] != -1) {
            out.appendField(Zchars.Z_CMD, fields['Z' - 'A']);
        }
        for (int i = 0; i < fields.length; i++) {
            if (i != 'Z' - 'A' && fields[i] != -1) {
                out.appendField((byte) (i + 'A'), fields[i]);
            }

        }
        for (BigField big : bigFields) {
            big.output(out);
        }
    }

}
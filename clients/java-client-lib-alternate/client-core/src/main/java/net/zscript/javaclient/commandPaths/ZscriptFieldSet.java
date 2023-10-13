package net.zscript.javaclient.commandPaths;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.OptionalInt;

import net.zscript.javaclient.commandbuilder.AbortCommandNode;
import net.zscript.javaclient.commandbuilder.BlankCommandNode;
import net.zscript.javaclient.commandbuilder.FailureCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptBuiltCommandNode;
import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.javaclient.commandbuilder.ZscriptCommandBuilder;
import net.zscript.javaclient.commandbuilder.ZscriptCommandNode;
import net.zscript.javareceiver.tokenizer.BlockIterator;
import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.javareceiver.tokenizer.ZscriptExpression;
import net.zscript.model.components.Zchars;
import net.zscript.model.components.ZscriptStatus;
import net.zscript.util.ByteString;

public class ZscriptFieldSet implements ZscriptExpression {

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

    public static ZscriptFieldSet from(ZscriptCommandNode node) {
        int[] fields = new int[26];
        Arrays.fill(fields, -1);
        List<BigField> bigFields = new ArrayList<>();
        if (node instanceof ZscriptBuiltCommandNode) {
            ZscriptBuiltCommandNode<?> source = (ZscriptBuiltCommandNode<?>) node;
            for (Map.Entry<Byte, Integer> e : source.getFields().entrySet()) {
                fields[e.getKey() - 'A'] = e.getValue();
            }
            for (ZscriptCommandBuilder.BigField b : source.getBigFields()) {
                bigFields.add(new BigField(b.getData(), b.isString()));
            }
        } else if (node instanceof BlankCommandNode) {
        } else if (node instanceof FailureCommandNode) {
            fields[Zchars.Z_CMD - 'A'] = 1;
            fields[Zchars.Z_STATUS - 'A'] = ZscriptStatus.COMMAND_FAIL_CONTROL;
        } else if (node instanceof AbortCommandNode) {
            fields[Zchars.Z_CMD - 'A'] = 1;
            fields[Zchars.Z_STATUS - 'A'] = ZscriptStatus.COMMAND_ERROR_CONTROL;
        } else {
            throw new IllegalStateException("Unknown ZscriptCommandNode type: " + node);
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

    public byte[] getBigFieldData() {
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
            length += big.data.length + 2;
        }
        return length;
    }

    public OptionalInt getField(byte f) {
        return fields[f - 'A'] == -1 ? OptionalInt.empty() : OptionalInt.of(fields[f - 'A']);
    }

    public int getFieldCount() {
        int count = 0;
        for (int i = 0; i < fields.length; i++) {
            if (fields[i] != -1) {
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
            len += big.getData().length;
        }
        return len;

    }

}

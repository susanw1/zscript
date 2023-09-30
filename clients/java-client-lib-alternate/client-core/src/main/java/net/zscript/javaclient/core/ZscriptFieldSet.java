package net.zscript.javaclient.core;

import java.io.IOException;
import java.io.OutputStream;
import java.io.UncheckedIOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import net.zscript.javareceiver.tokenizer.TokenBuffer;
import net.zscript.javareceiver.tokenizer.TokenBufferIterator;
import net.zscript.model.components.Zchars;

public class ZscriptFieldSet {
    public static int hexchar(byte nibble) {
        if (nibble > 9) {
            return 'a' + nibble - 10;
        } else {
            return '0' + nibble;
        }
    }

    public static void hexString(OutputStream out, int value) {
        try {
            if (value > 0xFFFF) {
                throw new IllegalStateException();
            }
            if (value > 0xFFF) {
                out.write(hexchar((byte) ((value >> 12) & 0xF)));
            }
            if (value > 0xFF) {
                out.write(hexchar((byte) ((value >> 8) & 0xF)));
            }
            if (value > 0xF) {
                out.write(hexchar((byte) ((value >> 4) & 0xF)));
            }
            if (value > 0) {
                out.write(hexchar((byte) (value & 0xF)));
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private final List<byte[]> bigFields;
    private final int[]        fields;
    private final boolean      hasClash;

    public static ZscriptFieldSet fromTokens(TokenBuffer.TokenReader.ReadToken start) {
        List<byte[]> bigFields = new ArrayList<>();

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
                bigFields.add(data);
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

    private ZscriptFieldSet(List<byte[]> bigFields, int[] fields, boolean hasClash) {
        this.bigFields = bigFields;
        this.fields = fields;
        this.hasClash = hasClash;
    }

    public int getFieldValue(byte key) {
        return fields[key - 'A'];
    }

    public int getFieldValue(char key) {
        return fields[key - 'A'];
    }

    public List<byte[]> getBigFields() {
        return bigFields;
    }

    public void toBytes(OutputStream out) {
        try {
            if (fields['Z' - 'A'] != -1) {
                out.write('Z');
                hexString(out, fields['Z' - 'A']);
            }
            for (int i = 0; i < fields.length; i++) {
                if (i != 'Z' - 'A' && fields[i] != -1) {
                    out.write(i + 'A');
                    hexString(out, fields[i]);
                }

            }
            for (byte[] bytes : bigFields) {
                out.write('+');
                for (byte b : bytes) {
                    out.write(hexchar((byte) (b >> 4)));
                    out.write(hexchar((byte) (b & 0xF)));
                }
            }
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }

}

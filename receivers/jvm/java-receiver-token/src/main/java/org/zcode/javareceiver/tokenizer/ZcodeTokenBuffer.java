package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenBuffer implements TokenBuffer {
    public final byte FIELD_EXTENSION = (byte) 0xE0;

    byte[] data;
    int    readLimit;
    int    writeLastLen;
    int    writePos;

    boolean inNibble = false;
    boolean numeric  = false;

    public static ZcodeTokenBuffer createBufferWithCapacity(int sz) {
        return new ZcodeTokenBuffer(sz);
    }

    // construct via static factories
    private ZcodeTokenBuffer(int sz) {
        data = new byte[sz];
    }

    // Visible for testing only!
    byte[] getInternalData() {
        return data.clone();
    }

    public int offset(int pos, int offset) {
        pos += offset;
        if (pos >= data.length) {
            pos = 0;
        }
        return pos;
    }

    public TokenBuffer fail(byte code) {
        closeToken();
        data[writePos] = 1;
        writePos = offset(writePos, 1);
        data[writePos] = (byte) 0xFF;
        writePos = offset(writePos, 1);
        data[writePos] = code;
        writePos = offset(writePos, 1);
        return this;
    }

    public TokenBuffer startToken(byte key, boolean numeric) {
        closeToken();
        this.numeric = numeric;
        data[writePos] = 0;
        writePos = offset(writePos, 1);
        data[writePos] = key;
        writePos = offset(writePos, 1);
        inNibble = false;
        return this;
    }

    private void extendField() {
        data[writePos] = 0;
        writeLastLen = writePos;
        writePos = offset(writePos, 1);
        data[writePos] = FIELD_EXTENSION;
        writePos = offset(writePos, 1);
    }

    public TokenBuffer continueTokenNibble(byte nibble) {
        if (writePos == readLimit) {
            throw new IllegalStateException("Can't write byte without field");
        }
        if (inNibble) {
            data[writePos] |= nibble;
            writePos = offset(writePos, 1);
        } else {
            if (data[writeLastLen] == 0xFF) {
                extendField();
            }
            data[writePos] = (byte) (nibble << 4);
            data[writeLastLen]++;
        }
        inNibble = !inNibble;
        return this;
    }

    public TokenBuffer continueTokenByte(byte b) {
        if (inNibble) {
            throw new IllegalStateException("Can't write byte while in nibble");
        }
        if (writePos == readLimit) {
            throw new IllegalStateException("Can't write byte without field");
        }
        if (data[writeLastLen] == 0xFF) {
            extendField();
        }
        data[writePos] = b;
        writePos = offset(writePos, 1);
        data[writeLastLen]++;
        return this;
    }

    public TokenBuffer closeToken() {
        if (numeric && inNibble) {
            if (writeLastLen != readLimit) {
                throw new IllegalStateException("Can't cope with numeric fields longer than 255 bytes");
            }
            byte hold = 0;
            int  pos  = offset(readLimit, 1);
            do {
                pos = offset(pos, 1);
                byte tmp = (byte) (data[pos] & 0xF);
                data[pos] = (byte) (hold | (data[pos] >> 4) & 0xF);
                hold = (byte) (tmp << 4);
            } while (pos != writePos);
        }
        writeLastLen = writePos;
        readLimit = writePos;
        inNibble = false;
        return this;
    }

    public boolean isInNibble() {
        return inNibble;
    }

    public int getCurrentWriteFieldKey() {
        return data[offset(readLimit, 1)];
    }

    public int getCurrentWriteFieldLength() {
        return data[readLimit];
    }

    public int getCurrentWriteFieldNibbleLength() {
        return data[readLimit] * 2 - (inNibble ? 1 : 0);
    }

    public boolean isFieldOpen() {
        return readLimit != writePos;
    }
}

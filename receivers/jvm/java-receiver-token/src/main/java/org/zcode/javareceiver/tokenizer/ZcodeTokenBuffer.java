package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenBuffer implements TokenBuffer {
    byte[] data;

    int writeLimit;
    int readLimit;
    int writePos;

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

    public int next(int pos) {
        pos++;
        if (pos >= data.length) {
            pos = 0;
        }
        return pos;
    }

    public TokenBuffer fail(byte code) {
        closeToken();
        data[writePos] = 1;
        writePos = next(writePos);
        data[writePos] = (byte) 0xFF;
        writePos = next(writePos);
        data[writePos] = code;
        writePos = next(writePos);
        return this;
    }

    public TokenBuffer startToken(byte key, boolean numeric) {
        closeToken();
        this.numeric = numeric;
        data[writePos] = 0;
        writePos = next(writePos);
        data[writePos] = key;
        writePos = next(writePos);
        inNibble = false;
        return this;
    }

    public TokenBuffer continueTokenNibble(byte nibble) {
        if (inNibble) {
            data[writePos] |= nibble;
            writePos = next(writePos);
        } else {
            data[writePos] = (byte) (nibble << 4);
            data[readLimit]++;
        }
        inNibble = !inNibble;
        return this;
    }

    public TokenBuffer continueTokenByte(byte b) {
        data[writePos] = b;
        writePos = next(writePos);
        data[readLimit]++;
        return this;
    }

    public TokenBuffer closeToken() {
        if (numeric && inNibble) {
            byte hold = 0;
            int  pos  = next(readLimit);
            do {
                pos = next(pos);
                byte tmp = (byte) (data[pos] & 0xF);
                data[pos] = (byte) (hold | (data[pos] >> 4) & 0xF);
                hold = (byte) (tmp << 4);
            } while (pos != writePos);
        }
        readLimit = writePos;
        inNibble = false;
        return this;
    }

    public boolean isInNibble() {
        return inNibble;
    }

    public int getCurrentWriteFieldKey() {
        return data[next(readLimit)];
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

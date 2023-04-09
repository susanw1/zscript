package org.zcode.javareceiver.tokeniser;

public class ZcodeTokenBuffer {
    byte[] data;
    int readLimit;
    int writePos;
    boolean inNibble = false;
    boolean numeric = false;

    private int next(int pos) {
        pos++;
        if (pos >= data.length) {
            pos = 0;
        }
        return pos;
    }

    public void fail(byte code) {
        closeField();
        data[writePos] = 1;
        writePos = next(writePos);
        data[writePos] = (byte) 0xFF;
        writePos = next(writePos);
        data[writePos] = code;
        writePos = next(writePos);
    }

    public void startField(byte key, boolean numeric) {
        closeField();
        this.numeric = numeric;
        data[writePos] = 0;
        writePos = next(writePos);
        data[writePos] = key;
        writePos = next(writePos);
        inNibble = false;
    }

    public void continueFieldNibble(byte nibble) {
        if (inNibble) {
            data[writePos] |= nibble;
            writePos = next(writePos);
        } else {
            data[writePos] = (byte) (nibble << 4);
            data[readLimit]++;
        }
        inNibble = !inNibble;
    }

    public void continueFieldByte(byte b) {
        data[writePos] = b;
        writePos = next(writePos);
        data[readLimit]++;
    }

    public void closeField() {
        if (numeric && inNibble) {
            byte hold = 0;
            int pos = next(next(readLimit));
            while (pos != writePos) {
                byte tmp = (byte) (data[pos] & 0xF);
                data[pos] = (byte) (hold | (data[pos] >> 4));
                hold = (byte) (tmp << 4);
                pos = next(pos);
            }
        }
        readLimit = writePos;
        inNibble = false;
    }

    public boolean isInNibble() {
        return inNibble;
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

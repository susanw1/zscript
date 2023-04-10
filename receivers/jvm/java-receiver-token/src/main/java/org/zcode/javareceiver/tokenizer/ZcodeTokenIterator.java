package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenIterator {
    private ZcodeTokenRingBuffer buffer = null;

    private int     position = 0;
    private byte    key      = 0;
    private boolean valid    = false;

    public void set(ZcodeTokenRingBuffer buffer, int position) {
        this.buffer = buffer;
        this.position = position;
        if (position == buffer.writeStart) {
            this.key = 0;
            this.valid = false;
        } else {
            this.key = buffer.data[buffer.offset(position, 1)];
            this.valid = true;
        }
    }

    public byte getCurrentKey() {
        return key;
    }

    public byte next() {
        do {
            position = buffer.offset(position, buffer.data[position] + 2);
            if (position == buffer.writeStart) {
                key = 0;
                valid = false;
                return 0;
            }
            key = buffer.data[buffer.offset(position, 1)];
        } while (key == ZcodeTokenRingBuffer.TOKEN_EXTENSION);
        valid = true;
        return key;
    }

    public boolean isValid() {
        return valid;
    }

    public void setTokenDataIterator(ZcodeTokenDataIterator iterator) {
        iterator.set(buffer, key, position);
    }

    public int getNumericValue() {
        byte len = buffer.data[position];
        if (len > 2 || len < 0) {
            throw new IllegalStateException("Token too long to get numeric value");
        }
        if (len == 0) {
            return 0;
        } else if (len == 1) {
            return buffer.data[buffer.offset(position, 2)];
        } else {
            return buffer.data[buffer.offset(position, 2)] << 8 | buffer.data[buffer.offset(position, 3)];
        }
    }

    public void moveReadStart() {
        buffer.readStart = position;
    }
}

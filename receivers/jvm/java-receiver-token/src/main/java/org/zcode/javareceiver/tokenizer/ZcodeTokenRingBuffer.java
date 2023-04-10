package org.zcode.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

public class ZcodeTokenRingBuffer implements ZcodeTokenBuffer {
    public static final byte TOKEN_EXTENSION      = (byte) 0x81;
    public static final byte BUFFER_OVERRUN_ERROR = (byte) 0xF0;

    /**
     * Zcode shouldn't need huge buffers, so 64K is our extreme limit. It should be addressable by uint16 indexes.
     * Note that *exact* 64K size implies that data.length cannot be held in a uint16, so careful code required if porting!
     */
    private static final int MAX_RING_BUFFER_SIZE = 0x1_0000;

    /** the ring-buffer's data array */
    byte[] data;

    /** index of first byte that readers own, defines the upper limit to writeable space */
    int         readStart    = 0;
    /** index of first byte that the writer owns, defines the upper limit to readable space */
    int         writeStart   = 0;
    /** start index of most recent token segment (esp required for long multi-segment tokens) */
    private int writeLastLen = 0;
    /** the current write index into data array */
    private int writePos     = 0;

    boolean inNibble = false;
    boolean numeric  = false;

    public static ZcodeTokenRingBuffer createBufferWithCapacity(int sz) {
        return new ZcodeTokenRingBuffer(sz);
    }

    // construct via static factories
    private ZcodeTokenRingBuffer(int sz) {
        if (sz > MAX_RING_BUFFER_SIZE) {
            throw new IllegalArgumentException(format("size too big [sz={}, max={}]", sz, MAX_RING_BUFFER_SIZE));
        }
        data = new byte[sz];
    }

    // Visible for testing only!
    byte[] getInternalData() {
        return data.clone();
    }

    int offset(final int pos, final int offset) {
        if (pos < 0 || offset < 0 || offset >= data.length) {
            throw new IllegalArgumentException(format("Unexpected values [pos={}, offset={}]", pos, offset));
        }
        return (pos + offset) % data.length;
    }

    private void writeAndMove2(byte b1, byte b2) {
        data[writePos] = b1;
        writePos = offset(writePos, 1);
        data[writePos] = b2;
        writePos = offset(writePos, 1);
    }

    private void failInternal(final byte errorCode) {
        closeToken();
        writeAndMove2((byte) 0, errorCode);
        writeLastLen = writePos;
        writeStart = writePos;
        inNibble = false;
    }

    @Override
    public boolean fail(byte errorCode) {
        if (getAvailableWrite() < 6) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        failInternal(errorCode);
        return true;
    }

    public int getAvailableWrite() {
        if (writePos >= readStart) {
            return data.length - writePos + readStart - 1;
        } else {
            return readStart - writePos - 1;
        }
    }

    @Override
    public boolean startToken(byte key, boolean numeric) {
        if (getAvailableWrite() < 4) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        closeToken();
        this.numeric = numeric;
        writeAndMove2((byte) 0, key);
        inNibble = false;
        return true;
    }

    private void extendToken() {
        writeLastLen = writePos;
        writeAndMove2((byte) 0, TOKEN_EXTENSION);
    }

    @Override
    public boolean continueTokenNibble(byte nibble) {
        if (writePos == writeStart) {
            throw new IllegalStateException("Can't write byte without field");
        }

        if (!inNibble && (getAvailableWrite() < 3 || data[writeLastLen] == 0xFF && getAvailableWrite() < 5)) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        if (inNibble) {
            data[writePos] |= nibble;
            writePos = offset(writePos, 1);
        } else {
            if (data[writeLastLen] == 0xFF) {
                extendToken();
            }
            data[writePos] = (byte) (nibble << 4);
            data[writeLastLen]++;
        }
        inNibble = !inNibble;
        return true;
    }

    @Override
    public boolean continueTokenByte(byte b) {
        if (inNibble) {
            throw new IllegalStateException("Incomplete nibble");
        }
        if (writePos == writeStart) {
            throw new IllegalStateException("Can't write byte without field");
        }

        if (getAvailableWrite() < 3 || data[writeLastLen] == 0xFF && getAvailableWrite() < 5) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        if (data[writeLastLen] == 0xFF) {
            extendToken();
        }
        data[writePos] = b;
        writePos = offset(writePos, 1);
        data[writeLastLen]++;
        return true;
    }

    @Override
    public void closeToken() {
        if (inNibble) {
            if (numeric) {
                if (writeLastLen != writeStart) {
                    throw new IllegalStateException("Illegal numeric field longer than 255 bytes");
                }
                byte hold = 0;
                int  pos  = offset(writeStart, 1);
                do {
                    pos = offset(pos, 1);
                    byte tmp = (byte) (data[pos] & 0xF);
                    data[pos] = (byte) (hold | (data[pos] >> 4) & 0xF);
                    hold = (byte) (tmp << 4);
                } while (pos != writePos);
            }
            writePos = offset(writePos, 1);
        }
        writeLastLen = writePos;
        writeStart = writePos;
        inNibble = false;
    }

    @Override
    public boolean isInNibble() {
        return inNibble;
    }

    @Override
    public int getCurrentWriteTokenKey() {
        return data[offset(writeStart, 1)];
    }

    @Override
    public int getCurrentWriteTokenLength() {
        return data[writeStart];
    }

    @Override
    public int getCurrentWriteTokenNibbleLength() {
        return data[writeStart] * 2 - (inNibble ? 1 : 0);
    }

    @Override
    public boolean isTokenOpen() {
        return writeStart != writePos;
    }

    public void setIterator(ZcodeTokenIterator iterator) {
        iterator.set(this, readStart);
    }
}

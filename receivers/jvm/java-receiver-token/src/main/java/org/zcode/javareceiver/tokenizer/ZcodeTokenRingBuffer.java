package org.zcode.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

public class ZcodeTokenRingBuffer implements ZcodeTokenBuffer {
    // TODO: Raise flag on error and on newline

    /**
     * Zcode shouldn't need huge buffers, so 64K is our extreme limit. It should be addressable by uint16 indexes. Note that *exact* 64K size implies that data.length cannot be
     * held in a uint16, so careful code required if porting!
     */
    private static final int  MAX_RING_BUFFER_SIZE = 0x1_0000;
    private static final byte MAX_DATA_LENGTH      = (byte) 255;

    /** the ring-buffer's data array */
    byte[] data;

    /** index of first byte that readers own, defines the upper limit to writable space */
    int         readStart    = 0;
    /** index of first byte that the writer owns, defines the upper limit to readable space */
    int         writeStart   = 0;
    /** start index of most recent token segment (esp required for long multi-segment tokens) */
    private int writeLastLen = 0;
    /** the current write index into data array */
    private int writeCursor  = 0;

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

    /**
     * Utility method that performs wrap-around for a lookup into the ring-buffer (ie modulo length)
     * 
     * @param index  location in the underlying ring-buffer
     * @param offset an offset to add, >= 0
     * @return a new index, at the requested offset
     */
    int offset(final int index, final int offset) {
        if (index < 0 || offset < 0 || offset >= data.length) {
            throw new IllegalArgumentException(format("Unexpected values [pos={}, offset={}]", index, offset));
        }
        return (index + offset) % data.length;
    }

    private void writeNewTokenStart(byte key) {
        data[writeCursor] = 0;
        writeCursor = offset(writeCursor, 1);
        data[writeCursor] = key;
        writeCursor = offset(writeCursor, 1);
    }

    private void failInternal(final byte errorCode) {
        endToken();
        writeNewTokenStart(errorCode);
        endToken();
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

    @Override
    public int getAvailableWrite() {
        return (writeCursor >= readStart ? data.length : 0) + readStart - writeCursor - 1;
    }

    @Override
    public boolean startToken(byte key, boolean numeric) {
        if (getAvailableWrite() < 4) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        endToken();
        this.numeric = numeric;
        writeNewTokenStart(key);
        inNibble = false;
        return true;
    }

    private void startNewSegment() {
        writeLastLen = writeCursor;
        writeNewTokenStart(TOKEN_EXTENSION);
    }

    @Override
    public boolean continueTokenNibble(byte nibble) {
        if (writeCursor == writeStart) {
            throw new IllegalStateException("Digit with missing field key");
        }

        if (!inNibble && (getAvailableWrite() < 3 || data[writeLastLen] == MAX_DATA_LENGTH && getAvailableWrite() < 5)) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        if (inNibble) {
            data[writeCursor] |= nibble;
            writeCursor = offset(writeCursor, 1);
        } else {
            if (data[writeLastLen] == MAX_DATA_LENGTH) {
                startNewSegment();
            }
            data[writeCursor] = (byte) (nibble << 4);
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
        if (writeCursor == writeStart) {
            throw new IllegalStateException("Byte with missing field key");
        }

        if (getAvailableWrite() < 3 || data[writeLastLen] == MAX_DATA_LENGTH && getAvailableWrite() < 5) {
            failInternal(BUFFER_OVERRUN_ERROR);
            return false;
        }
        if (data[writeLastLen] == MAX_DATA_LENGTH) {
            startNewSegment();
        }
        data[writeCursor] = b;
        writeCursor = offset(writeCursor, 1);
        data[writeLastLen]++;
        return true;
    }

    @Override
    public void endToken() {
        if (inNibble) {
            if (numeric) {
                if (writeLastLen != writeStart) {
                    throw new IllegalStateException("Illegal numeric field longer than 255 bytes");
                }

                // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
                byte hold = 0;
                int  pos  = offset(writeStart, 1);
                do {
                    pos = offset(pos, 1);
                    byte tmp = (byte) (data[pos] & 0xF);
                    data[pos] = (byte) (hold | (data[pos] >> 4) & 0xF);
                    hold = (byte) (tmp << 4);
                } while (pos != writeCursor);
            }
            writeCursor = offset(writeCursor, 1);
        }

        writeLastLen = writeStart = writeCursor;
        inNibble = false;
    }

    @Override
    public boolean isInNibble() {
        return inNibble;
    }

    @Override
    public int getCurrentWriteTokenKey() {
        if (isTokenComplete()) {
            throw new IllegalStateException("token isn't being written");
        }
        return data[offset(writeStart, 1)];
    }

    @Override
    public int getCurrentWriteTokenLength() {
        if (isTokenComplete()) {
            throw new IllegalStateException("token isn't being written");
        }
        return data[writeStart];
    }

    @Override
    public int getCurrentWriteTokenNibbleLength() {
        if (isTokenComplete()) {
            throw new IllegalStateException("token isn't being written");
        }
        return data[writeStart] * 2 - (inNibble ? 1 : 0);
    }

    @Override
    public boolean isTokenComplete() {
        return writeStart == writeCursor;
    }

    @Override
    public void setIterator(ZcodeTokenIterator iterator) {
        iterator.set(this, readStart);
    }

    public static class TokenStartIndex {
        private final int indexValue;

        private TokenStartIndex(int readIndex) {
            indexValue = readIndex;
        }
    }

    /** Can we encapsulate things like this? */

    TokenStartIndex nextNewReadToken() {
        return nextNewReadToken(new TokenStartIndex(readStart));
    }

    TokenStartIndex nextNewReadToken(TokenStartIndex currentIndex) {
        int index = currentIndex.indexValue;

        if (index == writeStart) {
            return null;
        }

        byte key;
        do {
            index = offset(index, (data[index] + 2) & 0xff);
            key = data[offset(index, 1)];
        } while (index != writeStart && key == ZcodeTokenRingBuffer.TOKEN_EXTENSION);
        return index == writeStart ? null : new TokenStartIndex(index);
    }

    /**
     * Moves read ptr to the next readable token, and returns a TokenStartIndex cookie, or null if we've run out of valid readable tokens.
     */
    TokenStartIndex skipToNextReadToken() {
        final TokenStartIndex nextNewReadToken = nextNewReadToken();
        readStart = (nextNewReadToken != null) ? nextNewReadToken.indexValue : writeStart;
        return nextNewReadToken;
    }
}

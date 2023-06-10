package org.zcode.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

/**
 * Ring-buffer implementation of a Token Buffer - the tokens making up incoming command or response sequences are encoded and accessed here. Rules are:
 * <ol>
 * <li>Maximum size is 64k bytes - Zcode isn't expected to work on huge datasets.</li>
 * <li>All indices are incremented modulo the length of the underlying byte array.</li>
 * <li>There is a writable area, owned by a TokenWriter, in the space <i>writeStart <= i < readStart</i>.</li>
 * <li>There is a readable area, owned by a TokenIterator, in the space <i>readStart <= i < writeStart</i>.</li>
 * <li>A token is written as >=2 bytes at <i>writeStart</i>: <code>datalen | key | [data]</code> - so tokens can be iterated by adding (datalen+2) to an existing token start.</li>
 * <li>Tokens may exceed datalen of 255 using additional new token with special key <i>TOKEN_EXTENSION</i></li>
 * <li></li>
 * </ol>
 * 
 */
public class ZcodeTokenRingBuffer implements ZcodeTokenBuffer {
    // TODO: Raise flag on error and on newline

    /**
     * Zcode shouldn't need huge buffers, so 64K is our extreme limit. It should be addressable by uint16 indexes. Note that *exact* 64K size implies that data.length cannot be
     * held in a uint16, so careful code required if porting!
     */
    private static final int  MAX_RING_BUFFER_SIZE = 0x1_0000;
    private static final byte MAX_DATA_LENGTH      = (byte) 255;

    /** the ring-buffer's data array */
    private byte[] data;

    private TokenWriter tokenWriter;

    /** index of first byte owned by reader, writable space ends just before it */
    private int readStart  = 0;
    /** index of first byte owned by TokenWriter, readable space ends just before it */
    private int writeStart = 0;

    public static ZcodeTokenRingBuffer createBufferWithCapacity(int sz) {
        return new ZcodeTokenRingBuffer(sz);
    }

    // construct via static factories
    private ZcodeTokenRingBuffer(int sz) {
        if (sz > MAX_RING_BUFFER_SIZE) {
            throw new IllegalArgumentException(format("size too big [sz={}, max={}]", sz, MAX_RING_BUFFER_SIZE));
        }
        data = new byte[sz];
        tokenWriter = new TokenRingBufferWriter();
    }

    // Visible for testing only!
    byte[] getInternalData() {
        return data.clone();
    }

    @Override
    public TokenWriter getTokenWriter() {
        return tokenWriter;
    }

    private class TokenRingBufferWriter implements TokenWriter {
        /** start index of most recent token segment (esp required for long multi-segment tokens) */
        private int writeLastLen = 0;
        /** the current write index into data array */
        private int writeCursor  = 0;

        private boolean inNibble = false;
        private boolean numeric  = false;

        @Override
        public void startToken(byte key, boolean numeric) {
            endToken();
            this.numeric = numeric;
            writeNewTokenStart(key);
        }

        @Override
        public void continueTokenNibble(byte nibble) {
            if (isTokenComplete()) {
                throw new IllegalStateException("Digit with missing field key");
            }
            if (nibble < 0 || nibble > 0xf) {
                throw new IllegalArgumentException("nibble value out of range");
            }

            if (inNibble) {
                data[writeCursor] |= nibble;
                moveCursor();
            } else {
                if (data[writeLastLen] == MAX_DATA_LENGTH) {
                    startNewSegment();
                }
                data[writeCursor] = (byte) (nibble << 4);
                data[writeLastLen]++;
            }
            inNibble = !inNibble;
        }

        @Override
        public void continueTokenByte(byte b) {
            if (inNibble) {
                throw new IllegalStateException("Incomplete nibble");
            }
            if (isTokenComplete()) {
                throw new IllegalStateException("Byte with missing field key");
            }

            if (data[writeLastLen] == MAX_DATA_LENGTH) {
                startNewSegment();
            }
            data[writeCursor] = b;
            moveCursor();
            data[writeLastLen]++;
        }

        private void startNewSegment() {
            writeLastLen = writeCursor;
            writeNewTokenStart(TOKEN_EXTENSION);
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
                moveCursor();
            }

            writeLastLen = writeStart = writeCursor;
            inNibble = false;
        }

        @Override
        public boolean checkAvailableCapacity(int size) {
            return getAvailableWrite() >= size;
        }

        private int getAvailableWrite() {
            return (writeCursor >= readStart ? data.length : 0) + readStart - writeCursor - 1;
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

        private void writeNewTokenStart(byte key) {
            data[writeCursor] = 0;
            moveCursor();
            data[writeCursor] = key;
            moveCursor();
        }

        @Override
        public void fail(final byte errorCode) {
            if (!isTokenComplete()) {
                // reset current token back to writeStart
                writeCursor = writeLastLen = writeStart;
            }
            writeNewTokenStart(errorCode);
            endToken();
        }

        private void moveCursor() {
            final int nextCursor = offset(writeCursor, 1);
            if (nextCursor == readStart) {
                // this should never happen - someone should have made sure there was space
                throw new IllegalStateException("Out of buffer - should have reserved more");
            }
            writeCursor = nextCursor;
        }
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

    @Override
    public void setIterator(ZcodeTokenIterator iterator) {
        iterator.set(this, readStart);
    }

    public static class TokenStartIndex {
        private final int indexValue;

        private TokenStartIndex(int indexValue) {
            this.indexValue = indexValue;
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

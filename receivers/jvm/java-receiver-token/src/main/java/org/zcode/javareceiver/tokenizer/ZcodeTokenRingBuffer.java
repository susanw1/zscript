package org.zcode.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

import java.util.Iterator;
import java.util.function.Predicate;

/**
 * Ring-buffer implementation of a Token Buffer - the tokens making up incoming command or response sequences are encoded and accessed here. Rules are:
 * <ol>
 * <li>Maximum size is 64k bytes - Zcode isn't expected to work on huge datasets.</li>
 * <li>All indices are incremented modulo the length of the underlying byte array.</li>
 * <li>There is a writable area, owned by a TokenWriter, in the space <i>writeStart <= i < readStart</i>.</li>
 * <li>There is a readable area, owned by a TokenIterator, in the space <i>readStart <= i < writeStart</i>.</li>
 * <li>A token is written as >=2 bytes at <i>writeStart</i>: <code>key | datalen | [data]</code> - so tokens can be iterated by adding (datalen+2) to an existing token start.</li>
 * <li>A marker is written as 1 byte at <i>writeStart</i>, indicating a dataless key - markers are identified as a key with top 3 bits set (eg from 0xe0-0xff).</li>
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
    private static final int  MAX_RING_BUFFER_SIZE  = 0x1_0000;
    private static final byte MAX_TOKEN_DATA_LENGTH = (byte) 255;

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
        /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
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
                throw new IllegalArgumentException("Nibble value out of range");
            }

            if (inNibble) {
                data[writeCursor] |= nibble;
                moveCursor();
            } else {
                if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
                    if (numeric) {
                        throw new IllegalArgumentException("Illegal numeric field longer than 255 bytes");
                    }
                    writeNewTokenStart(TOKEN_EXTENSION);
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

            if (data[writeLastLen] == MAX_TOKEN_DATA_LENGTH) {
                if (numeric) {
                    throw new IllegalArgumentException("Illegal numeric field longer than 255 bytes");
                }
                writeNewTokenStart(TOKEN_EXTENSION);
            }
            data[writeCursor] = b;
            moveCursor();
            data[writeLastLen]++;
        }

        @Override
        public void endToken() {
            if (inNibble) {
                if (numeric) {
//                    if (writeLastLen != writeStart) {
//                        throw new IllegalStateException("Illegal numeric field longer than 255 bytes");
//                    }

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

            writeStart = writeCursor;
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
        public byte getCurrentWriteTokenKey() {
            if (isTokenComplete()) {
                throw new IllegalStateException("no token being written");
            }
            return data[writeStart];
        }

        @Override
        public int getCurrentWriteTokenLength() {
            if (isTokenComplete()) {
                throw new IllegalStateException("no token being written");
            }
            return Byte.toUnsignedInt(data[offset(writeStart, 1)]);
        }

        @Override
        public int getCurrentWriteTokenNibbleLength() {
            if (isTokenComplete()) {
                throw new IllegalStateException("token isn't being written");
            }
            return Byte.toUnsignedInt(data[writeStart + 1]) * 2 - (inNibble ? 1 : 0);
        }

        @Override
        public boolean isTokenComplete() {
            return writeStart == writeCursor;
        }

        @Override
        public void writeMarker(final byte key) {
            if (!ZcodeTokenBuffer.isMarker(key)) {
                throw new IllegalArgumentException("invalid marker [key=0x" + Integer.toHexString(key) + "]");
            }
            endToken();
            data[writeCursor] = key;
            moveCursor();
            endToken();
        }

        private void writeNewTokenStart(final byte key) {
            data[writeCursor] = key;
            moveCursor();
            data[writeCursor] = 0;
            writeLastLen = writeCursor;
            moveCursor();
        }

        @Override
        public void fail(final byte errorCode) {
            if (!isTokenComplete()) {
                // reset current token back to writeStart
                writeCursor = writeStart;
            }
            writeMarker(errorCode);
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

    /**
     * Utility for determining whether the specified index is in the current readable area, defined as readStart <= index < writeStart (but accounting for this being a ringbuffer).
     * 
     * Everything included in the readable area is committed and immutable.
     * 
     * @param index the index to check
     * @return true if in readable area; false otherwise
     */
    boolean isInReadableArea(int index) {
        return writeStart >= readStart && (index >= readStart && index < writeStart)
                || writeStart < readStart && (index < writeStart || index >= readStart);
    }

    @Override
    public void setIterator(ZcodeTokenIterator iterator) {
        iterator.set(this, readStart);
    }

    @Override
    public RingBufferToken createReadToken() {
        return readStart == writeStart ? null : new RingBufferToken(readStart);
    }

    /**
     * A view of a token starting at a known index. In principle, all the operations around navigating tokens as stored in the buffer are encapsulated here.
     * 
     * Fundamentally, the ReadToken is nothing more than an index into the RingBuffer: it's really just a uint16 or uint8. If you re-implement this in C, then that's what you'd
     * have. But at a higher level, it has the following rules:
     * <ul>
     * <li>you can only create a Token from the valid start of the readable space, or from another token.</li>
     * <li>you can't go off the end of the readable space.</li>
     * </ul>
     * 
     * Any operation which gives a ReadToken where these rules might break should return null.
     */
    private class RingBufferToken implements ReadToken {
        private final int index;

        private RingBufferToken(int index) {
            if (index < 0 || index >= data.length) {
                throw new IllegalArgumentException("index out of data bounds [index=" + index + "]");
            }
            if (!isInReadableArea(index)) {
                throw new IllegalArgumentException("index is not in readable area [index=" + index + "]");
            }
            this.index = index;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void advanceReadStart() {
            readStart = index;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public byte getKey() {
            return data[index];
        }

        /**
         * Determines the size of the data portion of just this token (not including any extensions).
         * 
         * @return this Token's data size - 0 to 255
         */
        private int getSegmentDataSize() {
            return ZcodeTokenBuffer.isMarker(data[index]) ? 0 : Byte.toUnsignedInt(data[offset(index, 1)]);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public int getDataSize() {
            int totalSz = 0;

            int             thisSz;
            RingBufferToken t = this;

            do {
                thisSz = t.getSegmentDataSize();
                totalSz += thisSz;
            } while (thisSz == MAX_TOKEN_DATA_LENGTH && (t = t.next()) != null && t.getKey() == ZcodeTokenRingBuffer.TOKEN_EXTENSION);

            return totalSz;
        }

        /**
         * TODO: finish me! I'm a bit complicated!
         * 
         * Exposes the data as a set of byte[] units, as though we were posting the data to a DMA system in chunks. Notes: this is
         * 
         * 
         * Implementation note: if this is re-implemented in C, it wouldn't be *copying* the bytes. It would be referencing them as a [const char*, size] pair pointing directly
         * into the buffer to avoid copies.
         * 
         * @return
         */
        @Override
        public Iterator<byte[]> dataAsByteArrays() {
            return new Iterator<byte[]>() {
                RingBufferToken currentToken       = RingBufferToken.this;
                int             remainingInSegment = 0;

                @Override
                public boolean hasNext() {
                    return currentToken != null && currentToken.getSegmentDataSize() > 0;
                }

                @Override
                public byte[] next() {
                    if (remainingInSegment == 0) {
                        // processing new segment
                        remainingInSegment = currentToken.getSegmentDataSize();
                    }

                    // We want actualUnitLength clamped to the length of the data buffer, so we return data that's cut across the ring buffer as two parts
                    // TODO: FIGURE THIS OUT!! :
                    int    actualUnitLength = remainingInSegment;
                    byte[] b                = new byte[actualUnitLength];
                    System.arraycopy(data, index + 2, b, 0, actualUnitLength);
                    remainingInSegment -= actualUnitLength;
                    currentToken = currentToken.next();
                    if (currentToken != null && currentToken.getKey() != TOKEN_EXTENSION) {
                        currentToken = null;
                    }
                    return b;
                }
            };
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public Iterator<Byte> dataAsByteStream() {
            return new Iterator<Byte>() {
                RingBufferToken currentToken       = RingBufferToken.this;
                int             remainingInSegment = currentToken.getSegmentDataSize();

                @Override
                public boolean hasNext() {
                    return currentToken != null && remainingInSegment > 0;
                }

                @Override
                public Byte next() {
                    if (!hasNext()) {
                        // defensive against mistakes
                        throw new IllegalStateException("hasNext() method should have prevented this state!");
                    }
                    int  dataCursor = offset(index, currentToken.getSegmentDataSize() - remainingInSegment + 2);
                    byte b          = data[dataCursor];
                    remainingInSegment--;

                    if (remainingInSegment == 0) {
                        currentToken = currentToken.next();
                        if (currentToken != null) {
                            if (currentToken.getKey() != TOKEN_EXTENSION) {
                                currentToken = null;
                            } else {
                                remainingInSegment = currentToken.getSegmentDataSize();
                            }
                        }
                    }
                    return b;
                }
            };
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public RingBufferToken findFirst(Predicate<? super ReadToken> p) {
            RingBufferToken t = this;
            while (t != null && !p.test(t)) {
                t = t.next();
            }
            return t;
        }

        /**
         * Utility to create a Token for the token immediately after this one, if there is one (note, might be an extension of this one).
         * 
         * @return a valid Token, or null if none exists yet (eg if that would run off the end of the read space)
         */
        private RingBufferToken next() {
            int nextStart = offset(index, getTokenExtent());
            return nextStart == writeStart ? null : new RingBufferToken(nextStart);
        }

        /**
         * Utility method for determining how many bytes this token occupies in the buffer, taking into account key and any data, but not extensions. This amount is how much to add
         * to this token's start index find the next token's start index.
         * 
         * @return the number of bytes associates with this token (1 - 257)
         */
        private int getTokenExtent() {
            return ZcodeTokenBuffer.isMarker(data[index]) ? 1 : Byte.toUnsignedInt(data[offset(index, 1)]) + 2;
        }
    }
}

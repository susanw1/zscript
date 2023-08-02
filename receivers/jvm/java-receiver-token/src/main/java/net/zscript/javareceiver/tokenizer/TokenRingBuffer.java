package net.zscript.javareceiver.tokenizer;

import static java.text.MessageFormat.format;

/**
 * Ring-buffer implementation of a Token Buffer - a specific implementation of the ArrayBuffer. Additional rules are:
 * <ol>
 * <li>Maximum size is 64k bytes - Zscript isn't expected to work on huge datasets.</li>
 * <li>All indices are incremented modulo the length of the underlying byte array.</li>
 * <li></li>
 * </ol>
 *
 */
public class TokenRingBuffer extends TokenArrayBuffer {
    /**
     * Zscript shouldn't need huge buffers, so 64K is our extreme limit. It should be addressable by uint16 indexes. Note that *exact* 64K size implies that data.length cannot be
     * held in a uint16, so careful code required if porting!
     */
    private static final int MAX_RING_BUFFER_SIZE = 0x1_0000;

    public static TokenRingBuffer createBufferWithCapacity(final int sz) {
        return new TokenRingBuffer(sz);
    }

    private TokenRingBuffer(int sz) {
        super(sz);
        if (sz > MAX_RING_BUFFER_SIZE) {
            throw new IllegalArgumentException(format("size too big [sz={}, max={}]", sz, MAX_RING_BUFFER_SIZE));
        }
    }

    private int getAvailableWrite(int writeCursor) {
        return (writeCursor >= readStart ? data.length : 0) + readStart - writeCursor - 1;
    }

    @Override
    protected boolean checkAvailableCapacityFrom(int writeCursor, int size) {
        return getAvailableWrite(writeCursor) >= size;
    }

    @Override
    int offset(final int index, final int offset) {
        if (index < 0 || offset < 0 || offset >= data.length) {
            throw new IllegalArgumentException(format("Unexpected values [pos={}, offset={}]", index, offset));
        }
        return (index + offset) % data.length;
    }
}

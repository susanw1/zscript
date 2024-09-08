package net.zscript.util;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.CharacterCodingException;
import java.util.Arrays;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * Little utility class that gives expandable byte array capability without the hassle of ByteArrayOutputStream. Used by ByteString to buffer bytes.
 * <p/>
 * Visible for testing.
 */
final class ExpandableByteBuffer {
    /** the buffer where we store the incoming bytes. */
    private byte[] buf;
    /** The number of bytes stored so far, such that buf[count] is the first free slot */
    private int    count;

    public ExpandableByteBuffer(int initialSize) {
        buf = new byte[initialSize];
        count = 0;
    }

    /**
     * Determines the current byte count, which equates to the current size of the buffered data (actual capacity of buffer is likely to be larger).
     *
     * @return the current count
     */
    public int getCount() {
        return count;
    }

    /**
     * Ensure that there is at least enough capacity to store 'more' additional bytes. Reallocates the buffer by doubling the buffer size until there's enough room.
     *
     * @param more how many more bytes will need to be stored
     */
    private void growIfRequired(int more) {
        int sz = buf.length;
        if (count + more >= sz) {
            while (count + more >= sz) {
                sz <<= 1;
            }
            buf = Arrays.copyOf(buf, sz);
        }
    }

    /**
     * Test method for determining capacity.
     *
     * @return current capacity
     */
    int getCapacity() {
        return buf.length;
    }

    /**
     * Adds a single byte.
     *
     * @param b the byte to add
     */
    public void addByte(byte b) {
        growIfRequired(1);
        buf[count++] = b;
    }

    /**
     * Adds the supplied bytes, starting from 'offset' and adding 'len' bytes
     *
     * @param b      the bytes to add
     * @param offset starting from this offset
     * @param len    copying this many bytes
     */
    public void addBytes(byte[] b, int offset, int len) {
        growIfRequired(len);
        System.arraycopy(b, offset, buf, count, len);
        count += len;
    }

    /**
     * Writes the accumulated bytes to the specified OutputStream.
     *
     * @param out the stream to write
     * @throws IOException if write fails
     */
    public void writeTo(final OutputStream out) throws IOException {
        out.write(buf, 0, count);
    }

    /**
     * Creates a fresh copy of the buffer containing exactly the bytes added so far (ie the length = current count)
     *
     * @return copy of the bytes so far
     */
    public byte[] toByteArray() {
        return Arrays.copyOf(buf, count);
    }

    /**
     * A String containing the bytes so far interpreted as UTF-8. Has potential to throw {@link CharacterCodingException} or similar if encoding is invalid.
     *
     * @return string representing the bytes so far
     */
    public String asString() {
        return new String(buf, 0, count, UTF_8);
    }

    @Override
    public String toString() {
        return asString();
    }
}

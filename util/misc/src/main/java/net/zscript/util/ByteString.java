package net.zscript.util;

import javax.annotation.Nonnull;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.NoSuchElementException;

import static java.nio.charset.StandardCharsets.ISO_8859_1;
import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * A buildable byte string to make it easy to work with byte-oriented text data.
 */
public final class ByteString implements Iterable<Byte> {
    private final byte[] bytes;

    private ByteString(final byte[] bytes) {
        this.bytes = bytes;
    }

    /**
     * Returns (a copy of) the content of this ByteString.
     *
     * @return content
     */
    public byte[] toByteArray() {
        return bytes.clone();
    }

    /**
     * Determines the number of bytes in this ByteString.
     *
     * @return the number of bytes
     */
    public int size() {
        return bytes.length;
    }

    /**
     * Returns a block Iterator over the bytes in the string, with the default maximum block size set to the size of this ByteString.
     *
     * @return iterator a block iterator
     */
    @Nonnull
    @Override
    public BlockIterator iterator() {
        return new ByteBlockIterator(bytes.length);
    }

    /**
     * Returns a block Iterator over the bytes in the string, with the specified default maximum block size.
     *
     * @param defaultMaxLength the max number of bytes to return in a call to {@link BlockIterator#nextContiguous()}
     * @return iterator a block iterator
     */
    @Nonnull
    public BlockIterator iterator(int defaultMaxLength) {
        return new ByteBlockIterator(defaultMaxLength);
    }

    /**
     * Access the byte at the specified index.
     *
     * @param index the index to retrieve
     * @return the byte at that index
     * @throws ArrayIndexOutOfBoundsException if index < 0 or index >= size()
     */
    public byte get(int index) {
        return bytes[index];
    }

    /**
     * Writes this ByteString to the specified OutputStream.
     *
     * @param out the stream to write
     * @return this ByteString, to facilitate chaining
     * @throws IOException if the write fails
     */
    @Nonnull
    public ByteString writeTo(final OutputStream out) throws IOException {
        out.write(bytes);
        return this;
    }

    /**
     * Similar to System.arraycopy, this allows the bytes to be written to a destination byte-array. If 'max' implies a larger number of bytes than available, or there's
     * insufficient space in 'dest', then the actual number of bytes copied is correspondingly reduced, and returned. Areas of 'dest' that are before destPos or after the end of
     * the writing are unaffected.
     *
     * @param srcPos  the start index in this ByteString
     * @param dest    the destination byte-array
     * @param destPos the start position in the destination array
     * @param max     the max number of bytes to copy
     * @return the actual number of bytes written
     */
    public int copyTo(int srcPos, byte[] dest, int destPos, int max) {
        int len = Math.min(Math.min(bytes.length - srcPos, dest.length - destPos), max);
        System.arraycopy(bytes, srcPos, dest, destPos, len);
        return len;
    }

    /**
     * Exposes this ByteString as an Appendable to make it easy to append to other ByteStrings.
     *
     * @return an Appendable adapter that can write these bytes to another Builder
     */
    @Nonnull
    public ByteAppendable asAppendable() {
        return builder -> builder.appendRaw(bytes);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    @Nonnull
    public String toString() {
        return "ByteString[" + asString() + "]";
    }

    /**
     * Returns the content of this ByteString as a Java String, using UTF-8 conversion.
     *
     * @return content as a String
     */
    @Nonnull
    public String asString() {
        return new String(bytes, UTF_8);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(bytes);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        }
        if (!(obj instanceof ByteString)) {
            return false;
        }
        return Arrays.equals(bytes, ((ByteString) obj).bytes);
    }

    /**
     * Utility method to create a ByteString from a single Appendable
     *
     * @param a the appendable to append
     * @return the resulting ByteString
     */
    @Nonnull
    public static ByteString from(ByteAppendable a) {
        return from(a, ByteAppender.DEFAULT_APPENDER);
    }

    /**
     * Utility method to create a ByteString from byte[]
     *
     * @param b the byte array to copy
     * @return the resulting ByteString
     */
    @Nonnull
    public static ByteString from(byte[] b) {
        return from(b, (object, builder) -> builder.appendRaw(b));
    }

    /**
     * Utility method to create a ByteString from a single byte
     *
     * @param b the byte to copy
     * @return the resulting ByteString
     */
    @Nonnull
    public static ByteString from(byte b) {
        return from(b, (object, builder) -> builder.appendByte(b));
    }

    /**
     * Utility method to create a ByteString from any object, using a suitable ByteAppender
     *
     * @param object   the object to append
     * @param appender the appender to do the appending
     * @param <T>      the type of the object
     * @return the resulting ByteString
     */
    @Nonnull
    public static <T> ByteString from(T object, ByteAppender<? super T> appender) {
        return builder().append(object, appender).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) Appendables
     *
     * @param appendables the appendables to append
     * @return the resulting ByteString
     */
    @Nonnull
    public static ByteString concat(ByteAppendable... appendables) {
        return builder(appendables).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) objects using the supplied appender.
     * <p/>
     * SafeVarargs confirmed because objects array is not modified.
     *
     * @param appender the appender to use
     * @param objects  the appendables to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    @SafeVarargs
    @Nonnull
    public static <T> ByteString concat(ByteAppender<? super T> appender, T... objects) {
        return builder(appender, objects).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) Appendables in their Iterator order
     *
     * @param a the appendables to append
     * @return the new ByteString
     */
    @Nonnull
    public static ByteString concat(Iterable<? extends ByteAppendable> a) {
        return builder(a).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) Appendables
     *
     * @param appender the appender to use
     * @param objects  the appendables to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    @Nonnull
    public static <T> ByteString concat(ByteAppender<? super T> appender, Iterable<? extends T> objects) {
        return builder(appender, objects).build();
    }

    /**
     * Creates a fresh empty builder for a ByteString
     *
     * @return a new builder
     */
    @Nonnull
    public static ByteStringBuilder builder() {
        return new ByteStringBuilder();
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the concatenation of the supplied Appendables.
     *
     * @param appendables the appendables to be appended
     * @return a newly initialized builder
     */
    @Nonnull
    public static ByteStringBuilder builder(ByteAppendable... appendables) {
        return builder(ByteAppender.DEFAULT_APPENDER, appendables);
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the concatenation of the supplied Appendables, using the supplied ByteAppender.
     * <p/>
     * SafeVarargs confirmed because objects array is not modified.
     *
     * @param appender the appender to use
     * @param objects  the objects to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    @SafeVarargs
    @Nonnull
    public static <T> ByteStringBuilder builder(ByteAppender<? super T> appender, T... objects) {
        return builder().append(appender, objects);
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the supplied Appendables in their Iterator order.
     *
     * @param appendables appendables to be appended
     * @return a newly initialized builder
     */
    @Nonnull
    public static ByteStringBuilder builder(Iterable<? extends ByteAppendable> appendables) {
        return builder(ByteAppender.DEFAULT_APPENDER, appendables);
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the supplied objects in their Iterator order, using the supplied ByteAppender.
     *
     * @param appender the appender to use
     * @param objects  the objects to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    @Nonnull
    public static <T> ByteStringBuilder builder(ByteAppender<? super T> appender, Iterable<? extends T> objects) {
        return builder().append(appender, objects);
    }

    public static final class ByteStringBuilder {
        private static final byte[] HEX = "0123456789abcdef".getBytes(UTF_8);

        private final ExpandableBuffer buffer = new ExpandableBuffer(128);

        /**
         * Writes the current contents of the builder to the specified OutputStream.
         *
         * @param out the stream to write to
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder writeTo(final OutputStream out) throws IOException {
            buffer.writeTo(out);
            return this;
        }

        /**
         * Returns a copy of the current contents of the builder
         *
         * @return copy of the current contents
         */
        @Nonnull
        public byte[] toByteArray() {
            return buffer.toByteArray();
        }

        /**
         * Determines the number of bytes appended so far.
         *
         * @return the byte count
         */
        public int size() {
            return buffer.getCount();
        }

        /**
         * Appends the supplied single byte.
         *
         * @param b any value 0x00-0xff
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range 0-ff
         */
        @Nonnull
        public ByteStringBuilder appendByte(int b) {
            if ((b & ~0xff) != 0) {
                throw new IllegalArgumentException("Hexpair values must be 0x0-0xff: " + b);
            }
            return appendByte((byte) b);
        }

        /**
         * Appends the supplied single byte, without having to check for range.
         *
         * @param b any value 0x00-0xff
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder appendByte(byte b) {
            buffer.addByte(b);
            return this;
        }

        /**
         * Appends the supplied byte array.
         *
         * @param bytes any bytes
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder appendRaw(byte[] bytes) {
            buffer.addBytes(bytes, 0, bytes.length);
            return this;
        }

        /**
         * Adds the supplied bytes, starting from 'offset' and adding 'len' bytes
         *
         * @param bytes  the bytes to add
         * @param offset starting from this offset
         * @param len    copying this many bytes
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder appendRaw(byte[] bytes, int offset, int len) {
            buffer.addBytes(bytes, offset, len);
            return this;
        }

        /**
         * Appends the supplied text in UTF-8 encoding (with no escaping or other transforms).
         *
         * @param text the text to write
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder appendUtf8(CharSequence text) {
            return appendRaw(text.toString().getBytes(UTF_8));
        }

        /**
         * Appends the supplied byte string.
         *
         * @param byteString any existing ByteString
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder append(ByteString byteString) {
            return append(byteString.asAppendable());
        }

        /**
         * Appends the supplied Appendable object
         *
         * @param byteAppendable the object to be appended
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder append(ByteAppendable byteAppendable) {
            return append(byteAppendable, ByteAppender.DEFAULT_APPENDER);
        }

        /**
         * Appends the supplied object using the supplied ByteAppender.
         *
         * @param object   the object to append
         * @param appender the appender to use
         * @param <T>      the type of the object to be appended
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public <T> ByteStringBuilder append(T object, ByteAppender<? super T> appender) {
            appender.append(object, this);
            return this;
        }

        /**
         * Appends each of the Appendables in the supplied collection, in iterator order.
         *
         * @param appendables zero or more Appendables
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder append(Iterable<? extends ByteAppendable> appendables) {
            return append(ByteAppender.DEFAULT_APPENDER, appendables);
        }

        /**
         * Appends each of the supplied objects using the supplied appender, in iterator order.
         *
         * @param appender the appender to use
         * @param objects  zero or more objects to be appended
         * @param <T>      the type of the objects
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public <T> ByteStringBuilder append(ByteAppender<? super T> appender, Iterable<? extends T> objects) {
            objects.forEach(a -> appender.append(a, this));
            return this;
        }

        /**
         * Appends each of the supplied Appendables.
         *
         * @param appendables zero or more Appendables
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder append(ByteAppendable... appendables) {
            return append(ByteAppender.DEFAULT_APPENDER, Arrays.asList(appendables));
        }

        /**
         * Appends each of the supplied objects using the supplied appender.
         *
         * @param objects zero or more Appendables
         * @return this builder, to facilitate chaining
         */
        @SafeVarargs
        @Nonnull
        public final <T> ByteStringBuilder append(ByteAppender<? super T> appender, T... objects) {
            return append(appender, Arrays.asList(objects));
        }

        /**
         * Appends the supplied value as a 2 nibble hexadecimal.
         *
         * @param value any value 0x00-0xff
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range 0-ff
         */
        @Nonnull
        public ByteStringBuilder appendHexPair(int value) {
            if ((value & ~0xff) != 0) {
                throw new IllegalArgumentException("Hex-pair values must be 0x0-0xff: " + value);
            }
            return appendHexPair((byte) value);
        }

        /**
         * Appends the supplied value as a 2 nibble hexadecimal.
         *
         * @param value any value 0x00-0xff
         * @return this builder, to facilitate chaining
         */
        @Nonnull
        public ByteStringBuilder appendHexPair(byte value) {
            buffer.addByte(toHex((value >>> 4) & 0xF));
            buffer.addByte(toHex(value & 0xF));
            return this;
        }

        /**
         * Appends a number as up to 8 nibbles of hex. ALL leading zeroes are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
        @Nonnull
        public ByteStringBuilder appendNumeric32(long value) {
            return value == 0 ? this : appendNumeric32KeepZero(value);
        }

        /**
         * Appends a number as up to 8 nibbles of hex. All leading zeroes EXCEPT ONE are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
        @Nonnull
        public ByteStringBuilder appendNumeric32KeepZero(long value) {
            if ((value & ~0xffffffffL) != 0) {
                throw new IllegalArgumentException("Numeric32 fields must be 0x0-0xffffffff: " + value);
            }
            if (value >= 0x10000) {
                appendNumeric16KeepZero((int) (value >>> 16));
                appendHexPair((byte) (value >>> 8));
                return appendHexPair((byte) (value));
            } else {
                return appendNumeric16KeepZero((int) (value & 0xffff));
            }
        }

        /**
         * Appends a number as up to 4 nibbles of hex. ALL leading zeroes are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
        @Nonnull
        public ByteStringBuilder appendNumeric16(int value) {
            return value == 0 ? this : appendNumeric16KeepZero(value);
        }

        /**
         * Appends a number as up to 4 nibbles of hex. All leading zeroes EXCEPT ONE are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
        @Nonnull
        public ByteStringBuilder appendNumeric16KeepZero(int value) {
            if ((value & ~0xffff) != 0) {
                throw new IllegalArgumentException("Numeric fields must be 0x0-0xffff: " + value);
            }
            if (value >= 0x1000) {
                buffer.addByte(toHex(value >>> 12));
            }
            if (value >= 0x100) {
                buffer.addByte(toHex((value >>> 8) & 0xF));
            }
            if (value >= 0x10) {
                buffer.addByte(toHex((value >>> 4) & 0xF));
            }
            buffer.addByte(toHex(value & 0xF));
            return this;
        }

        /**
         * Exposes this Builder as an Appendable to make it easy to append to other ByteStrings.
         * <p/>
         * Note: The bytes appended will be those accumulated at the time the ByteAppendable is used, including any added since this method was called. It isn't a frozen snapshot.
         *
         * @return an Appendable adapter that can write this Builder to another Builder
         */
        @Nonnull
        public ByteAppendable asAppendable() {
            return builder -> builder.appendRaw(toByteArray());
        }

        @Nonnull
        public String asString() {
            return buffer.asString();
        }

        @Nonnull
        @Override
        public String toString() {
            return "ByteStringBuilder[" + asString() + "]";
        }

        @Nonnull
        public ByteString build() {
            return new ByteString(toByteArray());
        }
    }

    /**
     * Handy interface to allow classes to define their own means of being appended to a ByteString.
     */
    public interface ByteAppendable {
        /**
         * Defines how to be written to the supplied Builder
         *
         * @param builder the builder to append to
         */
        void appendTo(ByteStringBuilder builder);

        /**
         * Renders this object as a ByteString.
         *
         * @return a ByteString containing just this object.
         */
        @Nonnull
        default ByteString toByteString() {
            return ByteString.from(this);
        }
    }

    /**
     * Defines how to append an object of type T, in case it isn't ByteAppendable, or if a different append operation is required than the default. (This is a bit like
     * java.util.Comparator vs java.util.Comparable)
     *
     * @param <T> the type of object to append
     */
    public interface ByteAppender<T> {
        ByteAppender<ByteAppendable> DEFAULT_APPENDER   = ByteAppendable::appendTo;
        ByteAppender<String>         UTF8_APPENDER      = (s, builder) -> builder.appendUtf8(s);
        ByteAppender<String>         ISOLATIN1_APPENDER = (s, builder) -> builder.appendRaw(s.getBytes(ISO_8859_1));

        /**
         * Defines how to perform the append for the supplied object
         *
         * @param object  the object to append
         * @param builder the builder to append to
         */
        void append(T object, ByteStringBuilder builder);

        /**
         * Renders the specified object as a ByteString using this ByteAppender
         *
         * @param object the object to append
         * @return a ByteString containing just this object.
         */
        @Nonnull
        default ByteString toByteString(T object) {
            return ByteString.from(object, this);
        }
    }

    /**
     * Simple array iterator
     */
    private class ByteBlockIterator implements BlockIterator {
        final int defaultMaxLength;
        int index = 0;

        /**
         * Creates a new iterator.
         *
         * @param defaultMaxLength the max number of bytes to return in a call to {@link #nextContiguous()}
         */
        ByteBlockIterator(int defaultMaxLength) {
            this.defaultMaxLength = defaultMaxLength;
        }

        @Override
        public boolean hasNext() {
            return index < bytes.length;
        }

        @Nonnull
        @Override
        public Byte next() {
            if (!hasNext()) {
                throw new NoSuchElementException();
            }
            return bytes[index++];
        }

        @Nonnull
        @Override
        public byte[] nextContiguous() {
            return nextContiguous(defaultMaxLength);
        }

        @Nonnull
        @Override
        public byte[] nextContiguous(int maxLength) {
            int    n     = Math.min(maxLength, bytes.length - index);
            byte[] block = new byte[n];
            int    len   = copyTo(index, block, 0, n);
            index += len;
            return block;
        }
    }

    /**
     * Simply converts a nibble value (0-15) to an ASCII hex char, lowercase.
     *
     * @param nibble a value 0-15
     * @return the corresponding hex character, '0'-'9' or 'a'-'f'
     * @throws IndexOutOfBoundsException if value is out of range
     */
    private static byte toHex(int nibble) {
        return ByteStringBuilder.HEX[nibble];
    }
}

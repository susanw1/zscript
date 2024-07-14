package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import static java.nio.charset.StandardCharsets.UTF_8;

/**
 * A buildable byte string to make it easy to work with byte-oriented text data.
 */
public final class ByteString {
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
     * Writes this ByteString to the specified OutputStream.
     *
     * @param out the stream to write
     * @return this ByteString, to facilitate chaining
     * @throws IOException if the write fails
     */
    public ByteString writeTo(final OutputStream out) throws IOException {
        out.write(bytes);
        return this;
    }

    /**
     * Exposes this ByteString as an Appendable to make it easy to append to other ByteStrings.
     *
     * @return an Appendable adapter that can write these bytes to another Builder
     */
    public ByteAppendable asAppendable() {
        return builder -> builder.appendRaw(bytes);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return "ByteString[" + asString() + "]";
    }

    /**
     * Returns the content of this ByteString as a Java String, using UTF-8 conversion.
     *
     * @return content as a String
     */
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
    public static ByteString from(ByteAppendable a) {
        return from(a, ByteAppender.DEFAULT_APPENDER);
    }

    /**
     * Utility method to create a ByteString from any object, using a suitable ByteAppender
     *
     * @param object   the object to append
     * @param appender the appender to do the appending
     * @param <T>      the type of the object
     * @return the resulting ByteString
     */
    private static <T> ByteString from(T object, ByteAppender<? super T> appender) {
        return builder().append(object, appender).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) Appendables
     *
     * @param appendables the appendables to append
     * @return the resulting ByteString
     */
    public static ByteString concat(ByteAppendable... appendables) {
        return builder(appendables).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) objects using the supplied appender.
     *
     * @param appender the appender to use
     * @param objects  the appendables to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    public static <T> ByteString concat(ByteAppender<? super T> appender, T... objects) {
        return builder(appender, objects).build();
    }

    /**
     * Utility method to create a ByteString from a concatenated series of (zero or more) Appendables in their Iterator order
     *
     * @param a the appendables to append
     * @return the new ByteString
     */
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
    public static <T> ByteString concat(ByteAppender<? super T> appender, Iterable<? extends T> objects) {
        return builder(appender, objects).build();
    }

    /**
     * Creates a fresh empty builder for a ByteString
     *
     * @return a new builder
     */
    public static ByteStringBuilder builder() {
        return new ByteStringBuilder();
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the concatenation of the supplied Appendables.
     *
     * @param appendables the appendables to be appended
     * @return a newly initialized builder
     */
    public static ByteStringBuilder builder(ByteAppendable... appendables) {
        return builder(ByteAppender.DEFAULT_APPENDER, appendables);
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the concatenation of the supplied Appendables, using the supplied ByteAppender.
     *
     * @param appender the appender to use
     * @param objects  the objects to append
     * @param <T>      the type of the objects
     * @return the resulting ByteString
     */
    @SafeVarargs
    public static <T> ByteStringBuilder builder(ByteAppender<? super T> appender, T... objects) {
        return builder().append(appender, objects);
    }

    /**
     * Creates a fresh builder for a ByteString, initialized with the supplied Appendables in their Iterator order.
     *
     * @param appendables appendables to be appended
     * @return a newly initialized builder
     */
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
    public static <T> ByteStringBuilder builder(ByteAppender<? super T> appender, Iterable<? extends T> objects) {
        return builder().append(appender, objects);
    }

    public static final class ByteStringBuilder {
        private static final byte[] HEX = "0123456789abcdef".getBytes(UTF_8);

        private final ByteArrayOutputStream baos = new ByteArrayOutputStream();

        /**
         * Writes the current contents of the builder to the specified OutputStream.
         *
         * @param out the stream to write to
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder writeTo(final OutputStream out) throws IOException {
            out.write(baos.toByteArray());
            return this;
        }

        /**
         * Returns a copy of the current contents of the builder
         *
         * @return copy of the current contents
         */
        public byte[] toByteArray() {
            return baos.toByteArray();
        }

        /**
         * Appends the supplied single byte.
         *
         * @param b any value 0x00-0xff
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range 0-ff
         */
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
        public ByteStringBuilder appendByte(byte b) {
            baos.write(b);
            return this;
        }

        /**
         * Appends the supplied byte array.
         *
         * @param bytes any bytes
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder appendRaw(byte[] bytes) {
            baos.writeBytes(bytes);
            return this;
        }

        /**
         * Appends the supplied text in UTF-8 encoding (with no escaping or other transforms).
         *
         * @param text the text to write
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder appendUtf8(CharSequence text) {
            return appendRaw(text.toString().getBytes(UTF_8));
        }

        /**
         * Appends the supplied byte string.
         *
         * @param byteString any existing ByteString
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder append(ByteString byteString) {
            return append(byteString.asAppendable());
        }

        /**
         * Appends the supplied Appendable object
         *
         * @param byteAppendable the object to be appended
         * @param <B>            the type of Builder it needs to be written to
         * @return this builder, to facilitate chaining
         */
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
        public ByteStringBuilder append(ByteAppendable... appendables) {
            return append(ByteAppender.DEFAULT_APPENDER, Arrays.asList(appendables));
        }

        /**
         * Appends each of the supplied objects using the supplied appender.
         *
         * @param objects zero or more Appendables
         * @return this builder, to facilitate chaining
         */
        public <T> ByteStringBuilder append(ByteAppender<? super T> appender, T... objects) {
            return append(appender, Arrays.asList(objects));
        }

        /**
         * Appends the supplied value as a 2 nibble hexadecimal.
         *
         * @param value any value 0x00-0xff
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range 0-ff
         */
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
        public ByteStringBuilder appendHexPair(byte value) {
            baos.write(toHex((value >>> 4) & 0xF));
            baos.write(toHex(value & 0xF));
            return this;
        }

        /**
         * Appends a number as up to 8 nibbles of hex. ALL leading zeroes are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
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
        public ByteStringBuilder appendNumeric16KeepZero(int value) {
            if ((value & ~0xffff) != 0) {
                throw new IllegalArgumentException("Numeric fields must be 0x0-0xffff: " + value);
            }
            if (value >= 0x1000) {
                baos.write(toHex(value >>> 12));
            }
            if (value >= 0x100) {
                baos.write(toHex((value >>> 8) & 0xF));
            }
            if (value >= 0x10) {
                baos.write(toHex((value >>> 4) & 0xF));
            }
            baos.write(toHex(value & 0xF));
            return this;
        }

        /**
         * Exposes this Builder as an Appendable to make it easy to append to other ByteStrings.
         *
         * Note: The bytes appended will be those accumulated at the time the ByteAppendable is used, including any added since this method was called. It isn't a frozen snapshot.
         *
         * @return an Appendable adapter that can write this Builder to another Builder
         */
        public ByteAppendable asAppendable() {
            return builder -> builder.appendRaw(toByteArray());
        }

        /**
         * {@inheritDoc}
         */
        public String asString() {
            return baos.toString(UTF_8);
        }

        @Override
        public String toString() {
            return "ByteStringBuilder[" + asString() + "]";
        }

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
        ByteAppender<ByteAppendable> DEFAULT_APPENDER = ByteAppendable::appendTo;

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
        default ByteString toByteString(T object) {
            return ByteString.from(object, this);
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

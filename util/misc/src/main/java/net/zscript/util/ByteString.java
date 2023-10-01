package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;

import static java.nio.charset.StandardCharsets.UTF_8;

public interface ByteString {
    /**
     * Returns (a copy of) content of this ByteString
     *
     * @return content
     */
    byte[] toByteArray();

    /**
     * Writes this ByteString to the specified OutputStream.
     *
     * @param out the stream to write
     * @return this ByteString, to facilitate chaining
     * @throws IOException if the write fails
     */
    ByteString writeTo(final OutputStream out) throws IOException;

    /**
     * Creates a fresh builder for a ByteString
     *
     * @return a builder
     */
    static ByteStringBuilder builder() {
        return new ByteStringBuilder();
    }

    final class ImmutableByteString implements ByteString {
        private final byte[] bytes;

        private ImmutableByteString(final byte[] bytes) {
            this.bytes = bytes;
        }

        /**
         * {@inheritDoc}
         */
        public byte[] toByteArray() {
            return bytes.clone();
        }

        /**
         * {@inheritDoc}
         */
        public ImmutableByteString writeTo(final OutputStream out) throws IOException {
            out.write(bytes);
            return this;
        }

        @Override
        public String toString() {
            return "ImmutableByteString[" + new String(bytes, UTF_8) + "]";
        }

        @Override
        public final int hashCode() {
            return Arrays.hashCode(bytes);
        }

        @Override
        public final boolean equals(Object obj) {
            if (obj == this) {
                return true;
            }
            if (!(obj instanceof ImmutableByteString)) {
                return false;
            }
            return Arrays.equals(bytes, ((ImmutableByteString) obj).bytes);
        }

        private static final byte[] HEX = "0123456789abcdef".getBytes(UTF_8);
    }

    class ByteStringBuilder implements ByteString {
        protected final ByteArrayOutputStream baos = new ByteArrayOutputStream();

        /**
         * {@inheritDoc}
         */
        public ByteString writeTo(final OutputStream out) throws IOException {
            out.write(baos.toByteArray());
            return this;
        }

        /**
         * {@inheritDoc}
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
        public <B extends ByteStringBuilder> B appendByte(int b) {
            if ((b & ~0xff) != 0) {
                throw new IllegalArgumentException("Hexpair values must be 0x0-0xff: " + b);
            }
            baos.write(b);
            return asTypeB();
        }

        /**
         * Appends the supplied value as a 2 nibble hexadecimal.
         *
         * @param value any value 0x00-0xff
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range  0-ff
         */
        public <B extends ByteStringBuilder> B appendHexPair(int value) {
            if ((value & ~0xff) != 0) {
                throw new IllegalArgumentException("Hex-pair values must be 0x0-0xff: " + value);
            }
            baos.write(toHex((value >>> 4) & 0xF));
            baos.write(toHex(value & 0xF));
            return asTypeB();
        }

        /**
         * Appends a number as up to 4 nibbles of hex. All leading zeroes are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         * @throws IllegalArgumentException if value is out of range
         */
        public <B extends ByteStringBuilder> B appendNumeric(int value) {
            if (value == 0) {
                return asTypeB();
            }
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
            return asTypeB();
        }

        public <B extends ByteStringBuilder> B asTypeB() {
            return (B) this;
        }

        @Override
        public String toString() {
            return "ByteStringBuilder[" + baos.toString(UTF_8) + "]";
        }

        public ByteString build() {
            return new ImmutableByteString(toByteArray());
        }
    }

    /**
     * Simply converts a nibble value (0-15) to an ASCII hex char, lowercase.
     *
     * @param nibble a value 0-15
     * @return the corresponding hex character, '0'-'9' or 'a'-'f'
     * @throws IndexOutOfBoundsException if value is out of range
     */
    static byte toHex(int nibble) {
        return ImmutableByteString.HEX[nibble];
    }
}

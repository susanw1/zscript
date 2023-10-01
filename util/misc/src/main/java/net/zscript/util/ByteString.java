package net.zscript.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

public interface ByteString {
    byte[] toByteArray();

    ByteString writeTo(final OutputStream out) throws IOException;

    static ByteStringBuilder builder() {
        return new ByteStringBuilder();
    }

    class ImmutableByteString implements ByteString {
        private final byte[] bytes;

        private ImmutableByteString(final byte[] bytes) {
            this.bytes = bytes;
        }

        public byte[] toByteArray() {
            return bytes.clone();
        }

        public ImmutableByteString writeTo(final OutputStream out) throws IOException {
            out.write(bytes);
            return this;
        }

        private static final byte[] HEX = "0123456789abcdef".getBytes(StandardCharsets.UTF_8);
    }

    class ByteStringBuilder implements ByteString {
        private final ByteArrayOutputStream baos = new ByteArrayOutputStream();

        public ByteStringBuilder writeTo(final OutputStream out) throws IOException {
            out.write(baos.toByteArray());
            return this;
        }

        public byte[] toByteArray() {
            return baos.toByteArray();
        }

        /**
         * Appends the supplied value. Only the bottom 8 bits are added.
         *
         * @param b any value
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder appendByte(int b) {
            baos.write(b & 0xff);
            return this;
        }

        /**
         * Appends the supplied value as a 2 nibble hexadecimal. Only the bottom 8 bits are added, higher bits are ignored.
         *
         * @param value any value
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder appendHexValue(int value) {
            baos.write(toHex((value >> 4) & 0xF));
            baos.write(toHex(value & 0xF));
            return this;
        }

        /**
         * Appends a number as up to 4 nibble of hex. All leading zeroes are suppressed.
         *
         * @param value the value to append
         * @return this builder, to facilitate chaining
         */
        public ByteStringBuilder appendNumeric(int value) {
            if (value == 0) {
                return this;
            }
            if ((value & ~0xffff) != 0) {
                throw new IllegalArgumentException("Numeric fields must be uint16s: " + value);
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

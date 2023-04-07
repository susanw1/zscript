package org.zcode.javaclient.parsing;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.zcode.javaclient.components.ZcodeValidationException;

public class BigField {
    private static final Logger LOG = LoggerFactory.getLogger(BigField.class);

    private static final byte[] MANDATORY_ESCAPED_CHARS = new byte[] { '\0', '\n', '\r', '\"', '=' };

    private static final Map<Character, byte[]> ESCAPES = Map.of(
            '\0', "=00".getBytes(),
            '\n', "=0a".getBytes(),
            '\r', "=0d".getBytes(),
            '\"', "=22".getBytes(),
            '=', "=3d".getBytes());

    /**
     * This is the (un-escaped) data content to send, with text already in UTF-8.
     */
    private final byte[] rawBytes;

    enum Style {
        TEXT,
        BINARY,
        EITHER
    }

    public static BigField fromRawBytes(byte[] rawBytes) {
        return new BigField(rawBytes.clone());
    }

    public static BigField fromEscapedBytes(byte[] escapedBytes) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        try {
            for (int pos = 0, len = escapedBytes.length; pos < len; pos++) {
                if (escapedBytes[pos] == '=') {
                    if (pos + 3 >= len) {
                        throw new ZcodeValidationException("Cannot decode escaped characters", "pos", pos, "escapedBytes", escapedBytes);
                    }
                    baos.write(nibblesToHexByte(escapedBytes[pos + 1], escapedBytes[pos + 2]));
                    pos += 2;
                } else {
                    baos.write(escapedBytes[pos]);
                }
            }
        } catch (IOException e) {
            throw new ZcodeValidationException("Unexpected error parsing escapes", e, "escapedBytes", escapedBytes);
        }
        return new BigField(baos.toByteArray());
    }

    public static BigField fromText(String text) {
        return new BigField(text.getBytes(StandardCharsets.UTF_8));
    }

    public static BigFieldBuilder builder() {
        return new BigFieldBuilder();
    }

    private BigField(final byte[] rawBytes) {
        this.rawBytes = rawBytes;
    }

    public byte[] getRawBytes() {
        return rawBytes.clone();
    }

    /**
     * Generates the bytes as appropriate for transmission, with null chars, newlines, CRs, double-quotes and '=' escaped.
     * 
     * @return an escaped byte-string
     */
    public byte[] asEscapedBytes() {
        final ByteArrayOutputStream baos = new ByteArrayOutputStream();
        try {
            for (byte b : rawBytes) {
                switch (b) {
                case 0:
                case '\n':
                case '\r':
                case '\"':
                case '=':
                    baos.write(ESCAPES.get((char) b));
                    break;
                default:
                    baos.write(b);
                }
            }
        } catch (IOException e) {
            LOG.atError().setMessage("Unexpected error escaping BigField").setCause(e).log();
        }
        return baos.toByteArray();
    }

    public String asText() {
        return new String(rawBytes, StandardCharsets.UTF_8);
    }

    public byte[] asHexEncodedBinary() {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        for (byte b : rawBytes) {
            baos.write(Character.forDigit((b >> 4) & 0xf, 16));
            baos.write(Character.forDigit(b & 0xf, 16));
        }
        return baos.toByteArray();
    }

    private static int charToHexDigit(byte b) throws IOException {
        if (b >= '0' && b <= '9')
            return b - '0';
        else if (b >= 'a' && b <= 'f') {
            return b - ('a' + 10);
        } else {
            throw new IOException("Not a legal hex digit: " + b);
        }
    }

    private static int nibblesToHexByte(byte b1, byte b2) throws IOException {
        return (charToHexDigit(b1) << 4) | charToHexDigit(b2);
    }

    public static class BigFieldBuilder {
        private ByteArrayOutputStream big;
        private int                   pos;

        private byte    currentHexByte;
        private boolean inNibble;

        public BigFieldBuilder() {
            this.big = new ByteArrayOutputStream();
            reset();
        }

        public BigField build() {
            return BigField.fromRawBytes(big.toByteArray());
        }

        public final void reset() {
            this.big.reset();
            this.pos = 0;
            this.currentHexByte = 0;
            this.inNibble = false;
        }

        public byte[] getData() {
            return big.toByteArray();
        }

        public int getLength() {
            return pos;
        }

        public boolean addByte(byte c) {
            if (inNibble) {
                return false;
            }
            big.write(c & 0xFF);
            return true;
        }

        public boolean addNibble(byte nibble) {
            if (inNibble) {
                big.write((currentHexByte << 4 | nibble) & 0xFF);
                currentHexByte = 0;
            } else {
                currentHexByte = nibble;
            }
            inNibble = !inNibble;
            return true;
        }

        public boolean isInNibble() {
            return inNibble;
        }

    }
}

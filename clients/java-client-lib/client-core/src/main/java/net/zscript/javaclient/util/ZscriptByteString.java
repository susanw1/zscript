package net.zscript.javaclient.util;

import java.nio.charset.StandardCharsets;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

/**
 * Specialized ByteString whose builder has added utility methods for constructing Zscript strings.
 */
public interface ZscriptByteString extends ByteString {
    /**
     * Creates a new, empty builder to construct ZscriptByteStrings.
     *
     * @return a new builder
     */
    static ZscriptByteStringBuilder builder() {
        return new ZscriptByteStringBuilder();
    }

    /**
     * Zscript-oriented builder.
     */
    class ZscriptByteStringBuilder extends ByteStringBuilder {
        /**
         * @param field a single byte representing the Zscript field-name to add
         * @param value the numeric 16-bit value (0-0xffff) to append to the field-name
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendField(byte field, int value) {
            return appendByte(field).appendNumeric(value);
        }

        /**
         * @param field a single byte representing the Zscript field-name to add
         * @param value the numeric 32-bit value (0-0xffffffff) to append to the field-name
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendField32(byte field, long value) {
            return appendByte(field).appendNumeric32(value);
        }

        /**
         * @param bytes the bytes to be added as a new big-field value. This method adds the bytes as UTF-8 with enclosing quotes and appropriate escaping
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendBigfieldText(byte[] bytes) {
            appendByte(Zchars.Z_BIGFIELD_QUOTED);
            for (byte b : bytes) {
                if (Zchars.mustStringEscape(b)) {
                    appendByte(Zchars.Z_STRING_ESCAPE).appendHexPair(b);
                } else {
                    appendByte(b);
                }
            }
            appendByte(Zchars.Z_BIGFIELD_QUOTED);
            return this;
        }

        /**
         * @param text the text to be added as a new big-field value. This method encodes the bytes as UTF-8 with enclosing quotes and appropriate escaping
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendBigfieldText(String text) {
            return appendBigfieldText(strToBytes(text));
        }

        /**
         * @param bytes the bytes to be added as a new big-field hex-pair sequence. This method writes the HEX prefix '+' and a hex-pair for each byte.
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendBigfieldBytes(byte[] bytes) {
            appendByte(Zchars.Z_BIGFIELD_HEX);
            for (byte b : bytes) {
                appendHexPair(b);
            }
            return this;
        }

        /**
         * @param text the bytes to be added as a new big-field hex-pair sequence. This method writes the HEX prefix '+' and a hex-pair for each byte in the text, interpreted as
         *             UTF-8.
         * @return this builder, to facilitate chaining
         */
        public ZscriptByteStringBuilder appendBigfieldBytes(String text) {
            return appendBigfieldBytes(strToBytes(text));
        }

        private static byte[] strToBytes(String value) {
            return value.getBytes(StandardCharsets.UTF_8);
        }
    }
}

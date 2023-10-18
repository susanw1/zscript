package net.zscript.javaclient;

import java.nio.charset.StandardCharsets;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString;

public interface ZscriptByteString extends ByteString {
    static ZscriptByteStringBuilder builder() {
        return new ZscriptByteStringBuilder();
    }

    class ZscriptByteStringBuilder extends ByteStringBuilder {
        public ZscriptByteStringBuilder appendField(byte field, int value) {
            return appendByte(field).appendNumeric(value);
        }

        public ZscriptByteStringBuilder appendField32(byte field, long value) {
            return appendByte(field).appendNumeric32(value);
        }

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

        public ZscriptByteStringBuilder appendBigfieldText(String text) {
            return appendBigfieldText(strToBytes(text));
        }

        public ZscriptByteStringBuilder appendBigfieldBytes(byte[] bytes) {
            appendByte(Zchars.Z_BIGFIELD_HEX);
            for (byte b : bytes) {
                appendHexPair(b);
            }
            return this;
        }

        public ZscriptByteStringBuilder appendBigfieldBytes(String text) {
            return appendBigfieldBytes(strToBytes(text));
        }

        private static byte[] strToBytes(String value) {
            return value.getBytes(StandardCharsets.UTF_8);
        }
    }
}

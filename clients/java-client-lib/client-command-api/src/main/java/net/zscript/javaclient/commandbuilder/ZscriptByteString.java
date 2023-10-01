package net.zscript.javaclient.commandbuilder;

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

        public ZscriptByteStringBuilder appendText(byte[] bytes) {
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

        public ZscriptByteStringBuilder appendText(String text) {
            return appendText(strToBytes(text));
        }

        public ZscriptByteStringBuilder appendBytes(byte[] bytes) {
            appendByte(Zchars.Z_BIGFIELD_HEX);
            for (byte b : bytes) {
                appendHexPair(b);
            }
            return this;
        }

        public ZscriptByteStringBuilder appendBytes(String text) {
            return appendBytes(strToBytes(text));
        }

        private static byte[] strToBytes(String value) {
            return value.getBytes(StandardCharsets.UTF_8);
        }
    }
}
